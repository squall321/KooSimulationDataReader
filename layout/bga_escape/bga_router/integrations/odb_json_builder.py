# odb_to_json C++ 툴을 호출해 ODB++ 를 per-layer JSON 디렉터리로 추출하는 래퍼
"""Phase K — odb_to_json extraction wrapper.

odb_to_em_json (em_data_builder.py) 이 net 폴리곤을 뽑는다면, 이 래퍼는
odb_to_json 을 호출해 레이어별 feature/symbol 통계 JSON을 뽑는다.
odb_inspect 가 소비한다.
"""

from __future__ import annotations

import shutil
import subprocess
from pathlib import Path
from typing import List, Optional


REPO_ROOT = Path(__file__).resolve().parents[4]
DEFAULT_BINARY = REPO_ROOT / 'build' / 'bin' / 'examples' / 'odb_to_json'


def find_binary(explicit: Optional[str | Path] = None) -> Optional[Path]:
    if explicit:
        p = Path(explicit)
        if p.exists():
            return p
    if DEFAULT_BINARY.exists():
        return DEFAULT_BINARY
    which = shutil.which('odb_to_json')
    return Path(which) if which else None


def build_odb_json(*, odb_dir: str | Path,
                     output_dir: str | Path,
                     region: Optional[str] = None,
                     binary: Optional[str | Path] = None,
                     timeout_s: int = 600) -> Path:
    """odb_to_json 실행 → 레이어별 JSON 디렉터리 생성. output_dir 반환."""
    bin_path = find_binary(binary)
    if bin_path is None:
        raise FileNotFoundError(
            f'odb_to_json binary not found at {DEFAULT_BINARY} or on PATH; '
            f'build the C++ tools first (cmake --build build --target '
            f'odb_to_json)')
    out = Path(output_dir)
    out.parent.mkdir(parents=True, exist_ok=True)
    cmd: List[str] = [str(bin_path),
                       '--odb', str(odb_dir),
                       '--output', str(out)]
    if region:
        cmd += ['--region', region]
    proc = subprocess.run(cmd, capture_output=True, text=True,
                            timeout=timeout_s)
    if proc.returncode != 0:
        raise RuntimeError(
            f'odb_to_json failed (rc={proc.returncode}). '
            f'stderr: {proc.stderr[-400:]}')
    return out


def build_odb_json_if_missing(json_dir: str | Path, *,
                                odb_dir: str | Path, **kwargs) -> Path:
    """index.json 이 이미 있으면 no-op; 아니면 생성."""
    p = Path(json_dir)
    if (p / 'index.json').exists():
        return p
    return build_odb_json(odb_dir=odb_dir, output_dir=p, **kwargs)
