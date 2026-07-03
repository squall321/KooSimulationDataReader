# DFT 메트릭 단위 테스트 — test point 후보 / AOI 가시성 / probe pitch 충돌
"""Phase H-7 — DFT tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Tuple

import pytest

from bga_router.metrics.dft import (
    aoi_visibility,
    candidate_test_points,
    probe_pitch_conflicts,
    summarize_dft,
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


def test_aoi_visible_when_on_outer_layer():
    routed = {
        'A': _PR([('COMP', 0, 0), ('COMP', 10, 0)]),
        'B': _PR([('LAY2', 0, 5), ('LAY2', 10, 5)]),
    }
    vis = aoi_visibility(routed, outer_layers=('COMP',))
    assert vis['A'] is True
    assert vis['B'] is False


def test_candidate_test_points_clearance_filter():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        # A on outer layer, isolated
        'A': _PR([('COMP', 0, 0), ('COMP', 5, 0)]),
        # B on outer layer 5mm away — no interference
        'B': _PR([('COMP', 0, 50), ('COMP', 5, 50)]),
    }
    tps = candidate_test_points(routed, g, outer_layers=('COMP',),
                                   min_clear_mm=0.9)
    assert 'A' in tps and 'B' in tps


def test_candidate_test_points_rejected_when_crowded():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'A': _PR([('COMP', 0, 0), ('COMP', 5, 0)]),
        'B': _PR([('COMP', 0, 3), ('COMP', 5, 3)]),  # 0.3 mm away < 0.9
    }
    tps = candidate_test_points(routed, g, outer_layers=('COMP',),
                                   min_clear_mm=0.9)
    # Both nets' points are within 0.9 mm of each other → no candidates
    assert tps == {}


def test_probe_pitch_conflicts_detected():
    tps = {
        'A': [(0.0, 0.0)],
        'B': [(0.5, 0.0)],     # 0.5 mm < 1.27 pitch
        'C': [(10.0, 0.0)],    # far away
    }
    conflicts = probe_pitch_conflicts(tps, pitch_mm=1.27)
    assert len(conflicts) == 1
    assert set(conflicts[0]['nets']) == {'A', 'B'}


def test_summarize_dft_full():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'A': _PR([('COMP', 0, 0), ('COMP', 5, 0)]),
        'B': _PR([('COMP', 0, 50), ('COMP', 5, 50)]),
        'C': _PR([('LAY2', 0, 100), ('LAY2', 5, 100)]),  # buried
    }
    s = summarize_dft(routed, g, outer_layers=('COMP',))
    assert s['nets_with_test_point'] == 2
    assert s['nets_without_test_point'] == ['C']
    assert s['aoi_visible_count'] == 2
    assert s['aoi_buried_nets'] == ['C']
    assert s['test_point_coverage_pct'] == pytest.approx(66.67, abs=0.1)


def test_summarize_dft_empty():
    g = _FakeGrid()
    s = summarize_dft({}, g)
    assert s['test_point_coverage_pct'] == 0.0
    assert s['aoi_buried_nets'] == []
