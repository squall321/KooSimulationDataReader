# PCB 스택업 (signal/dielectric 적층 + 두께/εr) 로더 + 신호층 분류 헬퍼
"""Stackup loader for the BGA router eval pipeline.

Reads the YAML schema already in use by the C++ side
(``examples/ECAD/p3_stackup.yaml``). Exposes per-signal-layer helpers
that the Wadell Z0 formulas in ``si.py`` need: dielectric_below,
copper_thickness, microstrip vs stripline classification.

If a dataset has no stackup YAML registered, callers fall back to
``default_stackup()`` (4-layer FR-4) and the ``is_default`` flag
propagates into the result JSON so readers can mark Z0 numbers as
nominal.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable, Optional, Tuple

import yaml


class StackupError(ValueError):
    """Raised when the YAML is malformed or inconsistent."""


@dataclass(frozen=True)
class LayerEntry:
    name: str
    thickness_mm: float
    kind: str                       # 'copper' | 'prepreg' | 'core' | 'soldermask'
    material: Optional[str] = None
    er: Optional[float] = None      # only for dielectrics
    loss_tangent: Optional[float] = None


@dataclass(frozen=True)
class StackupSpec:
    layers: Tuple[LayerEntry, ...]              # top-to-bottom
    source_path: Optional[str] = None
    is_default: bool = False

    # --- introspection ---
    def signal_layer_names(self) -> Tuple[str, ...]:
        return tuple(L.name for L in self.layers if L.kind == 'copper')

    def _index_of(self, layer_name: str) -> Optional[int]:
        for i, L in enumerate(self.layers):
            if L.name == layer_name:
                return i
        return None

    def dielectric_below(self, signal_layer: str) -> Optional[LayerEntry]:
        """The prepreg / core directly under this signal — used as `h`."""
        i = self._index_of(signal_layer)
        if i is None:
            return None
        for L in self.layers[i + 1:]:
            if L.kind in ('prepreg', 'core'):
                return L
            if L.kind == 'copper':
                # Adjacent copper means we crossed a plane already; the
                # dielectric we want is the one just below.
                return None
        return None

    def dielectric_above(self, signal_layer: str) -> Optional[LayerEntry]:
        i = self._index_of(signal_layer)
        if i is None:
            return None
        for L in reversed(self.layers[:i]):
            if L.kind in ('prepreg', 'core'):
                return L
            if L.kind == 'copper':
                return None
        return None

    def copper_thickness_mm(self, signal_layer: str) -> Optional[float]:
        i = self._index_of(signal_layer)
        if i is None or self.layers[i].kind != 'copper':
            return None
        return self.layers[i].thickness_mm

    def microstrip_layer(self, signal_layer: str) -> bool:
        """True iff this signal is the outermost copper (no dielectric
        on one of its sides)."""
        signals = [i for i, L in enumerate(self.layers) if L.kind == 'copper']
        if not signals:
            return False
        i = self._index_of(signal_layer)
        if i is None:
            return False
        return i == signals[0] or i == signals[-1]

    def stripline_layer(self, signal_layer: str) -> bool:
        """True iff this signal has dielectric on BOTH sides (interior)."""
        i = self._index_of(signal_layer)
        if i is None or self.layers[i].kind != 'copper':
            return False
        return (self.dielectric_above(signal_layer) is not None
                and self.dielectric_below(signal_layer) is not None
                and not self.microstrip_layer(signal_layer))


# ---------------------------------------------------------------------------
# Loaders
# ---------------------------------------------------------------------------


_KIND_MAP = {
    'copper': 'copper',
    'prepreg': 'prepreg',
    'core': 'core',
    'soldermask': 'soldermask',
    'solder_mask': 'soldermask',
    'sm': 'soldermask',
}


def load_stackup_yaml(path: str | Path) -> StackupSpec:
    p = Path(path)
    if not p.exists():
        raise StackupError(f'stackup yaml not found: {p}')
    try:
        data = yaml.safe_load(p.read_text())
    except yaml.YAMLError as e:
        raise StackupError(f'malformed yaml at {p}: {e}') from e
    if not isinstance(data, dict) or 'layers' not in data:
        raise StackupError(f'{p}: top-level must be a mapping with "layers" key')
    raw_layers = data['layers']
    if not isinstance(raw_layers, list) or not raw_layers:
        raise StackupError(f'{p}: "layers" must be a non-empty list')

    entries = []
    for i, raw in enumerate(raw_layers):
        if not isinstance(raw, dict):
            raise StackupError(f'{p}: layer #{i} is not a mapping')
        name = raw.get('name')
        if not name:
            raise StackupError(f'{p}: layer #{i} missing "name"')
        kind = raw.get('type')
        if kind not in _KIND_MAP:
            raise StackupError(
                f'{p}: layer {name!r} has unknown type {kind!r}; '
                f'expected one of {sorted(_KIND_MAP)}')
        thickness = raw.get('thickness')
        if not isinstance(thickness, (int, float)) or thickness <= 0:
            raise StackupError(
                f'{p}: layer {name!r} thickness must be a positive number')
        entries.append(LayerEntry(
            name=str(name),
            thickness_mm=float(thickness),
            kind=_KIND_MAP[kind],
            material=raw.get('material'),
            er=raw.get('dielectric_constant'),
            loss_tangent=raw.get('loss_tangent'),
        ))

    # Sanity — copper must be separated by at least one dielectric.
    last_copper_idx = -2
    for i, L in enumerate(entries):
        if L.kind == 'copper':
            if i - last_copper_idx < 2:
                raise StackupError(
                    f'{p}: copper layers at index {last_copper_idx} and {i} '
                    f'must be separated by at least one dielectric')
            last_copper_idx = i

    return StackupSpec(layers=tuple(entries), source_path=str(p),
                        is_default=False)


def default_stackup() -> StackupSpec:
    """Generic 4-signal-layer FR-4 fallback when no YAML is registered."""
    layers = (
        LayerEntry('COMP',  0.035, 'copper',   material='Cu'),
        LayerEntry('pp1',   0.10,  'prepreg',  material='FR4', er=4.2, loss_tangent=0.02),
        LayerEntry('LAY2',  0.035, 'copper',   material='Cu'),
        LayerEntry('core1', 0.20,  'core',     material='FR4', er=4.5, loss_tangent=0.018),
        LayerEntry('LAY3',  0.035, 'copper',   material='Cu'),
        LayerEntry('pp2',   0.10,  'prepreg',  material='FR4', er=4.2, loss_tangent=0.02),
        LayerEntry('LAY4',  0.035, 'copper',   material='Cu'),
    )
    return StackupSpec(layers=layers, source_path=None, is_default=True)


def load_for_dataset(dataset_entry, registry=None) -> StackupSpec:
    """Resolve the stackup for a dataset via registry key ``stackup_yaml``.

    ``dataset_entry`` may be a dict (raw registry row) or any object with
    a ``.path`` attribute (legacy DatasetEntry). On absence, returns
    ``default_stackup()``. Never raises — falls back on any failure.
    """
    if dataset_entry is None:
        return default_stackup()
    yaml_path = None
    if isinstance(dataset_entry, dict):
        yaml_path = dataset_entry.get('stackup_yaml')
    else:
        yaml_path = getattr(dataset_entry, 'stackup_yaml', None)
    if not yaml_path:
        return default_stackup()
    p = Path(yaml_path)
    if not p.is_absolute():
        # Relative paths interpreted relative to the dataset root if
        # available.
        root = (dataset_entry.get('path') if isinstance(dataset_entry, dict)
                else getattr(dataset_entry, 'path', None))
        if root:
            p = Path(root) / yaml_path
    try:
        return load_stackup_yaml(p)
    except StackupError:
        return default_stackup()
