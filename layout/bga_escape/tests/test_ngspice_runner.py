# ngspice_runner 단위 테스트 — testbench 생성 + 출력 파싱 (+graceful skip)
"""Phase H-6 — ngspice crosstalk loop tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.ngspice_runner import (
    build_testbench,
    ngspice_available,
    parse_ngspice_output,
    run_crosstalk_batch,
    run_crosstalk_sim,
)


def test_build_testbench_includes_both_subckts(tmp_path):
    lib = tmp_path / 'x.lib'
    lib.write_text('* lib')
    tb = build_testbench(lib, 'net477', 'net479')
    assert 'NET_net477' in tb
    assert 'NET_net479' in tb
    assert '.include' in tb
    assert 'PULSE(0 1.0' in tb
    assert 'meas tran vpeak_near' in tb


def test_build_testbench_custom_swing(tmp_path):
    lib = tmp_path / 'x.lib'
    lib.write_text('* lib')
    tb = build_testbench(lib, 'A', 'B', swing_v=3.3)
    assert 'PULSE(0 3.3' in tb


def test_parse_ngspice_output_extracts_meas():
    stdout = """
some banner
vpeak_near  =  1.234500e-02
vpeak_far   =  5.678000e-03
done
"""
    out = parse_ngspice_output(stdout)
    assert out['vpeak_near'] == pytest.approx(0.012345)
    assert out['vpeak_far'] == pytest.approx(0.005678)


def test_parse_ngspice_output_empty():
    assert parse_ngspice_output('no meas here') == {}


def test_run_crosstalk_sim_writes_netlist_even_without_ngspice(tmp_path):
    lib = tmp_path / 'x.lib'
    lib.write_text('* lib')
    r = run_crosstalk_sim(lib, 'A', 'B', workdir=tmp_path / 'wd')
    assert Path(r['netlist_path']).exists()
    if not ngspice_available():
        assert r['ok'] is False
        assert 'ngspice not installed' in r['skip_reason']


def test_run_crosstalk_batch_top_k(tmp_path):
    lib = tmp_path / 'x.lib'
    lib.write_text('* lib')
    eval_data = {'metrics': {'coupling': {'top_pairs': [
        {'pair': ['A', 'B'], 'length_mm': 2.0},
        {'pair': ['C', 'D'], 'length_mm': 1.0},
        {'pair': ['E', 'F'], 'length_mm': 0.5},
    ]}}}
    out = run_crosstalk_batch(eval_data, lib,
                                workdir=tmp_path / 'wd', top_k=2)
    assert out['pairs_simulated'] == 2      # top_k respected
    nets = {(r['aggressor'], r['victim']) for r in out['results']}
    assert ('A', 'B') in nets
    assert ('E', 'F') not in nets


def test_run_crosstalk_batch_empty_coupling(tmp_path):
    lib = tmp_path / 'x.lib'
    lib.write_text('* lib')
    out = run_crosstalk_batch({'metrics': {}}, lib,
                                workdir=tmp_path / 'wd')
    assert out['pairs_simulated'] == 0
    assert out['worst_crosstalk_v'] is None
