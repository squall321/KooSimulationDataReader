# Phase G-5 multi-pin coverage 단위 테스트 — extra_pins 커버리지 검증
"""Phase G-5 — multi-pin verifier tests."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Optional, Tuple

import pytest

from bga_router.metrics.verifier import check_multi_pin_coverage


@dataclass
class _EP:
    layer: str
    ix: int
    iy: int


@dataclass
class _Task:
    net_name: str
    source: _EP
    sink: _EP
    rule: object = None
    extra_pins: Optional[Tuple[_EP, ...]] = None


@dataclass
class _PR:
    path: list


def test_none_when_no_extra_pins():
    t = _Task('A', _EP('L', 0, 0), _EP('L', 5, 5))
    assert check_multi_pin_coverage(t, {}) is None


def test_pass_when_all_pins_in_path():
    extras = (_EP('L', 0, 0), _EP('L', 5, 0), _EP('L', 10, 0))
    t = _Task('N', extras[0], extras[1], extra_pins=extras)
    routed = {'N': _PR([('L', 0, 0), ('L', 5, 0), ('L', 10, 0)])}
    assert check_multi_pin_coverage(t, routed) is True


def test_fail_when_a_pin_missing():
    extras = (_EP('L', 0, 0), _EP('L', 5, 0), _EP('L', 99, 99))
    t = _Task('N', extras[0], extras[1], extra_pins=extras)
    routed = {'N': _PR([('L', 0, 0), ('L', 5, 0)])}
    assert check_multi_pin_coverage(t, routed) is False


def test_fail_when_no_path():
    extras = (_EP('L', 0, 0), _EP('L', 5, 0))
    t = _Task('N', extras[0], extras[1], extra_pins=extras)
    assert check_multi_pin_coverage(t, {}) is False


def test_aggregates_across_subtask_paths():
    """Multi-pin nets are decomposed into 2-pin sub-tasks sharing net_name.
    The router creates multiple PathResult entries keyed by the same
    net_name — but the routed_paths dict is keyed by net_name so only one
    remains. Documented behaviour: verifier reads the surviving entry."""
    extras = (_EP('L', 0, 0), _EP('L', 5, 0), _EP('L', 10, 0))
    t = _Task('N', extras[0], extras[1], extra_pins=extras)
    routed = {'N': _PR([('L', 0, 0), ('L', 5, 0), ('L', 10, 0)])}
    assert check_multi_pin_coverage(t, routed) is True
