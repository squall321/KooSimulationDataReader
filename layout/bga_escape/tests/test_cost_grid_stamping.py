"""Layer 1C — Stamping protocol tests.

Invariants under test:
  - stamp_path: present_cost ↑, occupant_nets adds.
  - double-stamp same net → ValueError.
  - blocked cell → ValueError.
  - unstamp_path: 정확한 역연산, 점유하지 않은 cell unstamp → ValueError.
  - 다른 net 두 개 같은 cell → present_cost=2, occupant 2개.
  - is_overused helper.
"""

from __future__ import annotations

import sys
from pathlib import Path

import pytest

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))

from src.cost_grid_cells import SparseCellStore                  # noqa: E402
from src.cost_grid_stamping import (                             # noqa: E402
    stamp_path, unstamp_path, is_overused,
)


# ---------------------------------------------------------------------------
# Basic stamp / unstamp round-trip
# ---------------------------------------------------------------------------

def test_stamp_increments_present_and_adds_occupant() -> None:
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [(0, 0), (0, 1), (0, 2)], 'NET_A')
    for ix, iy in [(0, 0), (0, 1), (0, 2)]:
        c = s.peek('LAY2', ix, iy)
        assert c is not None
        assert c.present_cost == 1
        assert c.occupant_nets == {'NET_A'}


def test_unstamp_is_inverse() -> None:
    s = SparseCellStore()
    path = [(0, 0), (1, 0), (2, 0)]
    stamp_path(s, 'LAY2', path, 'NET_A')
    unstamp_path(s, 'LAY2', path, 'NET_A')
    for ix, iy in path:
        c = s.peek('LAY2', ix, iy)
        # cells survive (created during stamp) but state is reset
        assert c.present_cost == 0
        assert c.occupant_nets == set()


def test_two_nets_same_cell_overuse() -> None:
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [(5, 5)], 'NET_A')
    stamp_path(s, 'LAY2', [(5, 5)], 'NET_B')
    c = s.peek('LAY2', 5, 5)
    assert c.present_cost == 2
    assert c.occupant_nets == {'NET_A', 'NET_B'}
    assert is_overused(s, 'LAY2', 5, 5)


# ---------------------------------------------------------------------------
# Error cases
# ---------------------------------------------------------------------------

def test_double_stamp_same_net_refcounts() -> None:
    """Phase H-10 Stage 3 — same-net re-stamp는 오류가 아니라 ref-count.

    Multi-pin net의 sub-path들이 junction pin cell을 공유하기 때문.
    present_cost는 distinct net 수만 세므로 1로 유지 (overuse 아님),
    unstamp는 대칭적으로 count를 내리고 0이 될 때만 점유 해제."""
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [(0, 0)], 'NET_A')
    stamp_path(s, 'LAY2', [(0, 0)], 'NET_A')       # ref-count → 2
    c = s.peek('LAY2', 0, 0)
    assert c.present_cost == 1                     # distinct nets only
    assert c.occupant_counts['NET_A'] == 2
    assert not is_overused(s, 'LAY2', 0, 0)
    # First unstamp: still occupied (count 2→1)
    unstamp_path(s, 'LAY2', [(0, 0)], 'NET_A')
    assert 'NET_A' in s.peek('LAY2', 0, 0).occupant_nets
    assert s.peek('LAY2', 0, 0).present_cost == 1
    # Second unstamp: fully released
    unstamp_path(s, 'LAY2', [(0, 0)], 'NET_A')
    assert 'NET_A' not in s.peek('LAY2', 0, 0).occupant_nets
    assert s.peek('LAY2', 0, 0).present_cost == 0
    # Third unstamp: now a genuine error
    with pytest.raises(ValueError, match='does not occupy'):
        unstamp_path(s, 'LAY2', [(0, 0)], 'NET_A')


