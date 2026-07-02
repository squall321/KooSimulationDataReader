# em_data_builder 단위 테스트 — odb_to_em_json 자동 호출
"""Phase H-1 — em_data auto-build tests."""

from __future__ import annotations

from pathlib import Path

import pytest

from bga_router.integrations import em_data_builder


def test_find_binary_with_explicit_path(tmp_path):
    fake = tmp_path / 'odb_to_em_json'
    fake.write_text('#!/bin/sh\n')
    fake.chmod(0o755)
    assert em_data_builder.find_binary(fake) == fake


def test_find_binary_with_nonexistent_explicit(tmp_path):
    result = em_data_builder.find_binary(tmp_path / 'nope')
    # Falls back to DEFAULT_BINARY (may or may not exist depending on build)
    # But should NOT return the nonexistent explicit path.
    assert result != tmp_path / 'nope'


def test_build_em_data_raises_when_no_binary(tmp_path, monkeypatch):
    monkeypatch.setattr(em_data_builder, 'DEFAULT_BINARY',
                          tmp_path / 'does_not_exist')
    monkeypatch.setattr('shutil.which', lambda name: None)
    with pytest.raises(FileNotFoundError, match='odb_to_em_json'):
        em_data_builder.build_em_data(
            odb_dir=tmp_path, stackup_yaml=tmp_path / 'stack.yaml',
            output=tmp_path / 'em.json')


def test_build_em_data_if_missing_noop_when_exists(tmp_path):
    p = tmp_path / 'em.json'
    p.write_text('{}')
    ret = em_data_builder.build_em_data_if_missing(
        p, odb_dir=tmp_path, stackup_yaml=tmp_path / 'stack.yaml')
    assert ret == p


def test_build_em_data_if_missing_treats_empty_file_as_missing(tmp_path, monkeypatch):
    p = tmp_path / 'em.json'
    p.write_text('')  # empty → treated as missing
    monkeypatch.setattr(em_data_builder, 'DEFAULT_BINARY',
                          tmp_path / 'does_not_exist')
    monkeypatch.setattr('shutil.which', lambda name: None)
    with pytest.raises(FileNotFoundError):
        em_data_builder.build_em_data_if_missing(
            p, odb_dir=tmp_path, stackup_yaml=tmp_path / 'stack.yaml')
