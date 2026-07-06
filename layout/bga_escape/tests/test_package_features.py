# 패키지 물리/관계 피처 추출기 단위 테스트 — 합성 PackageInstance 기반
"""Tests for ``bga_router.metrics.package_features``.

All fixtures are built in-test (no ODB++ files needed). Three synthetic
packages exercise every relation helper plus the summary shape.
"""

from __future__ import annotations

import math
import sys
from pathlib import Path

import pytest

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))

from bga_router.metrics.package_features import (  # noqa: E402
    PackageInstance,
    bbox_of,
    bbox_overlap_area,
    board_context,
    build_packages,
    center_distance_mm,
    edge_distance_mm,
    neighbors,
    opposite_side_packages,
    package_intrinsic,
    pairwise,
    resolve_side,
    summarize_packages,
)


def _mk(ref, side, cx, cy, w, h):
    return PackageInstance(
        ref_des=ref, pkg_name=f'pkg_{ref}', side=side, side_source='test',
        cx_mm=cx, cy_mm=cy, width_mm=w, height_mm=h, area_mm2=w * h,
        pin_count=int(w * h), pitch_mm=0.5, rotation_deg=0.0,
    )


@pytest.fixture
def pkgs():
    # P1: TOP large @ (5,5) 4x4    → bbox (3,3,7,7), area 16
    # P2: BOT large @ (6,6) 4x4    → bbox (4,4,8,8), area 16  (overlaps P1)
    # P3: TOP small @ (20,20) 1x1  → bbox (19.5..20.5), area 1  (far)
    return [
        _mk('U1', 'TOP', 5, 5, 4, 4),
        _mk('U2', 'BOT', 6, 6, 4, 4),
        _mk('U3', 'TOP', 20, 20, 1, 1),
    ]


def test_bbox_of(pkgs):
    assert bbox_of(pkgs[0]) == (3.0, 3.0, 7.0, 7.0)
    assert bbox_of(pkgs[2]) == (19.5, 19.5, 20.5, 20.5)


def test_bbox_overlap_half_unit_squares():
    # Two unit squares sharing exactly half their area.
    a = (0.0, 0.0, 1.0, 1.0)
    b = (0.5, 0.0, 1.5, 1.0)
    assert bbox_overlap_area(a, b) == pytest.approx(0.5)
    # Disjoint → 0.
    assert bbox_overlap_area(a, (2.0, 2.0, 3.0, 3.0)) == 0.0


def test_center_and_edge_distance(pkgs):
    p1, p2, p3 = pkgs
    assert center_distance_mm(p1, p2) == pytest.approx(math.hypot(1, 1))
    assert center_distance_mm(p1, p3) == pytest.approx(math.hypot(15, 15))
    # P1 and P2 overlap → edge distance 0.
    assert edge_distance_mm(p1, p2) == 0.0
    # P1 bbox (3,3,7,7) vs P3 bbox (19.5,19.5,20.5,20.5): gap 12.5 each axis.
    assert edge_distance_mm(p1, p3) == pytest.approx(math.hypot(12.5, 12.5))


def test_opposite_side_packages(pkgs):
    res = opposite_side_packages(pkgs, 'U1')  # U1 is TOP → opposite = BOT
    assert [r['ref_des'] for r in res] == ['U2']
    r = res[0]
    assert r['center_dist'] == pytest.approx(math.hypot(1, 1))
    # Overlap of (3,3,7,7) and (4,4,8,8) = 3*3 = 9.
    assert r['overlap_area_mm2'] == pytest.approx(9.0)
    # overlap / min(area) = 9 / 16.
    assert r['overlap_ratio'] == pytest.approx(9.0 / 16.0)


def test_neighbors_same_side(pkgs):
    # Radius large enough to reach U3; U2 excluded because it is BOT.
    res = neighbors(pkgs, 'U1', radius_mm=25.0, same_side=True)
    assert [r['ref_des'] for r in res] == ['U3']
    assert res[0]['side'] == 'TOP'
    assert res[0]['center_dist'] == pytest.approx(math.hypot(15, 15))
    # Tight radius → nobody.
    assert neighbors(pkgs, 'U1', radius_mm=10.0, same_side=True) == []
    # Without same_side, the BOT neighbour U2 shows up too (sorted by dist).
    both = neighbors(pkgs, 'U1', radius_mm=25.0, same_side=False)
    assert [r['ref_des'] for r in both] == ['U2', 'U3']


def test_pairwise(pkgs):
    pr = pairwise(pkgs, 'U1', 'U2')
    assert pr['same_side'] is False
    assert pr['overlap_area'] == pytest.approx(9.0)
    assert pr['size_ratio'] == pytest.approx(1.0)  # equal areas
    assert pr['edge_dist'] == 0.0
    big_small = pairwise(pkgs, 'U1', 'U3')
    assert big_small['size_ratio'] == pytest.approx(16.0 / 1.0)
    assert pairwise(pkgs, 'U1', 'nope') == {}


