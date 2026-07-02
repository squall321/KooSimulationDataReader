# em-run 이 em_data.json 이 없을 때 odb_to_em_json 을 자동 호출해 생성
"""Phase H-1 — em_data.json auto-generator.

The C++ binary `odb_to_em_json` produces the input JSON that both
sol_d and sol_b need. Until now users had to invoke it manually — this
wrapper makes `em-run` fully hands-off when the input is missing.
"""

from __future__ import annotations

import shutil
import subprocess
from pathlib import Path
from typing import List, Optional


REPO_ROOT = Path(__file__).resolve().parents[4]
DEFAULT_BINARY = REPO_ROOT / 'build' / 'bin' / 'examples' / 'odb_to_em_json'


def find_binary(explicit: Optional[str | Path] = None) -> Optional[Path]:
    """Resolve the odb_to_em_json binary path."""
    if explicit:
        p = Path(explicit)
        if p.exists():
            return p
    if DEFAULT_BINARY.exists():
        return DEFAULT_BINARY
    which = shutil.which('odb_to_em_json')
    return Path(which) if which else None


def build_em_data(*,
                    odb_dir: str | Path,
                    stackup_yaml: str | Path,
                    output: str | Path,
                    nets: Optional[List[str]] = None,
                    precision: int = 6,
                    skip_negative: bool = False,
                    binary: Optional[str | Path] = None,
                    timeout_s: int = 600) -> Path:
    """Run odb_to_em_json → write em_data.json. Returns the output Path.

    Raises FileNotFoundError if binary not found, subprocess.CalledProcessError
    on non-zero exit, subprocess.TimeoutExpired on hang.
    """
    bin_path = find_binary(binary)
    if bin_path is None:
        raise FileNotFoundError(
            f'odb_to_em_json binary not found at {DEFAULT_BINARY} or on PATH; '
            f'build the C++ tools first (cmake --build build --target '
            f'odb_to_em_json)')
    out = Path(output)
    out.parent.mkdir(parents=True, exist_ok=True)
    cmd: List[str] = [
        str(bin_path),
        '--odb',    str(odb_dir),
        '--config', str(stackup_yaml),
        '--output', str(out),
        '--precision', str(precision),
    ]
    if nets:
        cmd += ['--nets', ','.join(nets)]
    if skip_negative:
        cmd += ['--skip-negative']
    proc = subprocess.run(cmd, capture_output=True, text=True,
                            timeout=timeout_s)
    if proc.returncode != 0:
        raise RuntimeError(
            f'odb_to_em_json failed (rc={proc.returncode}). '
            f'stderr: {proc.stderr[-400:]}')
    return out


def build_em_data_if_missing(em_data_path: str | Path, *,
                                odb_dir: str | Path,
                                stackup_yaml: str | Path,
                                **kwargs) -> Path:
    """No-op when em_data already exists; auto-generate otherwise."""
    p = Path(em_data_path)
    if p.exists() and p.stat().st_size > 0:
        return p
    return build_em_data(
        odb_dir=odb_dir, stackup_yaml=stackup_yaml, output=p, **kwargs)
