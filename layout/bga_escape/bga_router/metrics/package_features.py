# EDA 데이터에서 패키지(부품)의 물리 상태와 상호 관계 피처를 추출하는 순수 모듈
"""Package-level physical + relational features for the ML meta-model.

The router already parses ODB++ ``eda/data`` (see ``src.eda_parser``) into
``EdaData`` — a package library plus placed components. This module turns
that into a flat set of *per-package instances* and the pairwise / neighbour
relations between them, which become inputs to a downstream meta-model.

Design notes:
    - All geometry is in millimetres, matching the parser output.
    - Board-frame bounding boxes are approximated as the package footprint
      size centred on the component placement coordinate (the parser stores
      footprint bbox in *local* coords only). This is the convention the
      caller asked for; it is exact when the placement point is the
      footprint centre and a close approximation otherwise.
    - Component ``mirrored`` from Mentor is unreliable for side detection.
      The authoritative source is which ``comp_+_{top,bot}/components`` file
      lists the refDes; ``resolve_side`` reads those, and ``build_packages``
      falls back to ``mirrored`` only when the files are absent (flagging
      ``side_source``).

Only stdlib + ``math`` are used.
"""

from __future__ import annotations

import math
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Tuple


BBox = Tuple[float, float, float, float]  # (xmin, ymin, xmax, ymax)


@dataclass
class PackageInstance:
    """One placed package with its footprint geometry resolved to mm."""
    ref_des: str
    pkg_name: str
    side: str            # 'TOP' | 'BOT'
    side_source: str     # 'comp_layer' | 'mirrored'
    cx_mm: float         # board-frame placement x
    cy_mm: float         # board-frame placement y
    width_mm: float
    height_mm: float
    area_mm2: float
    pin_count: int
    pitch_mm: float
    rotation_deg: float


# --------------------------------------------------------------------------- #
# Side resolution                                                             #
# --------------------------------------------------------------------------- #

def resolve_side(eda_path: str | Path) -> Dict[str, Tuple[str, str]]:
    """Map refDes → ('TOP'|'BOT', source) using the per-side components files.

    ``eda_path`` points at ``.../eda/data``; the sibling layers live under
    ``<eda_path>/../../layers``. ``comp_+_top/components`` lists TOP-side
    refDes, ``comp_+_bot/components`` lists BOT-side. A refDes present in a
    file gets source ``'comp_layer'``. Missing files simply contribute
    nothing — the caller then falls back to ``mirrored``.
    """
    layers_dir = Path(eda_path).parent.parent / 'layers'
    result: Dict[str, Tuple[str, str]] = {}
    for side, dirname in (('TOP', 'comp_+_top'), ('BOT', 'comp_+_bot')):
        cfile = layers_dir / dirname / 'components'
        if not cfile.is_file():
            continue
        for ref in _iter_component_refs(cfile):
            # TOP is scanned first; don't let a stray BOT duplicate override.
            if ref not in result:
                result[ref] = (side, 'comp_layer')
    return result


def _iter_component_refs(cfile: Path):
    """Yield refDes tokens from CMP lines of a components file.

    CMP layout: ``CMP <pkg_idx> <x> <y> <rot> <mir> <refDes> [<comp_name>]``.
    We pad each line with spaces and match the refDes as a whitespace-
    delimited token so partial names never collide.
    """
    text = cfile.read_text(errors='replace')
    for raw in text.splitlines():
        line = raw.strip()
        if not line.startswith('CMP'):
            continue
        parts = line.split()
        if len(parts) < 7:
            continue
        # Guard against garbled lines: field 6 is the refDes token.
        ref = parts[6]
        yield ref


# --------------------------------------------------------------------------- #
# Build instances                                                            #
# --------------------------------------------------------------------------- #

def build_packages(eda, eda_path: Optional[str | Path] = None
                   ) -> List[PackageInstance]:
    """Turn ``EdaData`` (components + package library) into PackageInstances.

    ``width/height/area`` come from the package footprint bbox; ``cx/cy``
    come from the component placement coordinate. ``side`` uses
    ``resolve_side`` when ``eda_path`` is given and the refDes is found;
    otherwise it falls back to ``mirrored`` (True→'BOT', False→'TOP') and
    marks ``side_source='mirrored'``.
    """
    side_map = resolve_side(eda_path) if eda_path is not None else {}

    out: List[PackageInstance] = []
    for c in eda.components:
        pkg = eda.packages.get(c.pkg_name)
        if pkg is None:
            continue
        width = pkg.bbox_max_x_mm - pkg.bbox_min_x_mm
        height = pkg.bbox_max_y_mm - pkg.bbox_min_y_mm
        area = width * height

        resolved = side_map.get(c.ref_des)
        if resolved is not None:
            side, source = resolved
        else:
            side = 'BOT' if c.mirrored else 'TOP'
            source = 'mirrored'

        out.append(PackageInstance(
            ref_des=c.ref_des,
            pkg_name=c.pkg_name,
            side=side,
            side_source=source,
            cx_mm=c.x_mm,
            cy_mm=c.y_mm,
            width_mm=width,
            height_mm=height,
            area_mm2=area,
            pin_count=len(pkg.pins),
            pitch_mm=pkg.pitch_mm,
            rotation_deg=c.rotation_deg,
        ))
    return out


