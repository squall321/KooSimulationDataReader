# Phase E-5 RoutingTask.extra_pins 스캐폴드 단위 테스트
"""Phase E-5 — multi-pin scaffold tests."""

from __future__ import annotations

import pytest

from src.net_extractor import NetEndpoint, RoutingTask
from src.routing_spec import NetRule


def _ep(layer='L1', ix=0, iy=0, net='N', ref='U1', pin='1'):
    return NetEndpoint(net_name=net, ref_des=ref, pin_name=pin,
                        layer=layer, ix=ix, iy=iy)


def _rule(w=0.075):
    return NetRule(layers=('L1',), escape_sides=('E',), width_mm=w)


def test_routing_task_default_extra_pins_none():
    t = RoutingTask(net_name='A', source=_ep(), sink=_ep(ix=10),
                     rule=_rule())
    assert t.extra_pins is None


def test_routing_task_accepts_tuple_extra_pins():
    extras = (_ep(ix=20), _ep(ix=30))
    t = RoutingTask(net_name='A', source=_ep(), sink=_ep(ix=10),
                     rule=_rule(), extra_pins=extras)
    assert t.extra_pins == extras
    assert len(t.extra_pins) == 2


def test_routing_task_existing_fields_unchanged():
    """Backward-compat: 2-pin construction still works."""
    t = RoutingTask(net_name='B', source=_ep(), sink=_ep(ix=5),
                     rule=_rule())
    assert t.net_name == 'B'
    assert t.is_pair is False
    assert t.pair_partner is None
    assert t.extra_pins is None
