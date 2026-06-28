# via_stub_length_mm + ViaSpan 단위 테스트 — back-drill 후보 계산
"""Phase D-4 — via stub length tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.si import via_stub_length_mm
from bga_router.metrics.stackup import (
    LayerEntry,
    StackupSpec,
    ViaSpan,
    default_stackup,
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
class _Rule:
    width_mm: float = 0.075
    via_type: Optional[str] = None


def test_default_stackup_has_through_via():
    s = default_stackup()
    assert len(s.vias) == 1
    assert s.vias[0].name == 'through'
    assert s.vias[0].kind == 'through'


def test_stack_thickness_between_full_stack():
    s = default_stackup()
    full = s.stack_thickness_between_mm('COMP', 'LAY4')
    # COMP(0.035) + pp1(0.10) + LAY2(0.035) + core1(0.20) + LAY3(0.035)
    # + pp2(0.10) + LAY4(0.035) = 0.54
    assert full == pytest.approx(0.54, abs=1e-9)


def test_stub_length_zero_when_using_full_stack():
    s = default_stackup()
    via = s.vias[0]
    stub = s.stub_length_for_via(via, 'COMP', 'LAY4')
    assert stub == pytest.approx(0.0, abs=1e-9)


def test_stub_length_nonzero_when_using_partial():
    s = default_stackup()
    via = s.vias[0]
    # Use only COMP→LAY2 of a full-stack through via.
    # full = 0.54, used = COMP(0.035) + pp1(0.10) + LAY2(0.035) = 0.17
    # stub = 0.54 - 0.17 = 0.37
    stub = s.stub_length_for_via(via, 'COMP', 'LAY2')
    assert stub == pytest.approx(0.37, abs=1e-9)


def test_via_stub_length_mm_aggregates_per_via():
    s = default_stackup()
    g = _FakeGrid()
    # Both hops use the full COMP→LAY4 through via (only via declared).
    # Full stack = 0.54.
    # Hop 1: COMP→LAY2  used = layers[0..2] = 0.035+0.10+0.035 = 0.17
    #                   stub = 0.54 - 0.17 = 0.37
    # Hop 2: LAY2→LAY3  used = layers[2..4] = 0.035+0.20+0.035 = 0.27
    #                   stub = 0.54 - 0.27 = 0.27
    path = [('COMP', 0, 0), ('LAY2', 0, 0), ('LAY3', 0, 0)]
    total = via_stub_length_mm(path, g, _Rule(), s)
    assert total == pytest.approx(0.37 + 0.27, abs=1e-9)


def test_via_stub_length_mm_zero_for_no_via_transitions():
    s = default_stackup()
    g = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 10, 0)]
    assert via_stub_length_mm(path, g, _Rule(), s) == pytest.approx(0.0)


def test_via_stub_zero_when_stackup_has_no_vias():
    s_no_vias = StackupSpec(
        layers=(LayerEntry('A', 0.035, 'copper'),
                 LayerEntry('pp', 0.10, 'prepreg', er=4.2),
                 LayerEntry('B', 0.035, 'copper')),
        vias=())
    g = _FakeGrid()
    assert via_stub_length_mm(
        [('A', 0, 0), ('B', 0, 0)], g, _Rule(), s_no_vias) == 0.0


def test_via_type_resolution_picks_named_via(tmp_path):
    """Multiple vias declared; rule.via_type selects which one."""
    s = StackupSpec(
        layers=(LayerEntry('COMP', 0.035, 'copper'),
                 LayerEntry('pp1', 0.10, 'prepreg', er=4.2),
                 LayerEntry('LAY2', 0.035, 'copper'),
                 LayerEntry('core', 0.20, 'core', er=4.5),
                 LayerEntry('LAY3', 0.035, 'copper')),
        vias=(
            ViaSpan('through', 'COMP', 'LAY3', 'through'),
            ViaSpan('mv_top',  'COMP', 'LAY2', 'microvia'),
        ))
    g = _FakeGrid()
    # Same physical hop COMP→LAY2, but via_type='mv_top' means
    # the via fits perfectly (used==full), stub=0.
    via_mv = s.via_by_name('mv_top')
    assert s.stub_length_for_via(via_mv, 'COMP', 'LAY2') == pytest.approx(0.0)
    # Whereas 'through' would leave a stub.
    via_th = s.via_by_name('through')
    stub_through = s.stub_length_for_via(via_th, 'COMP', 'LAY2')
    assert stub_through > 0


def test_via_yaml_block_parsed(tmp_path):
    """Loader picks up vias: block in YAML."""
    from bga_router.metrics.stackup import load_stackup_yaml
    p = tmp_path / 'stack.yaml'
    p.write_text(
        'layers:\n'
        '  - {name: COMP, thickness: 0.035, type: copper}\n'
        '  - {name: pp, thickness: 0.10, type: prepreg, dielectric_constant: 4.2}\n'
        '  - {name: LAY2, thickness: 0.035, type: copper}\n'
        'vias:\n'
        '  - {name: through, start_layer: COMP, end_layer: LAY2, kind: through, drill_mm: 0.20}\n'
    )
    s = load_stackup_yaml(p)
    assert len(s.vias) == 1
    assert s.vias[0].name == 'through'
    assert s.vias[0].drill_mm == pytest.approx(0.20)
