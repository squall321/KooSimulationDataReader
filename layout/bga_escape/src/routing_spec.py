"""Phase 4 — Routing Spec (사람의 ECAD 의도를 입력으로).

라우터는 layer/방향을 *결정*하지 않는다. 사람이 ECAD 의도로 정의하고,
라우터는 그 안에서 *실현*만 한다. 이 모듈은 그 의도를 YAML로 받아서
모든 net에 대해 적용할 NetRule을 결정한다.

설계 결정 (사용자 확정):
  1. 입력 형식 = YAML 전용.
  2. layers: [A, B] = 동등 후보. PathFinder의 history cost가 선택.
  3. spec이 net을 커버 못 하면 fail-fast (silent fallback 금지).

이 모듈은 ECAD 입력만 검증. 실제 cost 계산이나 successor 생성은
이 모듈을 *사용하는* CostGrid / A* 코드의 책임이다.
"""

from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable, List, Optional

import yaml


class RoutingSpecError(ValueError):
    """Spec 파일 자체가 잘못되었거나, net이 spec에 커버되지 않을 때."""


@dataclass(frozen=True)
class NetRule:
    """한 net에 적용되는 라우팅 제약. resolve() 출력."""
    layers: tuple              # 동등 후보, 1개면 hard 제약
    escape_sides: tuple        # N/S/E/W 부분집합, hard 제약
    width_mm: float
    pair_gap_mm: Optional[float] = None
    pair_skew_max_mm: Optional[float] = None
    rule_source: str = 'defaults'   # 디버깅용: 'defaults' / 'U201.pin_rules[0]' / 'net_rules[NET_NAME]'

    # --- new in Phase 3.4b ---
    clearance_mm: Optional[float] = None       # DRC net-to-net spacing
    preferred_layers: tuple = ()               # subset of `layers`; try first (soft bias)
    length_match_group: Optional[str] = None   # e.g. 'DDR0_BYTE0'
    pair_partner_net: Optional[str] = None     # explicit override of _P/_N suffix heuristic
    # Deferred (YAGNI — no consumer yet):
    priority: Optional[int] = None             # higher = route earlier; consumer TBD
    via_type: Optional[str] = None             # stackup via key; consumer TBD
    skip_routing: bool = False                 # power/ground hint; consumer TBD
    # Phase 3.5 — opt-in flag for PlaneSplitAwareCost.
    split_avoidance: bool = False              # bias router around plane splits

    # --- Phase 3.5 — Impedance-aware routing ---
    # Surfaced into RoutingRequest.extra by negotiators/_rule_extra.build_extra
    # and consumed by ImpedanceAwareCost. All optional; cost fn applies
    # defaults (h=0.1mm, er=4.3, t=0.035mm) when missing.
    impedance_target_ohm: Optional[float] = None   # target Z0 (e.g. 50.0, 100.0 diff)
    dielectric_height_mm: Optional[float] = None   # h to reference plane
    dielectric_er: Optional[float] = None          # relative permittivity
    copper_thickness_mm: Optional[float] = None    # trace thickness t
    # Phase 37a: CrosstalkAwareCost opt-in (3W rule)
    crosstalk_aware: bool = False
    crosstalk_radius_cells: Optional[int] = None   # default 3 (cells)

    # --- Phase 42C — T1 NetRule extensions ---
    # Consumers added incrementally; data layer + propagation done now.
    max_via_count: Optional[int] = None            # SI limit, e.g. 2 vias max
    shield_required: bool = False                  # request guard trace on both sides
    min_trace_length_mm: Optional[float] = None    # lower bound for length-match
    net_topology: Optional[str] = None             # 'daisy_chain' | 'star' | 'tee'
    keep_out_zones: tuple = ()                     # tuple of (x_min,y_min,x_max,y_max) in mm

    # --- Phase 47-1: Mid-route via in CoupledAStar ---
    # When True, the joint coupled solver uses _find_pair_path_via with
    # per-side via budget. Off by default — backwards-compat with Phase 45.
    coupled_allow_via: bool = False
    coupled_via_budget: Optional[int] = None       # default 2 if None & enabled

    # --- Phase 49-3: per-net bend class for ManufacturingDRC ---
    # 'low_speed' (default-ish): only 90°+ flagged as sharp.
    # 'hf_diff' / 'hf_single': 45° bends ALSO flagged (genuine SI hazard
    # for >=10Gbps differential pairs and similar high-speed nets).
    # None → fall back to recipe-level config (flag_45_bends).
    bend_class: Optional[str] = None               # 'low_speed' | 'hf_diff' | 'hf_single'

    # --- Phase F-3 — driver / load dependency graph ---
    # Identifies which pin is the source of timing (driver) vs which are
    # the loads. Used by the dependency-graph builder to derive
    # "DDR_CLK drives DDR_DQ[0..7]" relationships and by length-match
    # verifier to compute skew from the driver, not arbitrary endpoints.
    # Both fields are name-based hints (e.g. 'U200.A14') — verifier
    # falls back to source/sink when None.
    driver_pin: Optional[str] = None               # 'U200.A14' or similar
    load_pins: tuple = ()                          # ('U300.D0', 'U301.D0', ...)
    # Optional: declare a clock-domain anchor so dependency_graph can
    # cluster every net driven by the same clock.
    clock_domain: Optional[str] = None             # 'DDR4_CLK0' / 'PCIE3_REFCLK'


