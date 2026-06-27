# matrix file parser 단위 테스트 — TYPE 분류 + plane layer 자동 탐지
"""Phase D-3 — matrix_loader tests."""

from __future__ import annotations

from pathlib import Path

import pytest

from src.ecad.matrix_loader import (
    MatrixInfo,
    MatrixLayer,
    load_matrix,
    parse_matrix_text,
)


SAMPLE_MATRIX = """\
STEP {
    COL=1
    NAME=MENTOR
}

LAYER {
    ROW=1
    CONTEXT=BOARD
    TYPE=COMPONENT
    NAME=COMP_+_TOP
    POLARITY=POSITIVE
}

LAYER {
    ROW=2
    CONTEXT=BOARD
    TYPE=SIGNAL
    NAME=COMP
    POLARITY=POSITIVE
    OLD_NAME=L1
}

LAYER {
    ROW=3
    CONTEXT=BOARD
    TYPE=POWER_GROUND
    NAME=GND1
    POLARITY=POSITIVE
}

LAYER {
    ROW=4
    CONTEXT=BOARD
    TYPE=MIXED
    NAME=PWR_VDD
    POLARITY=POSITIVE
}

LAYER {
    ROW=5
    CONTEXT=BOARD
    TYPE=DRILL
    NAME=drill
    POLARITY=POSITIVE
}
"""


def test_parse_matrix_basic():
    mx = parse_matrix_text(SAMPLE_MATRIX)
    assert len(mx.layers) == 5
    comp = mx.by_name('COMP')
    assert comp is not None
    assert comp.type == 'SIGNAL'
    assert comp.row == 2
    assert comp.old_name == 'L1'


def test_plane_layer_names_returns_power_ground_and_mixed():
    mx = parse_matrix_text(SAMPLE_MATRIX)
    planes = mx.plane_layer_names()
    assert 'GND1' in planes
    assert 'PWR_VDD' in planes
    assert 'COMP' not in planes


def test_signal_layer_names_excludes_planes():
    mx = parse_matrix_text(SAMPLE_MATRIX)
    sigs = mx.signal_layer_names()
    assert 'COMP' in sigs
    assert 'GND1' not in sigs


def test_by_name_case_insensitive():
    mx = parse_matrix_text(SAMPLE_MATRIX)
    assert mx.by_name('comp') is not None
    assert mx.by_name('GND1') is not None
    assert mx.by_name('does_not_exist') is None


def test_load_matrix_missing_returns_none(tmp_path):
    assert load_matrix(tmp_path) is None


def test_load_matrix_finds_file(tmp_path):
    matrix_dir = tmp_path / 'matrix'
    matrix_dir.mkdir()
    (matrix_dir / 'matrix').write_text(SAMPLE_MATRIX)
    mx = load_matrix(tmp_path)
    assert mx is not None
    assert len(mx.layers) == 5
    assert mx.source_path.endswith('matrix/matrix')


def test_load_matrix_real_p3():
    """If the real P3 fixture is present, exercise the parser on it."""
    repo_root = Path(__file__).resolve().parents[3]
    p3 = repo_root / 'examples' / 'ECAD' / 'P3_EUR_REV03'
    if not (p3 / 'matrix' / 'matrix').exists():
        pytest.skip('P3 fixture missing')
    mx = load_matrix(p3)
    assert mx is not None
    sigs = mx.signal_layer_names()
    assert 'COMP' in sigs
    assert len(sigs) >= 4
    # P3 has no POWER_GROUND-typed layers (planes realized as SIGNAL pours)
    planes = mx.plane_layer_names()
    assert isinstance(planes, tuple)
