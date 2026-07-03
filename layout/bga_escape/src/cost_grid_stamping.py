"""Phase 4 Layer 1C — Stamping protocol.

PathFinder의 핵심 mutation:
  - stamp_path: net의 cell path를 grid에 등록 (present_cost ↑, occupant_nets에 추가).
  - unstamp_path: 정확한 역연산.

원자성과 일관성을 caller가 신경 쓰지 않도록 다음 invariant를 강제한다:
  1. blocked cell을 거치는 path는 stamp 오류 (caller A*가 잘못된 successor를 만든 것).
  2. occupant에 없는 net을 unstamp하면 오류 (적이 그린 path를 지우려는 시도).
  3. (Phase H-10 Stage 3) 같은 net의 재-stamp는 ref-count로 허용.
     Multi-pin net은 net_name을 공유하는 2-pin sub-task로 분해되며,
     sub-path들이 junction pin cell에서 만난다. present_cost는 여전히
     DISTINCT net 수만 세므로 overuse 판정은 왜곡되지 않는다.
     unstamp는 count를 내리고 0이 될 때만 occupant에서 제거 — 대칭 유지.

모든 invariant 위반은 ValueError로 즉시 실패한다 (silent 무시 금지). 라우터의
internal consistency를 잡는 안전망 역할이므로 fail-fast가 옳다.
"""

from __future__ import annotations

from src.cost_grid_cells import SparseCellStore


CellPath = list[tuple[int, int]]   # list of (ix, iy) in one layer


def stamp_path(
    store: SparseCellStore,
    layer: str,
    cell_path: CellPath,
    net_name: str,
) -> None:
    """주어진 path의 모든 cell을 net_name이 점유한 것으로 등록.

    cell 하나가 blocked이거나, net_name이 이미 그 cell의 occupant이면 ValueError.
    중간에 실패하면 그 전까지의 변경이 남는다 (atomicity 없음) — caller는
    오류 발생 시 unstamp_path로 직접 정리하거나 path를 검증 후 호출해야 함.
    """
    for ix, iy in cell_path:
        cell = store.get(layer, ix, iy)
        if cell.blocked:
            raise ValueError(
                f"stamp_path: cell ({layer}, {ix}, {iy}) is blocked; "
                f"net {net_name!r} cannot traverse it"
            )
        if net_name in cell.occupant_nets:
            # Phase H-10 Stage 3 — same-net re-stamp (multi-pin sub-task
            # junction). Bump the ref-count only; present_cost counts
            # distinct nets, so overuse detection stays correct.
            cell.occupant_counts[net_name] = (
                cell.occupant_counts.get(net_name, 1) + 1)
            continue
        cell.occupant_nets.add(net_name)
        cell.occupant_counts[net_name] = 1
        cell.present_cost += 1


def unstamp_path(
    store: SparseCellStore,
    layer: str,
    cell_path: CellPath,
    net_name: str,
) -> None:
    """stamp_path의 역연산. net_name이 차지하지 않은 cell이면 ValueError."""
    for ix, iy in cell_path:
        cell = store.peek(layer, ix, iy)
        if cell is None or net_name not in cell.occupant_nets:
            raise ValueError(
                f"unstamp_path: net {net_name!r} does not occupy cell "
                f"({layer}, {ix}, {iy}); cannot unstamp"
            )
        count = cell.occupant_counts.get(net_name, 1)
        if count > 1:
            # Phase H-10 Stage 3 — another sub-path of the same net still
            # occupies this junction cell; drop the ref-count only.
            cell.occupant_counts[net_name] = count - 1
            continue
        cell.occupant_counts.pop(net_name, None)
        cell.occupant_nets.remove(net_name)
        cell.present_cost -= 1


def is_overused(store: SparseCellStore, layer: str, ix: int, iy: int) -> bool:
    """편의 함수. present_cost > 1 검사. cell이 없으면 False."""
    cell = store.peek(layer, ix, iy)
    return cell is not None and cell.present_cost > 1