# --------------------------------------------------------------------------- #
# Intrinsic features                                                         #
# --------------------------------------------------------------------------- #

def package_intrinsic(pkg: PackageInstance) -> dict:
    """All instance fields plus derived ``aspect_ratio`` (long/short dim)."""
    mn = min(pkg.width_mm, pkg.height_mm)
    mx = max(pkg.width_mm, pkg.height_mm)
    aspect = (mx / mn) if mn > 0 else 0.0
    return {
        'ref_des': pkg.ref_des,
        'pkg_name': pkg.pkg_name,
        'side': pkg.side,
        'side_source': pkg.side_source,
        'cx_mm': pkg.cx_mm,
        'cy_mm': pkg.cy_mm,
        'width_mm': pkg.width_mm,
        'height_mm': pkg.height_mm,
        'area_mm2': pkg.area_mm2,
        'pin_count': pkg.pin_count,
        'pitch_mm': pkg.pitch_mm,
        'rotation_deg': pkg.rotation_deg,
        'aspect_ratio': aspect,
    }


# --------------------------------------------------------------------------- #
# Geometry helpers                                                           #
# --------------------------------------------------------------------------- #

def bbox_of(pkg: PackageInstance) -> BBox:
    """Board-frame bbox: placement centre expanded by width/height."""
    hw = pkg.width_mm / 2.0
    hh = pkg.height_mm / 2.0
    return (pkg.cx_mm - hw, pkg.cy_mm - hh, pkg.cx_mm + hw, pkg.cy_mm + hh)


def bbox_overlap_area(a_bbox: BBox, b_bbox: BBox) -> float:
    """Overlap area of two bboxes; 0 when they do not intersect."""
    dx = min(a_bbox[2], b_bbox[2]) - max(a_bbox[0], b_bbox[0])
    dy = min(a_bbox[3], b_bbox[3]) - max(a_bbox[1], b_bbox[1])
    if dx <= 0.0 or dy <= 0.0:
        return 0.0
    return dx * dy


def center_distance_mm(a: PackageInstance, b: PackageInstance) -> float:
    """Euclidean distance between two placement centres."""
    return math.hypot(a.cx_mm - b.cx_mm, a.cy_mm - b.cy_mm)


def edge_distance_mm(a: PackageInstance, b: PackageInstance) -> float:
    """Shortest gap between the two board-frame bboxes; 0 when overlapping."""
    ab, bb = bbox_of(a), bbox_of(b)
    dx = max(ab[0] - bb[2], bb[0] - ab[2], 0.0)
    dy = max(ab[1] - bb[3], bb[1] - ab[3], 0.0)
    return math.hypot(dx, dy)


def _find(pkgs: List[PackageInstance], ref: str) -> Optional[PackageInstance]:
    for p in pkgs:
        if p.ref_des == ref:
            return p
    return None


# --------------------------------------------------------------------------- #
# Relations                                                                  #
# --------------------------------------------------------------------------- #

def neighbors(pkgs: List[PackageInstance], ref: str, radius_mm: float,
              same_side: bool = True) -> List[dict]:
    """Packages whose centre lies within ``radius_mm`` of ``ref``.

    Each entry: ``{ref_des, center_dist, edge_dist, side}``. When
    ``same_side`` only packages on the same side as ``ref`` are returned.
    Sorted by ascending centre distance.
    """
    a = _find(pkgs, ref)
    if a is None:
        return []
    out: List[dict] = []
    for b in pkgs:
        if b.ref_des == ref:
            continue
        if same_side and b.side != a.side:
            continue
        cd = center_distance_mm(a, b)
        if cd > radius_mm:
            continue
        out.append({
            'ref_des': b.ref_des,
            'center_dist': cd,
            'edge_dist': edge_distance_mm(a, b),
            'side': b.side,
        })
    out.sort(key=lambda d: d['center_dist'])
    return out