def test_package_intrinsic(pkgs):
    d = package_intrinsic(pkgs[0])
    assert d['ref_des'] == 'U1'
    assert d['area_mm2'] == pytest.approx(16.0)
    assert d['aspect_ratio'] == pytest.approx(1.0)  # square
    # Non-square instance.
    rect = _mk('R1', 'TOP', 0, 0, 4, 2)
    assert package_intrinsic(rect)['aspect_ratio'] == pytest.approx(2.0)


def test_board_context(pkgs):
    board = (0.0, 0.0, 30.0, 30.0)
    ctx = board_context(pkgs, 'U1', board)
    assert ctx['area_fraction'] == pytest.approx(16.0 / 900.0)
    # U1 bbox (3,3,7,7) inside 30x30 board → nearest edge gap = 3.
    assert ctx['dist_to_edge_mm'] == pytest.approx(3.0)


def test_summarize_packages(pkgs):
    s = summarize_packages(pkgs)
    assert s['package_count'] == 3
    assert s['by_side'] == {'TOP': 2, 'BOT': 1}
    assert len(s['packages']) == 3
    # Only the P1/P2 stack overlaps; P3 is far from the lone BOT part.
    assert len(s['stacked_pairs']) == 1
    sp = s['stacked_pairs'][0]
    assert sp['top_ref'] == 'U1'
    assert sp['bot_ref'] == 'U2'
    assert sp['overlap_area'] == pytest.approx(9.0)
    assert sp['overlap_ratio'] == pytest.approx(9.0 / 16.0)


# --------------------------------------------------------------------------- #
# File-backed side resolution + build_packages (synthetic ODB++ layout)      #
# --------------------------------------------------------------------------- #

class _FakePkg:
    def __init__(self, name, pitch, xmin, ymin, xmax, ymax, npins):
        self.name = name
        self.pitch_mm = pitch
        self.bbox_min_x_mm = xmin
        self.bbox_min_y_mm = ymin
        self.bbox_max_x_mm = xmax
        self.bbox_max_y_mm = ymax
        self.pins = list(range(npins))


class _FakeCmp:
    def __init__(self, ref, pkg, x, y, rot=0.0, mirrored=False):
        self.ref_des = ref
        self.pkg_name = pkg
        self.x_mm = x
        self.y_mm = y
        self.rotation_deg = rot
        self.mirrored = mirrored


class _FakeEda:
    def __init__(self, packages, components):
        self.packages = packages
        self.components = components


def _write_components(path: Path, refs):
    lines = ['#comment']
    for i, ref in enumerate(refs):
        lines.append(f'CMP {i} 1.0 2.0 0 N {ref} comp_{ref}')
    path.write_text('\n'.join(lines) + '\n')


def test_resolve_side_and_build(tmp_path):
    # Build .../<step>/eda/data  and  .../<step>/layers/comp_+_{top,bot}/components
    step = tmp_path / 'step1'
    eda_dir = step / 'eda'
    eda_dir.mkdir(parents=True)
    eda_path = eda_dir / 'data'
    eda_path.write_text('UNITS=MM\n')  # content irrelevant for resolve_side

    top_dir = step / 'layers' / 'comp_+_top'
    bot_dir = step / 'layers' / 'comp_+_bot'
    top_dir.mkdir(parents=True)
    bot_dir.mkdir(parents=True)
    _write_components(top_dir / 'components', ['U1', 'U3'])
    _write_components(bot_dir / 'components', ['U2'])

    sm = resolve_side(eda_path)
    assert sm['U1'] == ('TOP', 'comp_layer')
    assert sm['U2'] == ('BOT', 'comp_layer')
    assert sm['U3'] == ('TOP', 'comp_layer')

    pkgdef = _FakePkg('sq', 0.5, -2, -2, 2, 2, 16)
    eda = _FakeEda(
        packages={'sq': pkgdef},
        components=[
            _FakeCmp('U1', 'sq', 5, 5),
            _FakeCmp('U2', 'sq', 6, 6, mirrored=False),  # file says BOT
            _FakeCmp('U9', 'sq', 9, 9, mirrored=True),   # not in files → fallback
        ],
    )
    insts = build_packages(eda, eda_path)
    by_ref = {p.ref_des: p for p in insts}
    assert by_ref['U1'].side == 'TOP' and by_ref['U1'].side_source == 'comp_layer'
    # File authority overrides mirrored=False here (both agree anyway → BOT).
    assert by_ref['U2'].side == 'BOT' and by_ref['U2'].side_source == 'comp_layer'
    # U9 absent from files → mirrored=True fallback → BOT, flagged.
    assert by_ref['U9'].side == 'BOT' and by_ref['U9'].side_source == 'mirrored'
    # Geometry from footprint bbox (4x4) centred on placement.
    assert by_ref['U1'].width_mm == pytest.approx(4.0)
    assert by_ref['U1'].area_mm2 == pytest.approx(16.0)
    assert by_ref['U1'].pin_count == 16
