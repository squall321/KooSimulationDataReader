# 라우팅 경로의 길이 / via / 레이어 전이 등 순수 기하 계산을 모은 공용 헬퍼
"""Shared path-geometry helpers used by the metrics package and by the
existing post-processors (manufacturing_drc / length_match /
diff_pair / propagation_delay) which previously duplicated the same
``_path_length_mm`` implementation three times.

A path is a ``list[(layer:str, ix:int, iy:int)]`` of grid cells; mm
conversion uses ``grid.geom.cell_to_world(ix, iy)``. Vias are inferred
from layer transitions between consecutive cells.
"""

from __future__ import annotations

import math
from typing import Iterable, Iterator, Tuple


PathPoint = Tuple[str, int, int]              # (layer, ix, iy)
SegmentMm = Tuple[str, float, float, float, float]  # (layer, x0, y0, x1, y1)


def iter_segments_mm(path: Iterable[PathPoint], grid) -> Iterator[SegmentMm]:
    """Yield consecutive same-layer segments in mm world coordinates.

    A layer transition (via) is skipped — it contributes no mm length to
    the trace; it is counted separately via ``count_vias`` /
    ``count_layer_transitions``.
    """
    prev_layer = None
    prev_xy = None
    for layer, ix, iy in path:
        x, y = grid.geom.cell_to_world(ix, iy)
        if prev_layer is not None and prev_layer == layer:
            px, py = prev_xy
            yield (layer, px, py, x, y)
        prev_layer = layer
        prev_xy = (x, y)


def path_length_mm(path: Iterable[PathPoint], grid) -> float:
    """Total in-plane length, summed across all layers (vias excluded)."""
    total = 0.0
    for _layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
        total += math.hypot(x1 - x0, y1 - y0)
    return total


def length_per_layer_mm(path: Iterable[PathPoint], grid) -> dict[str, float]:
    """Per-layer trace length in mm. Layer absent in path → not present."""
    out: dict[str, float] = {}
    for layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
        out[layer] = out.get(layer, 0.0) + math.hypot(x1 - x0, y1 - y0)
    return out


def count_vias(path: Iterable[PathPoint]) -> int:
    """Count layer transitions in the path. One transition = one via."""
    n = 0
    prev_layer = None
    for layer, _ix, _iy in path:
        if prev_layer is not None and prev_layer != layer:
            n += 1
        prev_layer = layer
    return n


# Alias — semantically the same metric in current router (one via per
# transition, no stacked-via accounting yet).
count_layer_transitions = count_vias