@dataclass
class _CompiledPinRule:
    """component 안의 한 pin_rule 항목 (regex 컴파일 끝난 상태)."""
    pattern: re.Pattern
    pattern_str: str
    layers: tuple
    escape_sides: Optional[tuple]    # None이면 component-level 상속
    width_mm: Optional[float]
    pair_gap_mm: Optional[float]
    pair_skew_max_mm: Optional[float]


@dataclass
class _CompiledComponent:
    """component 항목 (escape_sides 와 pin_rules)."""
    ref_des: str
    escape_sides: Optional[tuple]
    pin_rules: List[_CompiledPinRule]


@dataclass
class RoutingSpec:
    """Spec 전체. resolve(net_name, ref_des) → NetRule."""
    defaults: NetRule
    components: dict           # ref_des → _CompiledComponent
    net_rules: dict = field(default_factory=dict)   # net_name → NetRule (highest precedence)
    source_path: Optional[str] = None

    def resolve(self, net_name: str, ref_des: Optional[str]) -> NetRule:
        """net_name에 적용할 NetRule을 돌려준다.

        Priority:
          0. net_rules[net_name] — 가장 명시적, NEW in Phase 3.4b
          1. component.pin_rules[i] (첫 match 우선)
          2. component-level escape_sides + defaults.layers
          3. defaults 전체

        spec이 net을 커버하지 못하면 (defaults조차 layers 비어있고
        component에서도 못 찾으면) RoutingSpecError.
        """
        # 0. exact net_name override (highest precedence)
        if net_name in self.net_rules:
            return self.net_rules[net_name]
        # 1. component-specific pin_rules
        if ref_des and ref_des in self.components:
            comp = self.components[ref_des]
            for pr in comp.pin_rules:
                if pr.pattern.search(net_name):
                    return NetRule(
                        layers=pr.layers,
                        escape_sides=(pr.escape_sides
                                       if pr.escape_sides is not None
                                       else comp.escape_sides
                                       or self.defaults.escape_sides),
                        width_mm=(pr.width_mm
                                   if pr.width_mm is not None
                                   else self.defaults.width_mm),
                        pair_gap_mm=pr.pair_gap_mm,
                        pair_skew_max_mm=pr.pair_skew_max_mm,
                        rule_source=f'{ref_des}.pin_rules[{pr.pattern_str!r}]',
                    )
            # 2. component-level (no matching pin_rule)
            if comp.escape_sides is not None:
                if not self.defaults.layers:
                    raise RoutingSpecError(
                        f"net {net_name!r} on component {ref_des!r}: no "
                        f"matching pin_rule and defaults.layers is empty"
                    )
                return NetRule(
                    layers=self.defaults.layers,
                    escape_sides=comp.escape_sides,
                    width_mm=self.defaults.width_mm,
                    rule_source=f'{ref_des}.escape_sides + defaults',
                )

        # 3. pure defaults
        if not self.defaults.layers:
            raise RoutingSpecError(
                f"net {net_name!r}"
                + (f" on component {ref_des!r}" if ref_des else "")
                + ": no matching rule and defaults.layers is empty"
            )
        return self.defaults


