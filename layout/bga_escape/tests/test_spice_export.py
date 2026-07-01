# spice_export 단위 테스트 — Z0 + DC R + coupling → SPICE .lib
"""Phase G-2 — SPICE export tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.spice_export import (
    _coupling_k,
    _lumped_lc_from_z0,
    build_spice_lib,
    write_spice_lib,
)


def test_lumped_lc_positive_values():
    lc = _lumped_lc_from_z0(50.0, 10.0, er=4.2)
    assert lc['L_nH'] > 0
    assert lc['C_pF'] > 0
    assert lc['T_ps'] > 0


def test_lumped_lc_zero_when_length_zero():
    lc = _lumped_lc_from_z0(50.0, 0.0)
    assert lc == {'L_nH': 0.0, 'C_pF': 0.0, 'T_ps': 0.0}


def test_coupling_k_bounded():
    """K ∈ [0, 0.15]."""
    assert 0 <= _coupling_k(0.0, 10.0) <= 0.15
    assert 0 <= _coupling_k(10.0, 10.0) <= 0.15
    # coupled_len > net_len clamped
    assert _coupling_k(100.0, 10.0) == pytest.approx(0.15)


def _synth_result():
    return {
        'dataset': 'DS', 'bga': 'B', 'recipe': 'R',
        'metrics': {
            'si': {
                'Z0_single_ended_ohm': {'A': 50.0, 'B': 55.0},
                'branch_dc_resistance_mohm': {'A': 10.0, 'B': 15.0},
                'via_stub_length_mm': {'A': 0.0, 'B': 0.5},
            },
            'coupling': {
                'top_pairs': [
                    {'pair': ['A', 'B'], 'length_mm': 2.0},
                ],
            },
        },
    }


def test_build_spice_lib_has_subckts():
    text = build_spice_lib(_synth_result())
    assert '.SUBCKT NET_A' in text
    assert '.SUBCKT NET_B' in text
    assert '.ENDS' in text


def test_build_spice_lib_emits_coupling_k_card():
    text = build_spice_lib(_synth_result())
    assert 'K_A_B' in text


def test_build_spice_lib_emits_stub_lumped():
    text = build_spice_lib(_synth_result())
    assert 'via stub' in text  # B has non-zero stub


def test_write_spice_lib_creates_file(tmp_path):
    inp = tmp_path / 'r.json'
    inp.write_text(json.dumps(_synth_result()))
    out = tmp_path / 'lib.spice'
    p = write_spice_lib(inp, out)
    assert p.exists()
    assert '.SUBCKT NET_A' in p.read_text()


def test_build_spice_lib_handles_empty_metrics():
    r = {'dataset': 'X', 'metrics': {'si': {}, 'coupling': {}}}
    text = build_spice_lib(r)
    assert 'net models emitted' in text
    assert '.SUBCKT' not in text
