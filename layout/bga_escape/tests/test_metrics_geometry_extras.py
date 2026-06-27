# bga_router.metrics.geometry 형상 품질 지표 합성 경로 단위 테스트
"""Phase A — geometry extras unit tests.

Hand-computed expected values, synthetic paths only. No ODB++ fixtures.

Note on strict inequalities (CLAUDE.md rule 1 — make assumptions explicit):
  - sharp_bends:  angle < 135°  (90° turn → 90° interior angle → counted)
  - acute_bends:  angle <  90°  (90° turn → 90° interior angle → NOT counted)
  - total_bends:  angle < 179°  (anything noticeably non-straight)
A 90° L-bend yields interior angle exactly 90° → sharp=1, acute=0.
"""

from __future__ import annotations

import math
from dataclasses import dataclass

import pytest

from bga_router.metrics.geometry import (
    count_bends,
    bend_density_per_mm,
    bbox_straightness,
    detour_count,
    routing_efficiency,
    escape_side_fidelity,
    summarize_geometry,
)


class _FakeGeom:
    def __init__(self, cell_mm: float = 0.1):
        self.cell_mm = cell_mm

    def cell_to_world(self, ix, iy):
        return ix * self.cell_mm, iy * self.cell_mm


class _FakeGrid:
    def __init__(self, cell_mm: float = 0.1):
        self.geom = _FakeGeom(cell_mm)


# --------------------------------------------------------------------------- #
# 1) count_bends                                                              #
# --------------------------------------------------------------------------- #

def test_count_bends_straight_line_has_no_bends():
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 5, 0), ('COMP', 10, 0)]
    out = count_bends(path, grid)
    assert out == {'sharp_bends': 0, 'acute_bends': 0, 'total_bends': 0}


def test_count_bends_l_bend_90deg_counts_sharp_not_acute():
    """90° L-bend: interior angle = 90°. sharp (<135) yes; acute (<90) no."""
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0), ('COMP', 10, 10)]
    out = count_bends(path, grid)
    assert out['sharp_bends'] == 1
    assert out['acute_bends'] == 0
    assert out['total_bends'] == 1


def test_count_bends_u_shape_two_90deg_bends():
    """U: (0,0)->(10,0)->(10,10)->(0,10). Two 90° bends.
    sharp=2 (both <135), acute=0 (strict <90 not satisfied at 90°)."""
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0),
            ('COMP', 10, 10), ('COMP', 0, 10)]
    out = count_bends(path, grid)
    assert out['sharp_bends'] == 2
    assert out['acute_bends'] == 0
    assert out['total_bends'] == 2


def test_count_bends_skips_via_triples():
    """Triples spanning a via must not be counted."""
    grid = _FakeGrid()
    path = [
        ('COMP', 0, 0),
        ('COMP', 10, 0),      # b
        ('LAY2', 10, 0),      # via — triple (a,b,c) has lb != lc → skip
        ('LAY2', 10, 10),     # straight on LAY2 (need 3 pts to bend)
    ]
    out = count_bends(path, grid)
    assert out == {'sharp_bends': 0, 'acute_bends': 0, 'total_bends': 0}


def test_count_bends_acute_turn_is_counted_in_both():
    """A 135° turn (45° interior angle) → both sharp and acute."""
    grid = _FakeGrid(cell_mm=1.0)
    # (0,0) -> (1,0) -> (0,1)
    # v1 = (1,0), v2 = (-1,1). cos = -1/sqrt(2) → turn = 135° → interior = 45°.
    path = [('COMP', 0, 0), ('COMP', 1, 0), ('COMP', 0, 1)]
    out = count_bends(path, grid)
    assert out == {'sharp_bends': 1, 'acute_bends': 1, 'total_bends': 1}


# --------------------------------------------------------------------------- #
# 2) bend_density_per_mm                                                      #
# --------------------------------------------------------------------------- #

def test_bend_density_zero_length_returns_zero():
    grid = _FakeGrid()
    assert bend_density_per_mm([], grid) == 0.0
    assert bend_density_per_mm([('COMP', 5, 5)], grid) == 0.0


