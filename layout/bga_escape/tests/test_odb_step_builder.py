# odb_step_builder 단위 테스트 — odb_to_step_example 바이너리 래퍼
"""ODB++ → STEP conversion wrapper tests."""

from __future__ import annotations

from pathlib import Path

import pytest

from bga_router.integrations import odb_step_builder


class _FakeProc:
    def __init__(self, returncode=0, stderr=''):
        self.returncode = returncode
        self.stderr = stderr
        self.stdout = 'Conversion complete'


def test_find_binary_explicit(tmp_path):
    fake = tmp_path / 'odb_to_step_example'
    fake.write_text('#!/bin/sh\n')
    fake.chmod(0o755)
    assert odb_step_builder.find_binary(fake) == fake


def test_find_binary_nonexistent_explicit_falls_through(tmp_path):
    r = odb_step_builder.find_binary(tmp_path / 'nope')
    assert r != tmp_path / 'nope'


def test_default_binary_path_shape():
    assert odb_step_builder.DEFAULT_BINARY.name == 'odb_to_step_example'
    assert 'build' in str(odb_step_builder.DEFAULT_BINARY)


def _install_fake_binary(tmp_path, monkeypatch):
    fake = tmp_path / 'odb_to_step_example'
    fake.write_text('#!/bin/sh\n')
    fake.chmod(0o755)
    monkeypatch.setattr(odb_step_builder, 'DEFAULT_BINARY', fake)
    return fake


def test_build_step_command_assembly(tmp_path, monkeypatch):
    _install_fake_binary(tmp_path, monkeypatch)
    captured = {}

    def fake_run(cmd, **kwargs):
        captured['cmd'] = cmd
        captured['kwargs'] = kwargs
        return _FakeProc(returncode=0)

    monkeypatch.setattr(odb_step_builder.subprocess, 'run', fake_run)

    ret = odb_step_builder.build_step(
        odb_dir='/odb', config_yaml='/cfg.yaml',
        output_dir=tmp_path / 'out')

    cmd = captured['cmd']
    assert '--odb' in cmd and cmd[cmd.index('--odb') + 1] == '/odb'
    assert '--config' in cmd and cmd[cmd.index('--config') + 1] == '/cfg.yaml'
    assert '--output' in cmd
    assert cmd[cmd.index('--output') + 1] == str(tmp_path / 'out')
    # 옵션 미지정 시 nets/per-net 플래그 없음
    assert '--nets' not in cmd
    assert '--per-net' not in cmd
    # subprocess.run 호출 인자 확인
    assert captured['kwargs'].get('capture_output') is True
    assert captured['kwargs'].get('text') is True
    assert ret == Path(tmp_path / 'out')


def test_build_step_command_with_nets(tmp_path, monkeypatch):
    _install_fake_binary(tmp_path, monkeypatch)
    captured = {}

    def fake_run(cmd, **kwargs):
        captured['cmd'] = cmd
        return _FakeProc(returncode=0)

    monkeypatch.setattr(odb_step_builder.subprocess, 'run', fake_run)

    odb_step_builder.build_step(
        odb_dir='/odb', config_yaml='/cfg.yaml',
        output_dir=tmp_path / 'out', nets=['a', 'b'])

    cmd = captured['cmd']
    idx = cmd.index('--nets')
    # '--nets' 바로 뒤에 콤마 결합된 값이 인접
    assert cmd[idx + 1] == 'a,b'


def test_build_step_command_per_net(tmp_path, monkeypatch):
    _install_fake_binary(tmp_path, monkeypatch)
    captured = {}

    def fake_run(cmd, **kwargs):
        captured['cmd'] = cmd
        return _FakeProc(returncode=0)

    monkeypatch.setattr(odb_step_builder.subprocess, 'run', fake_run)

    odb_step_builder.build_step(
        odb_dir='/odb', config_yaml='/cfg.yaml',
        output_dir=tmp_path / 'out', per_net=True)

    assert '--per-net' in captured['cmd']


def test_build_step_raises_on_nonzero_rc(tmp_path, monkeypatch):
    _install_fake_binary(tmp_path, monkeypatch)

    def fake_run(cmd, **kwargs):
        return _FakeProc(returncode=1, stderr='boom')

    monkeypatch.setattr(odb_step_builder.subprocess, 'run', fake_run)

    with pytest.raises(RuntimeError, match='rc=1'):
        odb_step_builder.build_step(
            odb_dir='/odb', config_yaml='/cfg.yaml',
            output_dir=tmp_path / 'out')


def test_build_step_raises_when_no_binary(tmp_path, monkeypatch):
    monkeypatch.setattr(odb_step_builder, 'DEFAULT_BINARY',
                        tmp_path / 'does_not_exist')
    monkeypatch.setattr('shutil.which', lambda name: None)
    with pytest.raises(FileNotFoundError, match='odb_to_step_example'):
        odb_step_builder.build_step(
            odb_dir=tmp_path, config_yaml=tmp_path / 'cfg.yaml',
            output_dir=tmp_path / 'out')


def test_build_step_if_missing_noop_when_assembly_exists(tmp_path):
    d = tmp_path / 'step'
    d.mkdir()
    (d / 'assembly.step').write_text('ISO-10303-21;')
    ret = odb_step_builder.build_step_if_missing(
        d, odb_dir=tmp_path, config_yaml=tmp_path / 'cfg.yaml')
    assert ret == d


# --- 가드된 실제 실행 테스트 (바이너리 + ODB 존재 시에만) ---

_REAL_ODB = Path('/data/backup/pyKooCAEPrev/pyKooCAE/P3_EUR_REV03Sample')
_REAL_CONFIG = (odb_step_builder.REPO_ROOT / 'examples' / 'ECAD'
                / 'p3_stackup.yaml')
_REAL_BINARY = odb_step_builder.find_binary()


@pytest.mark.skipif(
    _REAL_BINARY is None or not _REAL_ODB.exists()
    or not _REAL_CONFIG.exists(),
    reason='odb_to_step_example binary 또는 P3 ODB/config 부재')
def test_build_step_real_execution(tmp_path):
    out = odb_step_builder.build_step(
        odb_dir=_REAL_ODB,
        config_yaml=_REAL_CONFIG,
        output_dir=tmp_path,
        nets=['net028'])
    assert (out / 'assembly.step').exists()
