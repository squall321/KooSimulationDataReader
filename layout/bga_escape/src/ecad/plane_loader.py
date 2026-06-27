# ODB++ features 파일에서 plane (PowerGround) 폴리곤을 regex로 파싱하는 로더
"""Python-side plane geometry loader for ODB++ features files.

The C++ ECAD reader exposes plane polygons via Layer::getFeatures()
but has no Python binding yet. This module is a lightweight regex
parser sufficient for the Phase C return-path metrics. Limitations:

- Arcs (OC records) are linearized as straight chords. Good enough
  for point-in-polygon return-path heuristics; not for DRC.
- No Cu/Net association — ODB++ embeds net info in the EDA tree, not
  the features file. Net labelling for plane polygons is best-effort.
- Compressed features files (.Z / .z) are decompressed via zlib.
"""

from __future__ import annotations

import re
import zlib
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Tuple


XY = Tuple[float, float]


@dataclass(frozen=True)
class PlanePolygon:
    layer_name: str
    net_name: Optional[str]
    outer_xy_mm: Tuple[XY, ...]
    holes_xy_mm: Tuple[Tuple[XY, ...], ...]
    is_positive: bool


@dataclass(frozen=True)
class PlaneLayer:
    layer_name: str
    polygons: Tuple[PlanePolygon, ...]


@dataclass(frozen=True)
class PlaneGeometry:
    layers: Dict[str, PlaneLayer] = field(default_factory=dict)
    units_mm: bool = True


# ---------------------------------------------------------------------------
# Features parsing
# ---------------------------------------------------------------------------


_S_RE  = re.compile(r'^\s*S\s+([PN])')
_OB_RE = re.compile(r'^\s*OB\s+(-?[\d.eE+-]+)\s+(-?[\d.eE+-]+)')
_OS_RE = re.compile(r'^\s*OS\s+(-?[\d.eE+-]+)\s+(-?[\d.eE+-]+)')
_OE_RE = re.compile(r'^\s*OE\b')
_IB_RE = re.compile(r'^\s*IB\s+(-?[\d.eE+-]+)\s+(-?[\d.eE+-]+)')
_IE_RE = re.compile(r'^\s*IE\b')
_SE_RE = re.compile(r'^\s*SE\b')
_UNITS_RE = re.compile(r'(?i)^\s*UNITS\s*=\s*(MM|INCH)')


def _decompress_if_needed(path: Path) -> str:
    raw = path.read_bytes()
    if path.suffix.lower() in ('.z',) or raw.startswith(b'\x1f\x9d'):
        # ODB++ uses Lempel-Ziv-Welch via the legacy 'compress(1)' format
        # for .z; not zlib. zlib.decompress will fail on that, so try and
        # fall back to raw text.
        try:
            return zlib.decompress(raw).decode('utf-8', errors='replace')
        except zlib.error:
            return raw.decode('latin1', errors='replace')
    try:
        return raw.decode('utf-8', errors='replace')
    except UnicodeDecodeError:
        return raw.decode('latin1', errors='replace')


def parse_features_file(text: str, layer_name: str) -> Tuple[PlanePolygon, ...]:
    """Parse the S/OB/OS/OE/IB/IE/SE grammar; return polygons."""
    polys: List[PlanePolygon] = []

    in_surface = False
    is_positive = True
    cur_outer: List[XY] = []
    cur_island: List[XY] = []
    cur_holes: List[Tuple[XY, ...]] = []
    in_island = False

    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith('#'):
            continue

        m = _S_RE.match(line)
        if m:
            in_surface = True
            is_positive = (m.group(1) == 'P')
            cur_outer = []
            cur_island = []
            cur_holes = []
            in_island = False
            continue

        if not in_surface:
            continue

        if _SE_RE.match(line):
            if cur_outer:
                polys.append(PlanePolygon(
                    layer_name=layer_name,
                    net_name=None,
                    outer_xy_mm=tuple(cur_outer),
                    holes_xy_mm=tuple(cur_holes),
                    is_positive=is_positive,
                ))
            in_surface = False
            continue

        m = _OB_RE.match(line)
        if m:
            in_island = False
            cur_outer = [(float(m.group(1)), float(m.group(2)))]
            continue

        m = _IB_RE.match(line)
        if m:
            in_island = True
            cur_island = [(float(m.group(1)), float(m.group(2)))]
            continue

        if _OE_RE.match(line):
            # End of outer; nothing to flush — wait for SE.
            continue

        if _IE_RE.match(line):
            if cur_island:
                cur_holes.append(tuple(cur_island))
            cur_island = []
            in_island = False
            continue

        m = _OS_RE.match(line)
        if m:
            pt = (float(m.group(1)), float(m.group(2)))
            if in_island:
                cur_island.append(pt)
            else:
                cur_outer.append(pt)
            continue

    return tuple(polys)


