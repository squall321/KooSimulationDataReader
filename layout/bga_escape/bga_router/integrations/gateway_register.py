# odb-analyzer MCP를 AIDataHub MCP federation 게이트웨이에 upstream으로 등록
"""Phase M — HWAXPortal(=AIDataHub MCP federation) 게이트웨이 등록.

실사 결론.
- 게이트웨이 = AIDataHub의 MCP federation (POST /api/mcp/upstreams).
- **stdio는 스키마만 받고 미구현 → HTTP transport만 실동작.**
- 우리 MCP는 stdio(python -m bga_router.mcp_server, cwd 필요)라 그대로는
  연동 불가. 두 경로:
    A. stdio row 등록 (Phase 2 대기용, 실 dispatch 안 됨)
    B. (권장) stdio→HTTP 브리지 후 http로 등록.

이 모듈은 등록 payload 생성 + REST 등록 호출을 제공한다. 브리지 자체는
운영 배포 항목(mcp-proxy / FastMCP HTTP)이라 여기선 payload/등록만.
"""

from __future__ import annotations

import json
import os
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any, Dict, List, Optional


# alias 규칙: ^[a-z][a-z0-9_]{2,30}$ (dash 불가 → 언더스코어)
DEFAULT_ALIAS = 'odb_analyzer'
BGA_ESCAPE_CWD = str(Path(__file__).resolve().parents[2])  # .../layout/bga_escape


def build_stdio_upstream(alias: str = DEFAULT_ALIAS) -> Dict[str, Any]:
    """stdio upstream 등록 payload (Phase 2 대기용 — 현재 dispatch 미동작).

    주의: cwd 필드가 게이트웨이 스키마에 없어 실행 시 import 경로 문제.
    실동작하려면 build_http_upstream + 브리지 사용.
    """
    return {
        'alias': alias,
        'transport': 'stdio',
        'command': 'python',
        'command_args': ['-m', 'bga_router.mcp_server'],
        'description_prefix': '[ODB] ',
        'enabled': True,
        '_note': ('stdio는 게이트웨이 미구현(Phase 2). cwd 전달 수단도 '
                  f'없음. 실행 cwd 필요: {BGA_ESCAPE_CWD}'),
    }


def build_http_upstream(url: str, *, alias: str = DEFAULT_ALIAS,
                          token_env: str = 'ODB_MCP_TOKEN',
                          tls_verify: bool = True) -> Dict[str, Any]:
    """HTTP(streamable) upstream 등록 payload (권장, 실동작).

    url = stdio→HTTP 브리지가 노출하는 MCP 엔드포인트 (예:
    http://localhost:9040/mcp/). 브리지는 cwd를 브리지 프로세스에서 지정.
    """
    return {
        'alias': alias,
        'transport': 'http',
        'url': url,
        'auth': {'type': 'bearer', 'env_var': token_env},
        'description_prefix': '[ODB] ',
        'tls_verify': tls_verify,
        'enabled': True,
    }


def bridge_command(port: int = 9040) -> List[str]:
    """stdio MCP를 Streamable-HTTP로 노출하는 자체 브리지 실행 커맨드.

    외부 의존(mcp-proxy 등) 없이 stdlib http.server만 사용. cwd를
    bga_escape로 두고 실행하면 import 경로가 자동 해결된다. 노출 URL은
    http://<host>:<port>/mcp/ 이며 build_http_upstream의 url과 일치시킨다.
    """
    return ['python', '-m', 'bga_router.http_bridge', '--port', str(port)]


def register_upstream(payload: Dict[str, Any], *,
                        base_url: Optional[str] = None,
                        api_key: Optional[str] = None,
                        timeout_s: int = 30) -> Dict[str, Any]:
    """POST /api/mcp/upstreams 로 upstream 등록.

    payload에서 '_note' 등 언더스코어 prefix 키는 전송 전 제거.
    """
    base = (base_url or os.environ.get('AIDH_BASE_URL',
                                        'http://localhost:8000')).rstrip('/')
    key = api_key or os.environ.get('AIDH_API_KEY')
    clean = {k: v for k, v in payload.items() if not k.startswith('_')}
    data = json.dumps(clean).encode('utf-8')
    headers = {'Content-Type': 'application/json', 'Accept': 'application/json'}
    if key:
        headers['X-API-Key'] = key
    req = urllib.request.Request(base + '/api/mcp/upstreams',
                                  data=data, headers=headers, method='POST')
    try:
        with urllib.request.urlopen(req, timeout=timeout_s) as resp:
            raw = resp.read().decode('utf-8')
            return json.loads(raw) if raw else {}
    except urllib.error.HTTPError as e:
        detail = e.read().decode('utf-8', errors='replace')[:400]
        raise RuntimeError(
            f'gateway register → HTTP {e.code}: {detail}') from e
    except urllib.error.URLError as e:
        raise RuntimeError(f'gateway 연결 실패 ({base}): {e.reason}') from e
