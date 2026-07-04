"""Phase 4 Layer 1E — CostGrid (Layer 1A/1B/1C/1D 통합 진입점).

외부 모듈(Layer 2 이상)은 오직 이 파일에서 `CostGrid`만 import한다.
서브 모듈은 직접 import하지 않는다 — 의존 사이클과 인터페이스 분산을
방지하기 위함.

CostGrid는 PathFinder의 단일 진실 공급원:
  - 좌표계 (geom)
  - 셀 저장 (cells)
  - 차단 영역 (blockers)
  - stamp/unstamp 프로토콜
  - cost 함수 (PathFinder가 매 successor 평가할 때 호출)
"""

from __future__ import annotations

import math
from collections import defaultdict
from typing import Iterable, Optional

from src.cost_grid_blockers import (
    stamp_keepout_blockers,
    stamp_pad_blockers,
)
from src.cost_grid_cells import CellState, SparseCellStore
from src.cost_grid_coords import GridGeometry, compute_board_geometry
from src.cost_grid_stamping import (
    is_overused,
    stamp_path,
    unstamp_path,
)


# 외부에서 cost grid를 쓸 때 필요한 dataclass / 함수를 re-export.
__all__ = [
    'CostGrid',
    'GridGeometry',
    'CellState',
    'compute_board_geometry',
]


