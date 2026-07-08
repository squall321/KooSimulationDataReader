# odb_to_step_example C++ 툴을 호출해 ODB++ 를 레이어별 STEP(3D CAD) 로 변환하는 래퍼
"""ODB++ → STEP conversion wrapper.

odb_json_builder.py 가 odb_to_json 을 감싸듯이, 이 래퍼는
odb_to_step_example 을 호출해 레이어별 *.step + assembly.step 을 생성한다.
"""

from __future__ import annotations

import shutil
import subprocess
from pathlib import Path
from typing import List, Optional


REPO_ROOT = Path(__file__).resolve().parents[4]
DEFAULT_BINARY = REPO_ROOT / 'build' / 'bin' / 'examples' / 'odb_to_step_example'


def find_binary(explicit: Optional[str | Path] = None) -> Optional[Path]:
    if explicit:
        p = Path(explicit)
        if p.exists():
            return p
    if DEFAULT_BINARY.exists():
        return DEFAULT_BINARY
    which = shutil.which('odb_to_step_example')
    return Path(which) if which else None


def build_step(*, odb_dir: str | Path,
               config_yaml: str | Path,
               output_dir: str | Path,
               nets: Optional[List[str]] = None,
               per_net: bool = False,
               binary: Optional[str | Path] = None,
               timeout_s: int = 600) -> Path:
    """odb_to_step_example 실행 → 레이어별 STEP + assembly.step 생성. output_dir 반환."""
    bin_path = find_binary(binary)
    if bin_path is None:
        raise FileNotFoundError(
            f'odb_to_step_example binary not found at {DEFAULT_BINARY} or on PATH; '
            f'build the C++ tools first (cmake --build build --target '
            f'odb_to_step_example)')
    out = Path(output_dir)
    out.parent.mkdir(parents=True, exist_ok=True)
    cmd: List[str] = [str(bin_path),
                      '--odb', str(odb_dir),
                      '--config', str(config_yaml),
                      '--output', str(out)]
    if nets:
        cmd += ['--nets', ','.join(nets)]
    if per_net:
        cmd += ['--per-net']
    proc = subprocess.run(cmd, capture_output=True, text=True,
                          timeout=timeout_s)
    if proc.returncode != 0:
        raise RuntimeError(
            f'odb_to_step_example failed (rc={proc.returncode}). '
            f'stderr: {proc.stderr[-400:]}')
    return out


def build_step_if_missing(output_dir: str | Path, *,
                          odb_dir: str | Path,
                          config_yaml: str | Path, **kwargs) -> Path:
    """assembly.step 이 이미 있으면 no-op; 아니면 생성."""
    p = Path(output_dir)
    if (p / 'assembly.step').exists():
        return p
    return build_step(odb_dir=odb_dir, config_yaml=config_yaml,
                      output_dir=p, **kwargs)
