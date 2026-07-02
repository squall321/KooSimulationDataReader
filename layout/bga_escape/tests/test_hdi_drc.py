# HDI micro-via stack DRC + drill aspect ratio 단위 테스트
"""Phase H-3 — HDI DRC tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import List

import pytest

from bga_router.metrics.hdi_drc import (
    check_drill_aspect_ratio,
    detect_stacked_microvias,
    summarize_hdi_drc,
)
from bga_router.metrics.stackup import (
    LayerEntry,
    StackupSpec,
    ViaSpan,
)


@dataclass
class _PR:
    via_metadata: List[dict]


def _mv(x, y, kind='microvia', filled=False):
    return {'xy_mm': (x, y), 'kind': kind, 'filled': filled,
              'start_layer': 'COMP', 'end_layer': 'LAY2'}


# ---------------------------------------------------------------------------
# detect_stacked_microvias
# ---------------------------------------------------------------------------


def test_no_stack_when_xy_differ():
    by_net = {'A': [_mv(0, 0), _mv(1, 0)]}
    assert detect_stacked_microvias(by_net) == {}


def test_stack_detected_when_same_xy_and_unfilled():
    by_net = {'A': [_mv(0, 0, filled=False), _mv(0, 0, filled=False)]}
    r = detect_stacked_microvias(by_net)
    assert r['A'] == 1


def test_stack_allowed_when_filled_and_capped():
    by_net = {'A': [_mv(0, 0, filled=True), _mv(0, 0, filled=False)]}
    r = detect_stacked_microvias(by_net)
    assert r == {}


def test_stack_only_for_microvia_kind():
    by_net = {'A': [_mv(0, 0, kind='through'),
                      _mv(0, 0, kind='through')]}
    assert detect_stacked_microvias(by_net) == {}


# ---------------------------------------------------------------------------
# check_drill_aspect_ratio
# ---------------------------------------------------------------------------


def _stackup_4layer(pth_drill=0.20, microvia_drill=0.10):
    return StackupSpec(
        layers=(
            LayerEntry('COMP',  0.035, 'copper'),
            LayerEntry('pp1',   0.05,  'prepreg', er=4.2),
            LayerEntry('LAY2',  0.035, 'copper'),
            LayerEntry('core',  0.20,  'core',    er=4.5),
            LayerEntry('LAY3',  0.035, 'copper'),
            LayerEntry('pp2',   0.05,  'prepreg', er=4.2),
            LayerEntry('LAY4',  0.035, 'copper'),
        ),
        vias=(
            ViaSpan('through', 'COMP', 'LAY4', 'through', drill_mm=pth_drill),
            ViaSpan('mv12',    'COMP', 'LAY2', 'microvia', drill_mm=microvia_drill),
        ),
    )


def test_drill_aspect_pth_passes_at_ratio_2():
    s = _stackup_4layer(pth_drill=0.20)   # thickness=0.44 / drill=0.2 = 2.2 <= 8
    via = s.via_by_name('through')
    assert check_drill_aspect_ratio(via, s) is True


def test_drill_aspect_pth_fails_when_drill_too_small():
    s = _stackup_4layer(pth_drill=0.02)   # ratio=22 > 8
    via = s.via_by_name('through')
    assert check_drill_aspect_ratio(via, s) is False


def test_drill_aspect_microvia_passes_when_shallow():
    s = _stackup_4layer(microvia_drill=0.15)   # thickness=0.12/0.15=0.8 <= 1.0
    via = s.via_by_name('mv12')
    assert check_drill_aspect_ratio(via, s) is True


def test_drill_aspect_microvia_fails_when_too_deep():
    s = _stackup_4layer(microvia_drill=0.05)   # 0.12/0.05=2.4 > 1.0
    via = s.via_by_name('mv12')
    assert check_drill_aspect_ratio(via, s) is False


def test_drill_aspect_none_when_no_drill():
    s = StackupSpec(
        layers=(LayerEntry('A', 0.035, 'copper'),
                 LayerEntry('B', 0.05, 'prepreg', er=4.2),
                 LayerEntry('C', 0.035, 'copper')),
        vias=(ViaSpan('t', 'A', 'C', 'through', drill_mm=None),))
    via = s.via_by_name('t')
    assert check_drill_aspect_ratio(via, s) is None


# ---------------------------------------------------------------------------
# summarize_hdi_drc
# ---------------------------------------------------------------------------


def test_summarize_reports_stacks_and_aspect():
    s = _stackup_4layer(pth_drill=0.05)   # unsafe PTH aspect
    routed = {'A': _PR([_mv(0, 0), _mv(0, 0)])}
    out = summarize_hdi_drc(routed, s, {})
    assert out['stacked_microvia_count'] == 1
    assert out['stacked_microvia_violations']['A'] == 1
    assert 'through' in out['drill_aspect_by_via_type']
    assert 'through' in out['unsafe_via_types']