def load_routing_spec(
    path: str | Path,
    *,
    stackup_signal_layers: Optional[Iterable[str]] = None,
    known_ref_des: Optional[Iterable[str]] = None,
) -> RoutingSpec:
    """YAML 파일을 읽어 RoutingSpec을 구성.

    stackup_signal_layers / known_ref_des가 주어지면 spec이 참조하는
    layer/ref_des가 그 안에 있는지 검증 (없으면 RoutingSpecError).
    호출자가 EDA를 이미 파싱한 후 넘기면 됨.
    """
    path = Path(path)
    text = path.read_text()
    try:
        data = yaml.safe_load(text)
    except yaml.YAMLError as e:
        raise RoutingSpecError(f"failed to parse {path}: {e}") from e
    if not isinstance(data, dict):
        raise RoutingSpecError(
            f"{path}: top-level must be a mapping, got {type(data).__name__}"
        )

    valid_layers = (set(stackup_signal_layers)
                     if stackup_signal_layers is not None else None)
    valid_refdes = (set(known_ref_des)
                     if known_ref_des is not None else None)

    # --- defaults ---
    d = data.get('defaults') or {}
    defaults = _build_default_rule(d, valid_layers, path)

    # --- components ---
    raw_components = data.get('components') or {}
    if not isinstance(raw_components, dict):
        raise RoutingSpecError(
            f"{path}: 'components' must be a mapping if present"
        )
    components: dict = {}
    for ref_des, body in raw_components.items():
        if valid_refdes is not None and ref_des not in valid_refdes:
            raise RoutingSpecError(
                f"{path}: components.{ref_des}: ref_des not found in EDA "
                f"(known: {sorted(valid_refdes)[:5]}... total "
                f"{len(valid_refdes)})"
            )
        components[ref_des] = _build_component(
            ref_des, body or {}, valid_layers, path
        )

    # --- net_rules (Phase 3.4b) ---
    raw_net_rules = data.get('net_rules') or {}
    if not isinstance(raw_net_rules, dict):
        raise RoutingSpecError(
            f"{path}: 'net_rules' must be a mapping if present"
        )
    net_rules: dict = {}
    for net_name, body in raw_net_rules.items():
        net_rules[net_name] = _build_net_rule_from_yaml(
            net_name, body or {}, defaults, valid_layers, path,
        )

    return RoutingSpec(
        defaults=defaults,
        components=components,
        net_rules=net_rules,
        source_path=str(path),
    )


# --- helpers ---

_VALID_SIDES = {'N', 'S', 'E', 'W'}


def _check_layers(layers, valid_layers, where: str, path: Path) -> tuple:
    if layers is None:
        return ()
    if not isinstance(layers, list) or not all(isinstance(l, str) for l in layers):
        raise RoutingSpecError(
            f"{path}: {where}.layers must be a list of strings"
        )
    if valid_layers is not None:
        unknown = [l for l in layers if l not in valid_layers]
        if unknown:
            raise RoutingSpecError(
                f"{path}: {where}.layers references unknown layer(s) "
                f"{unknown}; stackup has {sorted(valid_layers)}"
            )
    return tuple(layers)


def _check_sides(sides, where: str, path: Path) -> Optional[tuple]:
    if sides is None:
        return None
    if not isinstance(sides, list) or not all(isinstance(s, str) for s in sides):
        raise RoutingSpecError(
            f"{path}: {where}.escape_sides must be a list of strings"
        )
    upper = tuple(s.upper() for s in sides)
    bad = [s for s in upper if s not in _VALID_SIDES]
    if bad:
        raise RoutingSpecError(
            f"{path}: {where}.escape_sides has invalid entries {bad}; "
            f"allowed: {sorted(_VALID_SIDES)}"
        )
    return upper


def _build_default_rule(body: dict, valid_layers, path: Path) -> NetRule:
    layers = _check_layers(body.get('layers'), valid_layers, 'defaults', path)
    sides = _check_sides(body.get('escape_sides'), 'defaults', path)
    width = body.get('width_mm')
    if width is not None and not isinstance(width, (int, float)):
        raise RoutingSpecError(
            f"{path}: defaults.width_mm must be a number, got {type(width).__name__}"
        )
    return NetRule(
        layers=layers,
        escape_sides=sides or ('N', 'S', 'E', 'W'),
        width_mm=float(width) if width is not None else 0.075,
        rule_source='defaults',
    )