def opposite_side_packages(pkgs: List[PackageInstance], ref: str) -> List[dict]:
    """All packages on the side opposite to ``ref``.

    Each entry: ``{ref_des, center_dist, overlap_area_mm2, overlap_ratio}``
    where ``overlap_ratio = overlap / min(area_a, area_b)``. Sorted by
    ascending centre distance.
    """
    a = _find(pkgs, ref)
    if a is None:
        return []
    ab = bbox_of(a)
    out: List[dict] = []
    for b in pkgs:
        if b.side == a.side:
            continue
        overlap = bbox_overlap_area(ab, bbox_of(b))
        denom = min(a.area_mm2, b.area_mm2)
        ratio = (overlap / denom) if denom > 0 else 0.0
        out.append({
            'ref_des': b.ref_des,
            'center_dist': center_distance_mm(a, b),
            'overlap_area_mm2': overlap,
            'overlap_ratio': ratio,
        })
    out.sort(key=lambda d: d['center_dist'])
    return out


def pairwise(pkgs: List[PackageInstance], a_ref: str, b_ref: str) -> dict:
    """Pairwise relation between two named packages.

    Returns ``{center_dist, edge_dist, overlap_area, same_side, size_ratio}``
    where ``size_ratio`` is larger area / smaller area. Empty dict if either
    refDes is missing.
    """
    a = _find(pkgs, a_ref)
    b = _find(pkgs, b_ref)
    if a is None or b is None:
        return {}
    areas = sorted((a.area_mm2, b.area_mm2))
    size_ratio = (areas[1] / areas[0]) if areas[0] > 0 else 0.0
    return {
        'center_dist': center_distance_mm(a, b),
        'edge_dist': edge_distance_mm(a, b),
        'overlap_area': bbox_overlap_area(bbox_of(a), bbox_of(b)),
        'same_side': a.side == b.side,
        'size_ratio': size_ratio,
    }


def board_context(pkgs: List[PackageInstance], ref: str,
                  board_bbox: BBox) -> dict:
    """Package position relative to the whole board.

    Returns ``{area_fraction, dist_to_edge_mm}`` where ``area_fraction`` is
    the package footprint area over the board area, and ``dist_to_edge_mm``
    is the shortest gap from the package bbox to the board boundary
    (negative when the package overhangs the board).
    """
    a = _find(pkgs, ref)
    if a is None:
        return {}
    bxmin, bymin, bxmax, bymax = board_bbox
    board_area = (bxmax - bxmin) * (bymax - bymin)
    area_fraction = (a.area_mm2 / board_area) if board_area > 0 else 0.0
    axmin, aymin, axmax, aymax = bbox_of(a)
    dist_to_edge = min(
        axmin - bxmin,
        bxmax - axmax,
        aymin - bymin,
        bymax - aymax,
    )
    return {
        'area_fraction': area_fraction,
        'dist_to_edge_mm': dist_to_edge,
    }


# --------------------------------------------------------------------------- #
# Summary                                                                    #
# --------------------------------------------------------------------------- #

def summarize_packages(pkgs: List[PackageInstance],
                       board_bbox: Optional[BBox] = None) -> dict:
    """Whole-board package summary.

    ``stacked_pairs`` lists every TOP/BOT pair whose footprints overlap,
    each ``{top_ref, bot_ref, overlap_area, center_dist, overlap_ratio}``
    sorted by descending ``overlap_ratio``. ``board_bbox`` is accepted for
    API symmetry (per-package board context is available via
    ``board_context``) but does not change the summary shape.
    """
    by_side = {'TOP': 0, 'BOT': 0}
    for p in pkgs:
        if p.side in by_side:
            by_side[p.side] += 1

    tops = [p for p in pkgs if p.side == 'TOP']
    bots = [p for p in pkgs if p.side == 'BOT']
    stacked: List[dict] = []
    for t in tops:
        tb = bbox_of(t)
        for b in bots:
            overlap = bbox_overlap_area(tb, bbox_of(b))
            if overlap <= 0.0:
                continue
            denom = min(t.area_mm2, b.area_mm2)
            ratio = (overlap / denom) if denom > 0 else 0.0
            stacked.append({
                'top_ref': t.ref_des,
                'bot_ref': b.ref_des,
                'overlap_area': overlap,
                'center_dist': center_distance_mm(t, b),
                'overlap_ratio': ratio,
            })
    stacked.sort(key=lambda d: d['overlap_ratio'], reverse=True)

    return {
        'package_count': len(pkgs),
        'by_side': by_side,
        'packages': [package_intrinsic(p) for p in pkgs],
        'stacked_pairs': stacked,
    }
