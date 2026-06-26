# BGA 라우팅 평가 메트릭 — geometry / verifier / si / stackup / standards 공용 진입점
"""Public surface of the metrics package.

Used by ``bga_router/eval.py`` to populate ``geometry.*`` /
``rule_check.*`` / ``si.*`` / ``standard.*`` sub-dicts in the result
JSON. Each sub-module is independently importable and testable.
"""

from .path_geometry import (
    path_length_mm,
    length_per_layer_mm,
    count_vias,
    count_layer_transitions,
    iter_segments_mm,
)

__all__ = [
    'path_length_mm',
    'length_per_layer_mm',
    'count_vias',
    'count_layer_transitions',
    'iter_segments_mm',
]
