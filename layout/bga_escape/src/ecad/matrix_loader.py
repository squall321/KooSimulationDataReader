# ODB++ matrix 파일에서 layer type 분류 정보를 추출하는 regex 파서
"""Phase D-3 — matrix file loader.

The ODB++ matrix file at ``<root>/matrix/matrix`` declares each layer's
TYPE (SIGNAL, POWER_GROUND, MIXED, COMPONENT, DRILL, etc.). The C++
ECAD reader parses this fully; the Python side only needs the subset
required to identify plane layers without naming heuristics.
"""

from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple


_LAYER_BLOCK_RE = re.compile(r'LAYER\s*\{([^}]*)\}', re.DOTALL)
_KV_RE = re.compile(r'^\s*(\w+)\s*=\s*([^\r\n]*)$', re.MULTILINE)


@dataclass(frozen=True)
class MatrixLayer:
    name: str
    type: str           # 'SIGNAL' | 'POWER_GROUND' | 'MIXED' | ...
    context: Optional[str] = None
    polarity: Optional[str] = None
    row: Optional[int] = None
    old_name: Optional[str] = None


@dataclass(frozen=True)
class MatrixInfo:
    layers: Tuple[MatrixLayer, ...]
    source_path: Optional[str] = None

    def by_name(self, name: str) -> Optional[MatrixLayer]:
        lname = name.lower()
        for L in self.layers:
            if L.name.lower() == lname:
                return L
        return None

    def plane_layer_names(self) -> Tuple[str, ...]:
        """Names of POWER_GROUND or MIXED-type layers (plane fills)."""
        return tuple(L.name for L in self.layers
                      if L.type in ('POWER_GROUND', 'MIXED'))

    def signal_layer_names(self) -> Tuple[str, ...]:
        return tuple(L.name for L in self.layers if L.type == 'SIGNAL')


def parse_matrix_text(text: str, source_path: Optional[str] = None
                        ) -> MatrixInfo:
    layers: List[MatrixLayer] = []
    for block_match in _LAYER_BLOCK_RE.finditer(text):
        body = block_match.group(1)
        kv: Dict[str, str] = {}
        for m in _KV_RE.finditer(body):
            kv[m.group(1).upper()] = m.group(2).strip()
        name = kv.get('NAME')
        type_ = kv.get('TYPE')
        if not name or not type_:
            continue
        row_str = kv.get('ROW')
        try:
            row = int(row_str) if row_str else None
        except ValueError:
            row = None
        layers.append(MatrixLayer(
            name=name,
            type=type_,
            context=kv.get('CONTEXT'),
            polarity=kv.get('POLARITY'),
            row=row,
            old_name=kv.get('OLD_NAME') or None,
        ))
    return MatrixInfo(layers=tuple(layers), source_path=source_path)


def load_matrix(odb_root: str | Path) -> Optional[MatrixInfo]:
    """Read <root>/matrix/matrix. Returns None if file missing."""
    p = Path(odb_root) / 'matrix' / 'matrix'
    if not p.exists():
        return None
    try:
        text = p.read_text(errors='replace')
    except OSError:
        return None
    return parse_matrix_text(text, source_path=str(p))