def test_stamp_blocked_cell_raises() -> None:
    s = SparseCellStore()
    s.get('LAY2', 1, 1).blocked = True
    with pytest.raises(ValueError, match='blocked'):
        stamp_path(s, 'LAY2', [(0, 0), (1, 1), (2, 2)], 'NET_A')
    # Earlier cells in the path were already stamped (no atomicity contract)
    assert s.peek('LAY2', 0, 0).present_cost == 1


def test_unstamp_non_occupant_raises() -> None:
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [(0, 0)], 'NET_A')
    with pytest.raises(ValueError, match='does not occupy'):
        unstamp_path(s, 'LAY2', [(0, 0)], 'NET_B')


def test_unstamp_missing_cell_raises() -> None:
    s = SparseCellStore()
    with pytest.raises(ValueError, match='does not occupy'):
        unstamp_path(s, 'LAY2', [(0, 0)], 'NET_A')


# ---------------------------------------------------------------------------
# Layer / path independence
# ---------------------------------------------------------------------------

def test_stamp_on_different_layers_independent() -> None:
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [(0, 0)], 'NET_A')
    stamp_path(s, 'LAY3', [(0, 0)], 'NET_A')   # same net, different layer = OK
    assert s.peek('LAY2', 0, 0).present_cost == 1
    assert s.peek('LAY3', 0, 0).present_cost == 1
    assert s.peek('LAY2', 0, 0) is not s.peek('LAY3', 0, 0)


def test_stamp_empty_path_is_noop() -> None:
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [], 'NET_A')
    assert s.total_cells() == 0
    unstamp_path(s, 'LAY2', [], 'NET_A')   # also noop, no raise
    assert s.total_cells() == 0


def test_overused_keys_after_two_net_stamp() -> None:
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [(0, 0), (1, 0)], 'NET_A')
    stamp_path(s, 'LAY2', [(1, 0), (2, 0)], 'NET_B')   # 충돌 at (1, 0)
    assert s.overused_keys() == [('LAY2', 1, 0)]


# ---------------------------------------------------------------------------
# Restamp after unstamp (PathFinder의 핵심 사용 패턴)
# ---------------------------------------------------------------------------

def test_unstamp_then_restamp_same_net_succeeds() -> None:
    s = SparseCellStore()
    path = [(0, 0), (1, 1), (2, 2)]
    stamp_path(s, 'LAY2', path, 'NET_A')
    unstamp_path(s, 'LAY2', path, 'NET_A')
    # Re-stamping must work — PathFinder repeats this every iteration
    stamp_path(s, 'LAY2', path, 'NET_A')
    for ix, iy in path:
        c = s.peek('LAY2', ix, iy)
        assert c.present_cost == 1
        assert c.occupant_nets == {'NET_A'}


def test_partial_unstamp_then_restamp_different_path() -> None:
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [(0, 0), (0, 1), (0, 2)], 'NET_A')
    unstamp_path(s, 'LAY2', [(0, 0), (0, 1), (0, 2)], 'NET_A')
    stamp_path(s, 'LAY2', [(0, 0), (1, 0), (2, 0)], 'NET_A')
    # Old path cells are released
    assert s.peek('LAY2', 0, 1).occupant_nets == set()
    assert s.peek('LAY2', 0, 2).occupant_nets == set()
    # New path cells are stamped
    assert s.peek('LAY2', 1, 0).occupant_nets == {'NET_A'}
    assert s.peek('LAY2', 2, 0).occupant_nets == {'NET_A'}


# ---------------------------------------------------------------------------
# is_overused on absent cell
# ---------------------------------------------------------------------------

def test_is_overused_on_missing_cell_returns_false() -> None:
    s = SparseCellStore()
    assert is_overused(s, 'LAY2', 99, 99) is False
    # peek should still not create the cell
    assert s.peek('LAY2', 99, 99) is None


def test_is_overused_on_single_occupant_returns_false() -> None:
    s = SparseCellStore()
    stamp_path(s, 'LAY2', [(0, 0)], 'NET_A')
    assert is_overused(s, 'LAY2', 0, 0) is False
