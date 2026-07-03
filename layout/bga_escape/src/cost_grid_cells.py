"""Phase 4 Layer 1B — Sparse Cell Storage.

PathFinder의 핵심 누적 상태를 (layer, ix, iy) → CellState 로 보관한다.
점유된 cell만 저장하여 메모리 사용을 최소화 (보드 전체 격자 X).

이 모듈은 다른 cost_grid_* 모듈을 import 하지 않는다 (의존 사이클 차단).
"""

from __future__ import annotations

from dataclasses import dataclass, field


CellKey = tuple[str, int, int]


@dataclass
class CellState:
    """한 cell의 누적 상태. PathFinder의 핵심 데이터."""

    present_cost: int = 0
    history_cost: float = 0.0
    occupant_nets: set[str] = field(default_factory=set)
    blocked: bool = False
    # Phase H-10 Stage 3 — per-net stamp ref-count. Multi-pin nets are
    # decomposed into sub-tasks sharing net_name; sub-paths meet at
    # junction pins, so the same net can legitimately stamp a cell
    # twice. count>1 keeps stamp/unstamp symmetric without inflating
    # present_cost (which counts DISTINCT nets for overuse detection).
    occupant_counts: dict = field(default_factory=dict)


class SparseCellStore:
    """(layer:str, ix:int, iy:int) → CellState. 점유된 cell만 저장."""

    def __init__(self) -> None:
        self._cells: dict[CellKey, CellState] = {}

    def get(self, layer: str, ix: int, iy: int) -> CellState:
        """없으면 새 CellState 생성하고 저장 후 반환.
        이미 있으면 그 객체 반환 (동일 instance)."""
        key: CellKey = (layer, ix, iy)
        cell = self._cells.get(key)
        if cell is None:
            cell = CellState()
            self._cells[key] = cell
        return cell

    def peek(self, layer: str, ix: int, iy: int) -> CellState | None:
        """없으면 None 반환 (CellState 생성 안 함). 읽기 전용 검사용."""
        return self._cells.get((layer, ix, iy))

    def overused_keys(self) -> list[CellKey]:
        """present_cost > 1인 (layer, ix, iy) key 목록."""
        return [key for key, cell in self._cells.items() if cell.present_cost > 1]

    def total_cells(self) -> int:
        """저장된 cell 수 (메모리 측정용)."""
        return len(self._cells)

    def bump_history(self, keys: list[CellKey], increment: float) -> None:
        """주어진 cell key들의 history_cost에 increment를 더함.
        Cell이 없으면 생성 후 적용."""
        for key in keys:
            layer, ix, iy = key
            cell = self.get(layer, ix, iy)
            cell.history_cost += increment

    def cells_with_history(
        self, threshold: float = 0.0
    ) -> list[tuple[CellKey, CellState]]:
        """history_cost > threshold인 (key, CellState) 목록.
        Diagnostic Analyzer용."""
        return [
            (key, cell)
            for key, cell in self._cells.items()
            if cell.history_cost > threshold
        ]