def test_bend_density_per_mm_l_bend():
    """L-bend 1 mm + 1 mm = 2 mm path with 1 total bend → 0.5 /mm."""
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0), ('COMP', 10, 10)]
    assert bend_density_per_mm(path, grid) == pytest.approx(0.5, abs=1e-9)


# --------------------------------------------------------------------------- #
# 3) bbox_straightness                                                        #
# --------------------------------------------------------------------------- #

def test_bbox_straightness_degenerate_straight_returns_1():
    grid = _FakeGrid()
    # Pure-x straight line: endpoints define zero-area bbox → 1.0.
    path = [('COMP', 0, 0), ('COMP', 5, 0), ('COMP', 10, 0)]
    assert bbox_straightness(path, grid) == 1.0


def test_bbox_straightness_l_bend_ratio():
    """L-bend (0,0)->(10,0)->(10,10): path bbox = 1×1 mm² = 1.0.
    Endpoints (0,0)-(10,10) → bbox 1×1 = 1.0 → ratio = 1.0."""
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0), ('COMP', 10, 10)]
    assert bbox_straightness(path, grid) == pytest.approx(1.0, abs=1e-9)


def test_bbox_straightness_detour_grows_ratio():
    """Detour where endpoints lie on x-axis: line bbox area = 0
    (axis-aligned), special-case path with non-zero bbox → huge value."""
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 5, 5), ('COMP', 10, 0)]
    val = bbox_straightness(path, grid)
    assert val > 1.0


# --------------------------------------------------------------------------- #
# 4) detour_count                                                             #
# --------------------------------------------------------------------------- #

def test_detour_count_doubling_back_counts_one():
    """Out and back: (0,0)->(10,0)->(5,0). Dot product < 0 → 1 detour."""
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0), ('COMP', 5, 0)]
    assert detour_count(path, grid) == 1


def test_detour_count_straight_no_detour():
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 5, 0), ('COMP', 10, 0)]
    assert detour_count(path, grid) == 0


def test_detour_count_90deg_l_is_not_a_detour():
    """90° bend: dot product == 0, not < 0 → not a detour."""
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0), ('COMP', 10, 10)]
    assert detour_count(path, grid) == 0


# --------------------------------------------------------------------------- #
# 5) routing_efficiency                                                       #
# --------------------------------------------------------------------------- #

def test_routing_efficiency_perfect_straight_is_one():
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0)]  # 1.0 mm
    eff = routing_efficiency(path, grid, (0.0, 0.0), (1.0, 0.0))
    assert eff == pytest.approx(1.0, abs=1e-9)


def test_routing_efficiency_l_bend_vs_euclidean():
    """L from (0,0) to (1,1): path 2.0 mm, euclidean √2 → 2/√2 = √2."""
    import math
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0), ('COMP', 10, 10)]
    eff = routing_efficiency(path, grid, (0.0, 0.0), (1.0, 1.0))
    assert eff == pytest.approx(math.sqrt(2.0), abs=1e-9)


def test_routing_efficiency_detour_increases_value():
    import math
    grid = _FakeGrid()
    # Path: 1.0 + 0.5 + 2.0 = 3.5 mm. endpoints (0,0)→(0.5,2.0).
    # euclidean = hypot(0.5, 2.0) = √4.25. ratio = 3.5/√4.25.
    path = [('COMP', 0, 0), ('COMP', 10, 0), ('COMP', 5, 0), ('COMP', 5, 20)]
    eff = routing_efficiency(path, grid, (0.0, 0.0), (0.5, 2.0))
    assert eff == pytest.approx(3.5 / math.hypot(0.5, 2.0), abs=1e-9)


def test_routing_efficiency_diagonal_is_one():
    """8-connected router: a 45° diagonal trace should report efficiency = 1.0."""
    import math
    grid = _FakeGrid()
    path = [('COMP', i, i) for i in range(11)]  # 10 unit-diagonal steps
    eff = routing_efficiency(path, grid, (0.0, 0.0), (1.0, 1.0))
    assert eff == pytest.approx(1.0, abs=1e-9)


# --------------------------------------------------------------------------- #
# 6) escape_side_fidelity                                                     #
# --------------------------------------------------------------------------- #