class CostGrid:
    """3D sparse cost-aware grid. PathFinder negotiation의 단일 진실원.

    좌표는 (layer:str, ix:int, iy:int)의 3D index space. mm 좌표는
    geom으로 변환.

    Cost 함수는 PathFinder 표준:
        base_cost
      + present_cost × p_factor(iter)
      + history_cost × h_factor
      + via_penalty if layer_changed
    """

    # PathFinder 기본 cost 파라미터. NegotiationConfig에서 override 가능.
    DEFAULT_BASE_COST: float = 1.0
    DEFAULT_VIA_PENALTY: float = 5.0
    DEFAULT_P_FACTOR_BASE: float = 0.5
    DEFAULT_P_FACTOR_STEP: float = 0.5
    DEFAULT_H_FACTOR: float = 1.0

    def __init__(
        self,
        geom: GridGeometry,
        signal_layers: list[str],
        *,
        base_cost: float = DEFAULT_BASE_COST,
        via_penalty: float = DEFAULT_VIA_PENALTY,
        p_factor_base: float = DEFAULT_P_FACTOR_BASE,
        p_factor_step: float = DEFAULT_P_FACTOR_STEP,
        h_factor: float = DEFAULT_H_FACTOR,
    ) -> None:
        if not signal_layers:
            raise ValueError(
                "CostGrid requires at least one signal_layer; got empty list"
            )
        # Duplicate detection — different layer name should mean different layer.
        if len(set(signal_layers)) != len(signal_layers):
            raise ValueError(
                f"CostGrid signal_layers contains duplicates: {signal_layers}"
            )
        self.geom: GridGeometry = geom
        self.signal_layers: list[str] = list(signal_layers)
        self.cells: SparseCellStore = SparseCellStore()
        self.base_cost = base_cost
        self.via_penalty = via_penalty
        self.p_factor_base = p_factor_base
        self.p_factor_step = p_factor_step
        self.h_factor = h_factor
        # Phase H-10 Stage 4 — Steiner-lite trunk reuse. When the routing
        # net ALREADY occupies a cell (multi-pin sub-task riding its own
        # earlier segment), charge base_cost * this factor instead of the
        # full base + own-present penalty. <1.0 attracts later sub-tasks
        # onto the existing trunk (shared copper); 1.0 disables the
        # discount. 2-pin nets never revisit their own cells (A* is
        # cycle-free) so this only activates for multi-pin nets.
        self.trunk_reuse_factor: float = 0.25
        # Phase 3.5 — plane split side-channel.
        # Maps layer name → list of bbox tuples (x_min, y_min, x_max, y_max) in mm.
        # Consumed by PlaneSplitAwareCost; ignored by all other cost fns.
        self.plane_splits: dict = defaultdict(list)

    def register_plane_split(
        self,
        layer: str,
        bbox: tuple[float, float, float, float],
    ) -> None:
        """Register a plane-split bbox (mm) on ``layer`` (a plane layer name).

        Per-V1 design: bbox-only — caller manually decomposes complex split
        polygons into axis-aligned bounding rectangles. PlaneSplitAwareCost
        reads adjacent-plane splits via stackup ctx.
        """
        x_min, y_min, x_max, y_max = bbox
        if x_min > x_max or y_min > y_max:
            raise ValueError(
                f"register_plane_split: invalid bbox {bbox} on layer {layer!r} "
                f"(min > max)"
            )
        self.plane_splits[layer].append((float(x_min), float(y_min),
                                          float(x_max), float(y_max)))

    # ---------- blocker import ----------

    def add_pad_blockers(
        self,
        components: Iterable,
        packages: dict,
        pad_clearance_mm: float = 0.0,
    ) -> int:
        """Layer 1D에 위임. 모든 signal_layer에 stamping."""
        return stamp_pad_blockers(
            self.cells, self.geom, components, packages,
            self.signal_layers, pad_clearance_mm,
        )

    def add_keepout_blockers(
        self,
        polygons: list[list[tuple[float, float]]],
    ) -> int:
        return stamp_keepout_blockers(
            self.cells, self.geom, polygons, self.signal_layers,
        )

    # ---------- stamping ----------

    def stamp(
        self,
        layer: str,
        cell_path: list[tuple[int, int]],
        net_name: str,
    ) -> None:
        if layer not in self.signal_layers:
            raise ValueError(
                f"CostGrid.stamp: layer {layer!r} not in signal_layers "
                f"{self.signal_layers}"
            )
        stamp_path(self.cells, layer, cell_path, net_name)

    def unstamp(
        self,
        layer: str,
        cell_path: list[tuple[int, int]],
        net_name: str,
    ) -> None:
        if layer not in self.signal_layers:
            raise ValueError(
                f"CostGrid.unstamp: layer {layer!r} not in signal_layers "
                f"{self.signal_layers}"
            )
        unstamp_path(self.cells, layer, cell_path, net_name)

    # ---------- query ----------

    def overused_keys(self) -> list[tuple[str, int, int]]:
        return self.cells.overused_keys()

    def bump_history(
        self,
        keys: list[tuple[str, int, int]],
        increment: float,
    ) -> None:
        self.cells.bump_history(keys, increment)

    def is_overused_cell(self, layer: str, ix: int, iy: int) -> bool:
        return is_overused(self.cells, layer, ix, iy)

    def is_blocked(self, layer: str, ix: int, iy: int) -> bool:
        c = self.cells.peek(layer, ix, iy)
        return c is not None and c.blocked

    def cells_occupied_by(self, net_name: str) -> list[tuple[str, int, int]]:
        """Return list of (layer, ix, iy) cells currently stamped by net_name.

        Phase 40a: used by the iter_max reroute loop to safely unstamp a net
        whose PathResult.path may have drifted (e.g. LengthMatch mutated
        path without restamping). Scanning the cell store is O(occupied)
        which is small for our sparse grids.
        """
        out: list[tuple[str, int, int]] = []
        # SparseCellStore exposes _cells as dict (or similar). Walk it.
        store = self.cells
        if hasattr(store, '_cells'):
            for (layer, ix, iy), cs in store._cells.items():
                if net_name in cs.occupant_nets:
                    out.append((layer, ix, iy))
        return out

    def unblock_cell(self, layer: str, ix: int, iy: int) -> bool:
        """주어진 cell의 blocked 플래그를 해제. 명시적 endpoint(라우팅이
        도달해야 하는 pad 정중앙 cell)에서만 사용한다.

        통합 시점에 발견된 본질적 책임 분리:
          - Layer 1D는 모든 pad 영역을 blocked로 stamp (보수적, 옳은 기본값).
          - 그러나 endpoint cell은 라우팅이 명시적으로 도달해야 하므로
            UNBPR 진입점이 위 stamp 후 endpoint cell만 골라 해제해 줘야 한다.

        반환: 실제로 unblock된 경우 True, 이미 free거나 cell이 없었으면
        False (디버깅용).
        """
        if layer not in self.signal_layers:
            raise ValueError(
                f"CostGrid.unblock_cell: layer {layer!r} not in signal_layers "
                f"{self.signal_layers}"
            )
        c = self.cells.peek(layer, ix, iy)
        if c is None or not c.blocked:
            return False
        c.blocked = False
        return True

    def total_cells(self) -> int:
        return self.cells.total_cells()

    # ---------- PathFinder cost ----------

    def p_factor(self, iteration: int) -> float:
        """현재 iteration의 present-cost multiplier.
        iter 0 (initial pass) → base 그대로, 이후 점진 증가."""
        if iteration <= 0:
            return self.p_factor_base
        return self.p_factor_base + self.p_factor_step * iteration

    def is_keep_out_for(
        self,
        request: Optional['RoutingRequest'],
        layer: str,
        ix: int,
        iy: int,
    ) -> bool:
        """Phase 45-3: shared keep_out_zones probe for ALL path solvers.

        Returns True iff this cell is masked by the request's
        ``keep_out_zones`` (per-net blocker). Returns False when the
        request is None, has no extra, has no zones, or the cell is
        outside every zone. Layer is accepted for future per-layer zones;
        currently ignored (zones are xy-only).

        Phase 45-20 — two semantic modes:
          * center-only (default): the cell's WORLD CENTER must be inside a
            zone to be masked. Fast and intuitive for prototyping.
          * copper-out (opt-in via ``request.extra['keep_out_inflate_by_width']
            = True``): zones are inflated by ``request.width_mm / 2`` so the
            entire trace POLYGON must stay out. This matches the industry
            convention for fab-rule keep-out boxes (no copper inside the
            zone, not just no centerline).

        Solvers that bypass :meth:`cell_cost` (CoupledAStar's joint loop,
        :meth:`CostGrid_v4.directional_cell_cost`) must call this on every
        successor candidate to honor per-net keep-out semantics.
        """
        if request is None:
            return False
        extra = request.extra
        if not extra:
            return False
        zones = extra.get('keep_out_zones')
        if not zones:
            return False
        if isinstance(zones[0], (int, float)):
            zones = (zones,)
        # Phase 45-20: width-aware inflation when opt-in flag is set.
        # Inflation half-width = max(width_mm, 0) / 2 — degenerate width=0
        # falls back to center-only naturally.
        # Phase 45-review fix M1: width_mm may be None (sparse JSON input)
        # — coerce defensively so we never crash with TypeError inside the
        # A* inner loop.
        if extra.get('keep_out_inflate_by_width'):
            w = getattr(request, 'width_mm', None)
            half_w = (max(0.0, float(w)) / 2.0) if w is not None else 0.0
        else:
            half_w = 0.0
        wx, wy = self.geom.cell_to_world(ix, iy)
        for (x0, y0, x1, y1) in zones:
            if (x0 - half_w <= wx <= x1 + half_w
                    and y0 - half_w <= wy <= y1 + half_w):
                return True
        return False

    def cell_cost(
        self,
        layer: str,
        ix: int,
        iy: int,
        iteration: int,
        prev_layer: Optional[str] = None,
        *,
        request: Optional['RoutingRequest'] = None,
    ) -> float:
        """이 cell로 진입하는 cost. A*의 g-value 증가량.

        Blocked cell은 무한대 (inf) → A* successor에서 자연 제외.

        Phase 34a: optional ``request`` kwarg carries the active
        :class:`RoutingRequest` so cost can vary per net (priority, class,
        via override). ``None`` reproduces v1 behaviour bit-for-bit.
        """
        cell = self.cells.peek(layer, ix, iy)
        if cell is not None and cell.blocked:
            return math.inf

        # Phase 44-2 / 45-3: per-net keep_out_zones, single source of
        # truth via is_keep_out_for() so non-cost_fn solvers stay in sync.
        if self.is_keep_out_for(request, layer, ix, iy):
            return math.inf

        present = cell.present_cost if cell is not None else 0
        history = cell.history_cost if cell is not None else 0.0

        # Phase H-10 Stage 4 — Steiner-lite: when this net already occupies
        # the cell (multi-pin sub-task meeting its own trunk), exclude the
        # own-net share of present cost and discount the base cost so the
        # router prefers riding the existing trunk over laying new copper.
        base = self.base_cost
        if (request is not None and cell is not None
                and request.net_name in cell.occupant_nets):
            present = max(0, present - 1)
            base = self.base_cost * self.trunk_reuse_factor

        cost = (
            base
            + present * self.p_factor(iteration)
            + history * self.h_factor
        )
        if prev_layer is not None and prev_layer != layer:
            cost += self.via_penalty
        return cost