def _build_component(ref_des: str, body: dict, valid_layers, path: Path
                     ) -> _CompiledComponent:
    where = f'components.{ref_des}'
    sides = _check_sides(body.get('escape_sides'), where, path)

    raw_rules = body.get('pin_rules') or []
    if not isinstance(raw_rules, list):
        raise RoutingSpecError(
            f"{path}: {where}.pin_rules must be a list if present"
        )
    pin_rules: List[_CompiledPinRule] = []
    for i, r in enumerate(raw_rules):
        if not isinstance(r, dict):
            raise RoutingSpecError(
                f"{path}: {where}.pin_rules[{i}] must be a mapping"
            )
        pat_str = r.get('pattern')
        if not pat_str or not isinstance(pat_str, str):
            raise RoutingSpecError(
                f"{path}: {where}.pin_rules[{i}].pattern is required (str)"
            )
        try:
            pat = re.compile(pat_str)
        except re.error as e:
            raise RoutingSpecError(
                f"{path}: {where}.pin_rules[{i}].pattern is not a valid "
                f"regex: {e}"
            ) from e

        layers = _check_layers(
            r.get('layers'), valid_layers, f'{where}.pin_rules[{i}]', path
        )
        if not layers:
            raise RoutingSpecError(
                f"{path}: {where}.pin_rules[{i}].layers must be non-empty"
            )
        pr_sides = _check_sides(
            r.get('escape_sides'), f'{where}.pin_rules[{i}]', path
        )
        width = r.get('width_mm')
        if width is not None and not isinstance(width, (int, float)):
            raise RoutingSpecError(
                f"{path}: {where}.pin_rules[{i}].width_mm must be a number"
            )
        gap = r.get('pair_gap_mm')
        if gap is not None and not isinstance(gap, (int, float)):
            raise RoutingSpecError(
                f"{path}: {where}.pin_rules[{i}].pair_gap_mm must be a number"
            )
        skew = r.get('pair_skew_max_mm')
        if skew is not None and not isinstance(skew, (int, float)):
            raise RoutingSpecError(
                f"{path}: {where}.pin_rules[{i}].pair_skew_max_mm must be a number"
            )
        pin_rules.append(_CompiledPinRule(
            pattern=pat,
            pattern_str=pat_str,
            layers=layers,
            escape_sides=pr_sides,
            width_mm=float(width) if width is not None else None,
            pair_gap_mm=float(gap) if gap is not None else None,
            pair_skew_max_mm=float(skew) if skew is not None else None,
        ))

    return _CompiledComponent(
        ref_des=ref_des,
        escape_sides=sides,
        pin_rules=pin_rules,
    )


# ---------------------------------------------------------------------------
# Phase 3.4b — net_rules helpers
# ---------------------------------------------------------------------------


