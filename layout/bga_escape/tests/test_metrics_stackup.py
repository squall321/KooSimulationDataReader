# stackup 로더 단위 테스트 — YAML 파싱, default fallback, signal 분류
"""Phase C1 — stackup loader tests."""

from __future__ import annotations

from pathlib import Path

import pytest

from bga_router.metrics.stackup import (
    LayerEntry,
    StackupError,
    StackupSpec,
    default_stackup,
    load_for_dataset,
    load_stackup_yaml,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
P3_YAML = REPO_ROOT / 'examples' / 'ECAD' / 'p3_stackup.yaml'


def test_load_existing_p3_yaml_8_signals():
    if not P3_YAML.exists():
        pytest.skip(f'reference YAML missing at {P3_YAML}')
    s = load_stackup_yaml(P3_YAML)
    sigs = s.signal_layer_names()
    assert len(sigs) == 8
    assert 'COMP' in sigs
    assert s.microstrip_layer('COMP') is True
    assert s.stripline_layer('LAY3') is True
    assert s.is_default is False


def test_dielectric_below_finds_prepreg():
    if not P3_YAML.exists():
        pytest.skip(f'reference YAML missing at {P3_YAML}')
    s = load_stackup_yaml(P3_YAML)
    d = s.dielectric_below('COMP')
    assert d is not None
    assert d.kind in ('prepreg', 'core')
    assert d.er == pytest.approx(4.2, abs=0.1)


def test_dielectric_below_bottom_returns_none():
    s = default_stackup()
    sigs = s.signal_layer_names()
    bottom = sigs[-1]
    assert s.dielectric_below(bottom) is None


def test_copper_thickness_known():
    s = default_stackup()
    assert s.copper_thickness_mm('COMP') == pytest.approx(0.035)
    assert s.copper_thickness_mm('does_not_exist') is None


def test_default_stackup_4_signals_and_flag():
    s = default_stackup()
    assert s.is_default is True
    assert s.source_path is None
    sigs = s.signal_layer_names()
    assert len(sigs) == 4
    assert sigs[0] == 'COMP'
    assert sigs[-1] == 'LAY4'


def test_microstrip_and_stripline_classification_default():
    s = default_stackup()
    # COMP and LAY4 are outermost → microstrip
    assert s.microstrip_layer('COMP') is True
    assert s.microstrip_layer('LAY4') is True
    # LAY2 and LAY3 are interior → stripline
    assert s.stripline_layer('LAY2') is True
    assert s.stripline_layer('LAY3') is True
    assert s.microstrip_layer('LAY2') is False


def test_malformed_yaml_raises(tmp_path):
    bad = tmp_path / 'bad.yaml'
    bad.write_text('not: a list\n')
    with pytest.raises(StackupError):
        load_stackup_yaml(bad)


def test_missing_file_raises(tmp_path):
    with pytest.raises(StackupError):
        load_stackup_yaml(tmp_path / 'nope.yaml')


def test_copper_must_have_dielectric_between(tmp_path):
    bad = tmp_path / 'adjacent_copper.yaml'
    bad.write_text(
        'layers:\n'
        '  - {name: A, thickness: 0.035, type: copper}\n'
        '  - {name: B, thickness: 0.035, type: copper}\n'
    )
    with pytest.raises(StackupError, match='must be separated'):
        load_stackup_yaml(bad)


def test_load_for_dataset_default_when_no_key():
    s = load_for_dataset({'path': '/nope'})
    assert s.is_default is True


def test_load_for_dataset_default_when_none():
    s = load_for_dataset(None)
    assert s.is_default is True


def test_load_for_dataset_with_absolute_path():
    if not P3_YAML.exists():
        pytest.skip(f'reference YAML missing at {P3_YAML}')
    s = load_for_dataset({'stackup_yaml': str(P3_YAML)})
    assert s.is_default is False
    assert len(s.signal_layer_names()) == 8


def test_load_for_dataset_falls_back_on_malformed(tmp_path):
    bad = tmp_path / 'bad.yaml'
    bad.write_text('not valid yaml: [')
    s = load_for_dataset({'stackup_yaml': str(bad)})
    assert s.is_default is True
