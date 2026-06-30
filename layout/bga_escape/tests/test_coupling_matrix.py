# per-pair coupling matrix 단위 테스트 — parallel run length 산출
"""Phase F-1 — coupling matrix tests."""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Tuple

import pytest

from bga_router.metrics.coupling import (
    _parallel_overlap_length_mm,
    aggressors_per_net,
    coupling_matrix,
    summarize_coupling,
)


class _FakeGeom:
    def __init__(self, cell_mm: float = 0.1):
        self.cell_mm = cell_mm

    def cell_to_world(self, ix: int, iy: int) -> Tuple[float, float]:
        return ix * self.cell_mm, iy * self.cell_mm


class _FakeGrid:
    def __init__(self, cell_mm: float = 0.1):
        self.geom = _FakeGeom(cell_mm)


@dataclass
class _PR:
    path: list


# ---------------------------------------------------------------------------
# _parallel_overlap_length_mm
# ---------------------------------------------------------------------------


def test_overlap_parallel_close():
    """Two horizontal segments 0.1 mm apart, overlapping 5 mm."""
    s1 = ('A', 0.0, 0.0, 10.0, 0.0)
    s2 = ('B', 0.0, 0.1, 5.0, 0.1)
    L = _parallel_overlap_length_mm(s1, s2, gap_threshold_mm=0.30)
    assert L == pytest.approx(5.0, abs=1e-9)


def test_overlap_none_when_gap_exceeds_threshold():
    s1 = ('A', 0.0, 0.0, 10.0, 0.0)
    s2 = ('B', 0.0, 1.0, 5.0, 1.0)
    assert _parallel_overlap_length_mm(s1, s2,
                                          gap_threshold_mm=0.30) is None


def test_overlap_none_when_perpendicular():
    s1 = ('A', 0.0, 0.0, 10.0, 0.0)
    s2 = ('B', 5.0, -1.0, 5.0, 1.0)
    assert _parallel_overlap_length_mm(s1, s2) is None


def test_overlap_none_when_disjoint_projections():
    """Parallel but no overlap region."""
    s1 = ('A', 0.0, 0.0, 5.0, 0.0)
    s2 = ('B', 10.0, 0.1, 15.0, 0.1)
    assert _parallel_overlap_length_mm(s1, s2) is None


def test_overlap_partial_overlap():
    s1 = ('A', 0.0, 0.0, 10.0, 0.0)
    s2 = ('B', 7.0, 0.1, 12.0, 0.1)
    # overlap region 7..10 → 3.0 mm
    L = _parallel_overlap_length_mm(s1, s2)
    assert L == pytest.approx(3.0, abs=1e-9)


# ---------------------------------------------------------------------------
# coupling_matrix
# ---------------------------------------------------------------------------


def test_coupling_matrix_empty_when_no_pairs():
    g = _FakeGrid()
    routed = {'A': _PR([('M1', 0, 0), ('M1', 100, 0)])}
    assert coupling_matrix(routed, g) == {}


def test_coupling_matrix_picks_up_close_parallel_pair():
    g = _FakeGrid(cell_mm=0.05)
    routed = {
        'A': _PR([('M1', 0, 0), ('M1', 100, 0)]),    # (0,0)→(5,0)
        'B': _PR([('M1', 0, 2), ('M1', 100, 2)]),    # 0.1 mm away
    }
    matrix = coupling_matrix(routed, g, gap_threshold_mm=0.30)
    assert ('A', 'B') in matrix
    assert matrix[('A', 'B')] == pytest.approx(5.0, abs=1e-9)


def test_coupling_matrix_skips_distant_pairs():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'A': _PR([('M1', 0, 0), ('M1', 100, 0)]),
        'B': _PR([('M1', 0, 10), ('M1', 100, 10)]),  # 1.0 mm away
    }
    assert coupling_matrix(routed, g, gap_threshold_mm=0.30) == {}


def test_coupling_matrix_different_layers_ignored():
    g = _FakeGrid(cell_mm=0.05)
    routed = {
        'A': _PR([('M1', 0, 0), ('M1', 100, 0)]),
        'B': _PR([('M2', 0, 2), ('M2', 100, 2)]),     # different layer
    }
    assert coupling_matrix(routed, g) == {}


# ---------------------------------------------------------------------------
# aggressors_per_net + summarize_coupling
# ---------------------------------------------------------------------------


def test_aggressors_per_net_symmetric():
    matrix = {('A', 'B'): 5.0, ('A', 'C'): 2.0, ('B', 'C'): 0.1}
    out = aggressors_per_net(matrix, min_length_mm=0.5, top_k=5)
    assert 'A' in out and 'B' in out
    assert out['A'][0]['with'] == 'B'      # higher coupling first
    assert out['A'][0]['length_mm'] == pytest.approx(5.0)


def test_summarize_coupling_top_pairs_sorted():
    g = _FakeGrid(cell_mm=0.05)
    routed = {
        'A': _PR([('M1', 0, 0), ('M1', 100, 0)]),
        'B': _PR([('M1', 0, 2), ('M1', 100, 2)]),
        'C': _PR([('M1', 0, 6), ('M1', 50, 6)]),   # 0.3 mm from B → AT threshold
    }
    s = summarize_coupling(routed, g, top_pairs=10,
                              min_length_mm=0.5,
                              gap_threshold_mm=0.31)  # slightly higher
    assert s['pair_count'] >= 1
    if s['top_pairs']:
        # First pair should have the longest coupling
        first_L = s['top_pairs'][0]['length_mm']
        for entry in s['top_pairs'][1:]:
            assert entry['length_mm'] <= first_L


def test_summarize_coupling_thresholds_reported():
    g = _FakeGrid()
    s = summarize_coupling({}, g)
    assert s['thresholds']['gap_threshold_mm'] == 0.30
    assert s['thresholds']['angle_tol_deg'] == 5.0