def test_escape_side_east_match():
    """Source at (0,0); path heads east → 'E' matches, 'N' does not."""
    grid = _FakeGrid()
    # (0,0) → (10,0) on grid = 1.0 mm in +x → exceeds 0.5mm threshold.
    path = [('COMP', 0, 0), ('COMP', 10, 0)]
    assert escape_side_fidelity(path, grid, (0.0, 0.0), ('E',)) is True
    assert escape_side_fidelity(path, grid, (0.0, 0.0), ('N',)) is False


def test_escape_side_empty_means_no_constraint():
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0)]
    assert escape_side_fidelity(path, grid, (0.0, 0.0), ()) is True


def test_escape_side_short_stub_returns_true():
    """No point exceeds 0.5mm from source → returns True (vacuous)."""
    grid = _FakeGrid()
    # 0.4 mm move only.
    path = [('COMP', 0, 0), ('COMP', 4, 0)]
    assert escape_side_fidelity(path, grid, (0.0, 0.0), ('N',)) is True


def test_escape_side_multi_side_allowed():
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0)]
    assert escape_side_fidelity(path, grid, (0.0, 0.0), ('N', 'E')) is True


# --------------------------------------------------------------------------- #
# Aggregate                                                                   #
# --------------------------------------------------------------------------- #

@dataclass
class _FakePathResult:
    path: list


@dataclass
class _FakeRule:
    escape_sides: tuple


@dataclass
class _FakeTask:
    net_name: str
    source: tuple
    sink: tuple
    rule: _FakeRule


def test_summarize_geometry_basic():
    grid = _FakeGrid()
    # Net A: L-bend straight from (0,0) to (1,1) mm — efficiency 1.0,
    # sharp_bends=1.
    path_a = [('COMP', 0, 0), ('COMP', 10, 0), ('COMP', 10, 10)]
    # Net B: pure straight east 1mm — no bends, efficiency 1.0.
    path_b = [('COMP', 0, 0), ('COMP', 10, 0)]
    routed = {
        'A': _FakePathResult(path_a),
        'B': _FakePathResult(path_b),
    }
    tasks = [
        _FakeTask('A', ('COMP', 0, 0), ('COMP', 10, 10),
                  _FakeRule(escape_sides=('E',))),
        _FakeTask('B', ('COMP', 0, 0), ('COMP', 10, 0),
                  _FakeRule(escape_sides=('E',))),
    ]
    summary = summarize_geometry(routed, grid, tasks)
    assert summary['sharp_bends'] == 1
    assert summary['acute_bends'] == 0
    assert summary['total_bends'] == 1
    assert summary['detour_count'] == 0
    # bend_density = 1 bend / (2 mm A + 1 mm B) = 1/3
    assert summary['bend_density_per_mm'] == pytest.approx(1.0 / 3.0, abs=1e-9)
    # A: L-bend path 2.0 / euclidean √2 = √2. B: straight = 1.0.
    import math
    expected_eff_a = 2.0 / math.sqrt(2.0)
    assert summary['routing_efficiency_mean'] == pytest.approx(
        (expected_eff_a + 1.0) / 2.0, abs=1e-9)
    assert summary['routing_efficiency_max'] == pytest.approx(
        expected_eff_a, abs=1e-9)
    assert summary['escape_side_fidelity_pct'] == 1.0


def test_summarize_geometry_no_tasks_skip_escape_check():
    """A net in routed_paths without a matching task → no escape check.
    With zero checked nets, fidelity_pct defaults to 1.0."""
    grid = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0)]
    routed = {'A': _FakePathResult(path)}
    summary = summarize_geometry(routed, grid, tasks=[])
    assert summary['escape_side_fidelity_pct'] == 1.0
    assert summary['routing_efficiency_mean'] == 0.0  # no task → no measurement


def test_summarize_geometry_escape_side_violation():
    grid = _FakeGrid()
    # Path heads east but escape_sides demands 'N' → violation.
    path = [('COMP', 0, 0), ('COMP', 10, 0)]
    routed = {'A': _FakePathResult(path)}
    tasks = [_FakeTask('A', ('COMP', 0, 0), ('COMP', 10, 0),
                       _FakeRule(escape_sides=('N',)))]
    summary = summarize_geometry(routed, grid, tasks)
    assert summary['escape_side_fidelity_pct'] == 0.0
