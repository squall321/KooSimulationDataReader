# PathResult.segment_widths_mm 라우터 자동 기록 단위 테스트
"""Phase E-3 — per-segment width recording tests."""

from __future__ import annotations

from dataclasses import dataclass

import pytest

from src.skills.protocols import PathResult


def test_pathresult_default_segment_widths_empty():
    pr = PathResult(path=[], total_cost=0.0)
    assert pr.segment_widths_mm == []


def test_pathresult_segment_widths_assignable():
    pr = PathResult(path=[], total_cost=0.0)
    pr.segment_widths_mm = [0.075, 0.075, 0.10]
    assert pr.segment_widths_mm == [0.075, 0.075, 0.10]


def test_pathresult_via_metadata_and_widths_independent():
    pr = PathResult(path=[], total_cost=0.0,
                     via_metadata=[{'xy_mm': (0, 0)}],
                     segment_widths_mm=[0.05])
    assert len(pr.via_metadata) == 1
    assert pr.segment_widths_mm == [0.05]