def load_plane_geometry(odb_root: str | Path,
                          step: str = 'mentor',
                          plane_layer_names: Iterable[str] = (
                              'GND', 'VDD', 'VSS', 'VCC', 'PWR')
                          ) -> PlaneGeometry:
    """Walk steps/<step>/layers/<plane>/features for each candidate plane.

    Returns an empty PlaneGeometry (layers={}) when nothing is found —
    callers check `.layers`. Never raises on missing files.
    """
    root = Path(odb_root)
    layers_dir = root / 'steps' / step / 'layers'
    out: Dict[str, PlaneLayer] = {}

    if not layers_dir.exists():
        return PlaneGeometry(layers={}, units_mm=True)

    # Walk every layer dir; we don't know the exact names in advance,
    # so we accept any layer whose name contains one of the candidate
    # substrings (case-insensitive).
    candidates = set(s.upper() for s in plane_layer_names)
    for layer_dir in layers_dir.iterdir():
        if not layer_dir.is_dir():
            continue
        name = layer_dir.name
        if not any(c in name.upper() for c in candidates):
            continue
        feat = None
        for fname in ('features', 'features.z', 'features.Z'):
            cand = layer_dir / fname
            if cand.exists():
                feat = cand
                break
        if feat is None:
            continue
        try:
            text = _decompress_if_needed(feat)
        except OSError:
            continue
        polys = parse_features_file(text, layer_name=name)
        if polys:
            out[name] = PlaneLayer(layer_name=name, polygons=polys)

    return PlaneGeometry(layers=out, units_mm=True)


# ---------------------------------------------------------------------------
# Point-in-polygon queries
# ---------------------------------------------------------------------------


def _point_in_ring(x: float, y: float, ring: Tuple[XY, ...]) -> bool:
    """Standard ray-casting point-in-polygon."""
    inside = False
    n = len(ring)
    if n < 3:
        return False
    j = n - 1
    for i in range(n):
        xi, yi = ring[i]
        xj, yj = ring[j]
        intersect = ((yi > y) != (yj > y)) and (
            x < (xj - xi) * (y - yi) / (yj - yi + 1e-30) + xi)
        if intersect:
            inside = not inside
        j = i
    return inside


def is_point_inside_plane(geom: PlaneGeometry, layer_name: str,
                            x_mm: float, y_mm: float) -> bool:
    layer = geom.layers.get(layer_name)
    if layer is None:
        return False
    inside_any_positive = False
    for poly in layer.polygons:
        if not poly.is_positive:
            continue
        if _point_in_ring(x_mm, y_mm, poly.outer_xy_mm):
            # Inside the outer boundary — check holes
            in_hole = any(_point_in_ring(x_mm, y_mm, h)
                            for h in poly.holes_xy_mm)
            if not in_hole:
                inside_any_positive = True
                break
    # Subtract negative polygons (anti-pad voids)
    if inside_any_positive:
        for poly in layer.polygons:
            if poly.is_positive:
                continue
            if _point_in_ring(x_mm, y_mm, poly.outer_xy_mm):
                return False
    return inside_any_positive


def find_nearest_return_via_distance(geom: PlaneGeometry,
                                       signal_xy: Iterable[XY],
                                       vias_xy: Iterable[XY]) -> Optional[float]:
    """Min distance from any signal sample point to any via location.
    Returns None if no vias supplied."""
    vias = list(vias_xy)
    if not vias:
        return None
    best = None
    for sx, sy in signal_xy:
        for vx, vy in vias:
            d2 = (sx - vx) ** 2 + (sy - vy) ** 2
            if best is None or d2 < best:
                best = d2
    return None if best is None else best ** 0.5
