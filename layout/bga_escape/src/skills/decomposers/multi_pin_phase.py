# Multi-pin net을 별도 마지막 phase로 분리하는 decomposer (Stage 5)
"""MultiPinPhaseDecomposer — Phase H-10 Stage 5.

Multi-pin nets (net_name을 공유하는 sub-task 2개 이상, 또는
extra_pins 보유) 는 branched 구조 + trunk-reuse discount 때문에
grid를 넓게 쓰는 경향이 있다. 이 decomposer는:

  phase 0 — 모든 2-pin net (짧고 직선적인 corridor 먼저 확보)
  phase 1 — multi-pin net (남은 공간에서 trunk를 자유롭게 분기)

design doc(2026-06-29-multi-pin-router-design.md) Stage 5의
"multi-pin nets to dedicated phase (often last)" 를 구현.
같은 net의 sub-task들은 반드시 같은 group에 있어야 trunk-reuse가
연속으로 동작한다 (분리되면 junction ref-count가 phase 경계를 넘음).
"""

from __future__ import annotations

from collections import Counter
from typing import Dict, List

from src.cost_grid import CostGrid
from src.skills.protocols import NetGroup
from src.skills.registry import register_skill


def _bbox_of_sources(tasks: list, grid: CostGrid):
    xs, ys = [], []
    for t in tasks:
        x, y = grid.geom.cell_to_world(t.source.ix, t.source.iy)
        xs.append(x)
        ys.append(y)
    if not xs:
        return None
    return (min(xs), min(ys), max(xs), max(ys))


@register_skill('decomposer')
class MultiPinPhaseDecomposer:
    """2-pin nets first (phase 0), multi-pin nets last (phase 1)."""

    name = 'MultiPinPhaseDecomposer'

    def decompose(self, tasks: list, grid: CostGrid, *,
                    config: dict) -> List[NetGroup]:
        if not tasks:
            return []

        # A net is multi-pin when it has 2+ sub-tasks (shared net_name)
        # or any of its tasks carries extra_pins.
        counts = Counter(t.net_name for t in tasks)
        multi_nets = {n for n, c in counts.items() if c >= 2}
        for t in tasks:
            if getattr(t, 'extra_pins', None):
                multi_nets.add(t.net_name)

        two_pin = [t for t in tasks if t.net_name not in multi_nets]
        multi = [t for t in tasks if t.net_name in multi_nets]

        groups: List[NetGroup] = []
        phase = 0
        if two_pin:
            two_sorted = sorted(two_pin, key=lambda t: t.net_name)
            groups.append(NetGroup(
                group_id='two_pin',
                phase=phase,
                tasks=two_sorted,
                region_bbox=_bbox_of_sources(two_sorted, grid),
                metadata={'n_tasks': len(two_sorted),
                           'kind': 'two_pin'},
            ))
            phase += 1
        if multi:
            # Keep same-net sub-tasks adjacent AND in original order —
            # daisy segments must stamp in sequence for trunk reuse.
            order_index = {id(t): i for i, t in enumerate(tasks)}
            multi_sorted = sorted(
                multi, key=lambda t: (t.net_name, order_index[id(t)]))
            groups.append(NetGroup(
                group_id='multi_pin',
                phase=phase,
                tasks=multi_sorted,
                region_bbox=_bbox_of_sources(multi_sorted, grid),
                metadata={'n_tasks': len(multi_sorted),
                           'kind': 'multi_pin',
                           'nets': sorted(multi_nets)},
            ))
        return groups
