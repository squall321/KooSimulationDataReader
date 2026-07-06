# Phase J-1 — S21 insertion loss 곡선 추출 + SI 리포트 렌더 테스트
"""Phase J-1 — frequency-domain SI report tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.sim_aggregator import (
    collect_sim_results,
    insertion_loss_curve,
)
from bga_router.integrations.si_report import (
    render_si_report,
    write_si_report,
)


FIXTURE = Path(__file__).parent / 'fixtures' / 'sol_d_net114'


# ---------------------------------------------------------------------------
# insertion_loss_curve
# ---------------------------------------------------------------------------


def test_curve_empty_input():
    c = insertion_loss_curve([], [])
    assert c['freqs_ghz'] == []
    assert c['worst_il_db'] is None


def test_curve_basic_flat():
    freqs = [1e9, 2e9, 3e9]
    s21 = [complex(1.0, 0), complex(0.9, 0), complex(0.5, 0)]
    c = insertion_loss_curve(freqs, s21)
    assert len(c['il_db']) == 3
    assert c['il_db'][0] == pytest.approx(0.0, abs=1e-6)   # |1.0| → 0 dB
    # |0.5| = -6.02 dB, monotone decreasing
    assert c['il_db'][2] < c['il_db'][1] < c['il_db'][0] + 1e-9
    assert c['worst_il_db'] == pytest.approx(c['il_db'][2])


def test_curve_interpolation_at_1ghz():
    freqs = [0.5e9, 1.5e9]
    s21 = [complex(1.0, 0), complex(0.5, 0)]   # 0 dB → -6 dB
    c = insertion_loss_curve(freqs, s21)
    # 1 GHz is midpoint → ~-3 dB
    assert c['il_at_1ghz_db'] == pytest.approx(-3.01, abs=0.2)


def test_curve_f3db_detection():
    freqs = [1e9, 2e9, 3e9]
    s21 = [complex(1.0, 0), complex(0.8, 0), complex(0.3, 0)]
    c = insertion_loss_curve(freqs, s21)
    # -3 dB crossing: |0.8|=-1.9, |0.3|=-10.5 → first <=-3 at 3 GHz
    assert c['f3db_ghz'] == pytest.approx(3.0)


def test_curve_out_of_range_returns_none():
    freqs = [2e9, 3e9]           # doesn't cover 1 GHz
    s21 = [complex(0.9, 0), complex(0.8, 0)]
    c = insertion_loss_curve(freqs, s21)
    assert c['il_at_1ghz_db'] is None


# ---------------------------------------------------------------------------
# collect_sim_results — curve wired in
# ---------------------------------------------------------------------------


def test_collect_includes_il_curve(tmp_path):
    task_dir = tmp_path / 'T1'
    task_dir.mkdir(parents=True)
    (task_dir / 'net.s2p').write_text(
        (FIXTURE / 'net114.s2p').read_text())
    (task_dir / 'summary.json').write_text(
        (FIXTURE / 'summary.json').read_text())
    out = collect_sim_results(tmp_path, {'T1': 'net114'})
    curve = out['net114']['insertion_loss_curve']
    assert len(curve['freqs_ghz']) == 201
    assert curve['worst_il_db'] is not None
    # A good short trace has near-zero loss at low freq
    assert curve['il_db'][0] > -1.0


# ---------------------------------------------------------------------------
# render_si_report
# ---------------------------------------------------------------------------


def _eval_with_sim():
    return {
        'dataset': 'DS', 'bga': 'B',
        'metrics': {'si': {'simulated': {'sim_results': {
            'netA': {
                'simulated_z0_ohm': 58.5,
                'insertion_loss_curve': {
                    'freqs_ghz': [1.0, 2.0, 3.0],
                    'il_db': [-0.1, -0.5, -1.2],
                    'il_at_1ghz_db': -0.1, 'il_at_5ghz_db': None,
                    'f3db_ghz': None, 'worst_il_db': -1.2},
            }}}}},
    }


def test_render_contains_net_and_svg():
    html_text = render_si_report(_eval_with_sim())
    assert 'netA' in html_text
    assert '<svg' in html_text
    assert 'polyline' in html_text
    assert '58.5 Ω' in html_text


def test_render_no_sim_graceful():
    html_text = render_si_report({'dataset': 'X', 'metrics': {}})
    assert 'no simulated results' in html_text


def test_render_no_fetch():
    html_text = render_si_report(_eval_with_sim())
    assert 'fetch(' not in html_text


def test_write_si_report_file(tmp_path):
    src = tmp_path / 'e.json'
    src.write_text(json.dumps(_eval_with_sim()))
    out = tmp_path / 'si.html'
    p = write_si_report(src, out)
    assert p.exists()
    assert '<svg' in p.read_text()
