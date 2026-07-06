# odb_json_builder 단위 테스트 — odb_to_json 바이너리 래퍼
"""Phase K — odb_to_json wrapper tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations import odb_json_builder


def test_find_binary_explicit(tmp_path):
    fake = tmp_path / 'odb_to_json'
    fake.write_text('#!/bin/sh\n')
    fake.chmod(0o755)
    assert odb_json_builder.find_binary(fake) == fake


def test_find_binary_nonexistent_explicit_falls_through(tmp_path):
    r = odb_json_builder.find_binary(tmp_path / 'nope')
    assert r != tmp_path / 'nope'


def test_build_raises_when_no_binary(tmp_path, monkeypatch):
    monkeypatch.setattr(odb_json_builder, 'DEFAULT_BINARY',
                          tmp_path / 'does_not_exist')
    monkeypatch.setattr('shutil.which', lambda name: None)
    with pytest.raises(FileNotFoundError, match='odb_to_json'):
        odb_json_builder.build_odb_json(
            odb_dir=tmp_path, output_dir=tmp_path / 'out')


def test_build_if_missing_noop_when_index_exists(tmp_path):
    d = tmp_path / 'json'
    d.mkdir()
    (d / 'index.json').write_text('{}')
    ret = odb_json_builder.build_odb_json_if_missing(d, odb_dir=tmp_path)
    assert ret == d


def test_default_binary_path_shape():
    # DEFAULT_BINARY should point at build/bin/examples/odb_to_json
    assert odb_json_builder.DEFAULT_BINARY.name == 'odb_to_json'
    assert 'build' in str(odb_json_builder.DEFAULT_BINARY)