def _build_net_rule_from_yaml(
    net_name: str,
    body: dict,
    defaults: NetRule,
    valid_layers,
    path: Path,
) -> NetRule:
    """Build a NetRule from a YAML entry, merging per-field with defaults.

    Per-field merge means a user only writes the fields they want to override;
    everything else inherits from `defaults`. This keeps `resolve()` O(1) and
    `rule_source='net_rules[NET_NAME]'` immediately meaningful.
    """
    where = f'net_rules.{net_name}'

    # layers (hard candidate set) — fallback to defaults.layers
    raw_layers = body.get('layers')
    layers = (_check_layers(raw_layers, valid_layers, where, path)
              if raw_layers is not None else defaults.layers)

    # escape_sides — fallback to defaults.escape_sides
    raw_sides = body.get('escape_sides')
    sides = (_check_sides(raw_sides, where, path)
             if raw_sides is not None else defaults.escape_sides)

    # preferred_layers — soft bias; must be subset of effective layers
    raw_pref = body.get('preferred_layers')
    if raw_pref is None:
        preferred_layers = ()
    else:
        preferred_layers = _check_layers(raw_pref, valid_layers, where, path)
        bad = [l for l in preferred_layers if l not in layers]
        if bad:
            raise RoutingSpecError(
                f"{path}: {where}.preferred_layers {bad} not in effective "
                f"layers {list(layers)}"
            )

    def _num(key):
        v = body.get(key)
        if v is None:
            return None
        if not isinstance(v, (int, float)):
            raise RoutingSpecError(
                f"{path}: {where}.{key} must be a number, got "
                f"{type(v).__name__}"
            )
        return float(v)

    width = _num('width_mm')
    if width is None:
        width = defaults.width_mm

    pair_gap = _num('pair_gap_mm')
    if pair_gap is None:
        pair_gap = defaults.pair_gap_mm
    pair_skew = _num('pair_skew_max_mm')
    if pair_skew is None:
        pair_skew = defaults.pair_skew_max_mm
    clearance = _num('clearance_mm')
    if clearance is None:
        clearance = defaults.clearance_mm

    length_match = body.get('length_match_group')
    if length_match is not None and not isinstance(length_match, str):
        raise RoutingSpecError(
            f"{path}: {where}.length_match_group must be a string"
        )

    pair_partner = body.get('pair_partner_net')
    if pair_partner is not None and not isinstance(pair_partner, str):
        raise RoutingSpecError(
            f"{path}: {where}.pair_partner_net must be a string"
        )

    priority = body.get('priority')
    if priority is not None and not isinstance(priority, int):
        raise RoutingSpecError(
            f"{path}: {where}.priority must be an int"
        )

    via_type = body.get('via_type')
    if via_type is not None and not isinstance(via_type, str):
        raise RoutingSpecError(
            f"{path}: {where}.via_type must be a string"
        )

    skip = bool(body.get('skip_routing', False))
    split_avoid = bool(body.get('split_avoidance', False))

    # --- Phase 42C T1 extensions ---
    max_via = body.get('max_via_count')
    if max_via is not None and not isinstance(max_via, int):
        raise RoutingSpecError(
            f"{path}: {where}.max_via_count must be an int"
        )
    shield = bool(body.get('shield_required', False))
    min_len = _num('min_trace_length_mm')

    topo = body.get('net_topology')
    if topo is not None:
        if not isinstance(topo, str) or topo not in (
                'daisy_chain', 'star', 'tee'):
            raise RoutingSpecError(
                f"{path}: {where}.net_topology must be one of "
                f"'daisy_chain'/'star'/'tee'"
            )

    raw_koz = body.get('keep_out_zones') or ()
    koz: list = []
    if raw_koz:
        if not isinstance(raw_koz, list):
            raise RoutingSpecError(
                f"{path}: {where}.keep_out_zones must be a list of "
                f"[x_min, y_min, x_max, y_max]"
            )
        for i, z in enumerate(raw_koz):
            if (not isinstance(z, (list, tuple)) or len(z) != 4
                    or not all(isinstance(v, (int, float)) for v in z)):
                raise RoutingSpecError(
                    f"{path}: {where}.keep_out_zones[{i}] must be "
                    f"[x_min, y_min, x_max, y_max] of numbers"
                )
            # Phase 44 review fix #11: degenerate bbox (min > max) would
            # be silently no-op'd by cell_cost. Catch the typo at parse
            # time. Equal min/max is allowed (zero-area zone blocks one
            # cell-center).
            x0, y0, x1, y1 = (float(v) for v in z)
            if x0 > x1 or y0 > y1:
                raise RoutingSpecError(
                    f"{path}: {where}.keep_out_zones[{i}]: degenerate "
                    f"bbox — got [{x0}, {y0}, {x1}, {y1}], expected "
                    f"x_min<=x_max and y_min<=y_max"
                )
            koz.append((x0, y0, x1, y1))

    return NetRule(
        layers=layers,
        escape_sides=sides,
        width_mm=width,
        pair_gap_mm=pair_gap,
        pair_skew_max_mm=pair_skew,
        rule_source=f'net_rules[{net_name!r}]',
        clearance_mm=clearance,
        preferred_layers=preferred_layers,
        length_match_group=length_match,
        pair_partner_net=pair_partner,
        priority=priority,
        via_type=via_type,
        skip_routing=skip,
        split_avoidance=split_avoid,
        max_via_count=max_via,
        shield_required=shield,
        min_trace_length_mm=min_len,
        net_topology=topo,
        keep_out_zones=tuple(koz),
    )


