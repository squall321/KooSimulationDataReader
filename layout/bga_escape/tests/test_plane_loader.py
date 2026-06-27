# plane_loader regex 파서 + point-in-polygon 단위 테스트
"""Phase C2 — plane loader tests."""

from __future__ import annotations

from pathlib import Path

import pytest

from src.ecad.plane_loader import (
    PlaneGeometry,
    PlaneLayer,
    PlanePolygon,
    find_nearest_return_via_distance,
    is_point_inside_plane,
    load_plane_geometry,
    parse_features_file,
)


_SAMPLE_FEATURES = """\
# synthetic plane features
S P 0
OB 0 0
OS 10 0
OS 10 10
OS 0 10
OE
IB 4 4
OS 6 4
OS 6 6
OS 4 6
IE
SE
"""


def test_parse_features_one_positive_with_hole():
    polys = parse_features_file(_SAMPLE_FEATURES, layer_name='GND1')
    assert len(polys) == 1
    p = polys[0]
    assert p.is_positive is True
    assert len(p.outer_xy_mm) == 4
    assert len(p.holes_xy_mm) == 1
    assert len(p.holes_xy_mm[0]) == 4


def test_parse_features_negative_polygon():
    text = """\
S N 0
OB 0 0
OS 5 0
OS 5 5
OS 0 5
OE
SE
"""
    polys = parse_features_file(text, layer_name='GND1')
    assert len(polys) == 1
    assert polys[0].is_positive is False


def test_parse_features_skips_empty_and_comments():
    text = "# comment\n\n" + _SAMPLE_FEATURES
    polys = parse_features_file(text, layer_name='GND1')
    assert len(polys) == 1


def test_point_in_polygon_inside_outer_outside_hole():
    polys = parse_features_file(_SAMPLE_FEATURES, layer_name='GND1')
    geom = PlaneGeometry(
        layers={'GND1': PlaneLayer(layer_name='GND1', polygons=polys)})

    # (1,1) — inside outer, outside hole
    assert is_point_inside_plane(geom, 'GND1', 1.0, 1.0) is True
    # (5,5) — inside hole
    assert is_point_inside_plane(geom, 'GND1', 5.0, 5.0) is False
    # (20,20) — outside outer
    assert is_point_inside_plane(geom, 'GND1', 20.0, 20.0) is False


def test_point_in_polygon_unknown_layer_false():
    geom = PlaneGeometry(layers={}, units_mm=True)
    assert is_point_inside_plane(geom, 'NOPE', 1.0, 1.0) is False


def test_load_plane_geometry_returns_empty_on_missing_dir(tmp_path):
    geom = load_plane_geometry(tmp_path)
    assert geom.layers == {}


def test_load_plane_geometry_finds_plane_layer(tmp_path):
    layer_dir = tmp_path / 'steps' / 'mentor' / 'layers' / 'GND_plane'
    layer_dir.mkdir(parents=True)
    (layer_dir / 'features').write_text(_SAMPLE_FEATURES)

    geom = load_plane_geometry(tmp_path)
    assert 'GND_plane' in geom.layers
    assert len(geom.layers['GND_plane'].polygons) == 1


def test_load_plane_geometry_ignores_non_plane_layer(tmp_path):
    layer_dir = tmp_path / 'steps' / 'mentor' / 'layers' / 'comp'
    layer_dir.mkdir(parents=True)
    (layer_dir / 'features').write_text(_SAMPLE_FEATURES)
    geom = load_plane_geometry(tmp_path)
    assert geom.layers == {}


def test_nearest_return_via_distance():
    signal_xy = [(0.0, 0.0), (10.0, 0.0)]
    vias_xy = [(5.0, 5.0), (12.0, 0.0)]
    # Min from any signal to any via: 2.0 (10,0)→(12,0)
    d = find_nearest_return_via_distance(None, signal_xy, vias_xy)
    assert d == pytest.approx(2.0, abs=1e-9)


def test_nearest_return_via_distance_no_vias():
    assert find_nearest_return_via_distance(None, [(0, 0)], []) is None