def net_rules_from_yaml(path: str | Path) -> dict:
    """Parse a standalone YAML/JSON net-rules file → ``dict[str, NetRule]``.

    Convenience helper for callers that want to feed net_rules without
    a full RoutingSpec YAML (e.g. mid-pipeline injection after EDA load).
    The file's top level may be either:
      ``net_rules:`` (mapping under that key, same shape as load_routing_spec)
      or a bare ``{net_name: {...}}`` mapping (treated as the net_rules block).

    No `defaults` merge happens — each entry must be fully self-contained
    (layers/escape_sides/width_mm required). Use load_routing_spec() if you
    want defaults inheritance.
    """
    path = Path(path)
    text = path.read_text()
    try:
        data = yaml.safe_load(text)
    except yaml.YAMLError as e:
        raise RoutingSpecError(f"failed to parse {path}: {e}") from e
    if not isinstance(data, dict):
        raise RoutingSpecError(
            f"{path}: top-level must be a mapping"
        )
    raw = data.get('net_rules') if 'net_rules' in data else data
    if not isinstance(raw, dict):
        raise RoutingSpecError(
            f"{path}: 'net_rules' must be a mapping"
        )

    # Build a synthetic minimal "defaults" so _build_net_rule_from_yaml can
    # fall back. Caller-supplied entries should override all required fields.
    synth_default = NetRule(
        layers=(), escape_sides=('N', 'S', 'E', 'W'), width_mm=0.075,
    )
    out: dict = {}
    for name, body in raw.items():
        body = body or {}
        if not body.get('layers'):
            raise RoutingSpecError(
                f"{path}: net_rules[{name!r}].layers is required when using "
                f"net_rules_from_yaml (no defaults to inherit from)"
            )
        out[name] = _build_net_rule_from_yaml(
            name, body, synth_default, None, path,
        )
    return out


def synth_net_rules_from_name(net_name: str) -> Optional[NetRule]:
    """Heuristic NetRule generator from net name — for tests/quick demos.

    Returns ``None`` when the name doesn't match any well-known pattern
    (caller should fall back to defaults / spec.resolve).

    Patterns:
      DDR\\w*_DQ\\d+(_P|_N|+|-)?  → priority=90, length_match_group='DDR_DQ',
                                     pair_partner set when name has _P/_N suffix.
      USB_TX, USB_RX, USB_DP, USB_DM → priority=80, length_match='USB_DATA'.
      CLK*                          → priority=100.
      GND*, VDD*, VCC*, VSS*        → priority=10, skip_routing=True.
    """
    n = net_name.upper()

    # Power/ground — lowest priority, skip
    if (n.startswith('GND') or n.startswith('VDD')
            or n.startswith('VCC') or n.startswith('VSS')):
        return NetRule(
            layers=(),
            escape_sides=('N', 'S', 'E', 'W'),
            width_mm=0.075,
            rule_source=f'synth[power:{net_name!r}]',
            priority=10,
            skip_routing=True,
        )

    # Clock — highest priority
    if n.startswith('CLK') or '_CLK' in n:
        return NetRule(
            layers=(),
            escape_sides=('N', 'S', 'E', 'W'),
            width_mm=0.075,
            rule_source=f'synth[clk:{net_name!r}]',
            priority=100,
        )

    # DDR data bus (DDR*_DQ<n> optionally with _P/_N for diff variant)
    m = re.match(r'^(DDR\w*)_DQ(\d+)(_P|_N|\+|\-)?$', n)
    if m:
        suffix = m.group(3)
        partner = None
        if suffix in ('_P', '_N'):
            partner = (net_name[:-2] + ('_N' if suffix == '_P' else '_P'))
        elif suffix in ('+', '-'):
            partner = (net_name[:-1] + ('-' if suffix == '+' else '+'))
        return NetRule(
            layers=(),
            escape_sides=('N', 'S', 'E', 'W'),
            width_mm=0.075,
            rule_source=f'synth[ddr_dq:{net_name!r}]',
            priority=90,
            length_match_group='DDR_DQ',
            pair_partner_net=partner,
        )

    # USB high-speed pair
    if n in ('USB_TX', 'USB_RX', 'USB_DP', 'USB_DM'):
        partner_map = {'USB_DP': 'USB_DM', 'USB_DM': 'USB_DP',
                       'USB_TX': 'USB_RX', 'USB_RX': 'USB_TX'}
        return NetRule(
            layers=(),
            escape_sides=('N', 'S', 'E', 'W'),
            width_mm=0.075,
            rule_source=f'synth[usb:{net_name!r}]',
            priority=80,
            length_match_group='USB_DATA',
            pair_partner_net=partner_map.get(n),
        )

    return None
