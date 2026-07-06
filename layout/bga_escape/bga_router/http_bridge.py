# stdio MCP(_handle)를 Streamable-HTTP(POST /mcp/)로 노출하는 자체 브리지 — 의존성 zero
"""Phase M-4 — HTTP 브리지.

AIDataHub MCP federation은 Streamable HTTP transport(mcp SDK
streamablehttp_client)로만 upstream에 접속한다. 우리 MCP는 stdio라
그대로는 연동 불가하므로, `bga_router.mcp_server._handle`(순수 JSON-RPC
함수)을 감싸 단일 `POST /mcp/` 엔드포인트로 노출한다.

federation client 규격(실사 확정).
- 단일 엔드포인트 POST /mcp/ 하나. GET /sse·POST /messages 불필요.
- 요청 헤더 Accept: application/json, text/event-stream / Content-Type:
  application/json — 관용(거부 금지). mcp-session-id/mcp-protocol-version
  선택.
- request(id 있음) → HTTP 200 + application/json + JSON-RPC 응답 1개.
- notification(id 없음) → HTTP 202 + 빈 본문.
- **stateless 권장**: mcp-session-id 응답 헤더를 내보내지 않으면 client가
  GET SSE/DELETE를 시도하지 않아 POST 하나로 initialize/tools/list/
  tools/call 전부 성립.
- auth(옵션): Authorization: Bearer <token>.

stdlib http.server만 사용 — 외부 의존 없음. cwd는 이 프로세스를
bga_escape에서 띄우면 자동 해결(federation stdio의 cwd gap 우회).
"""

from __future__ import annotations

import argparse
import json
import os
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from typing import Optional

from bga_router.mcp_server import _handle, SERVER_NAME


# 옵션 bearer 토큰 (env). 설정 시 Authorization 헤더 검사.
_EXPECTED_TOKEN: Optional[str] = None
# federation이 POST 하는 경로. 등록 url의 경로와 일치해야 함.
_MCP_PATH = '/mcp/'


def _make_handler():
    class _BridgeHandler(BaseHTTPRequestHandler):
        # 로그를 stderr로 (stdout은 오염 금지 — 여긴 http라 무관하나 조용히)
        def log_message(self, fmt, *args):
            sys.stderr.write('[bridge] ' + (fmt % args) + '\n')

        def _reject(self, code, msg):
            self.send_response(code)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps({'error': msg}).encode())

        def _check_auth(self) -> bool:
            if _EXPECTED_TOKEN is None:
                return True
            auth = self.headers.get('Authorization', '')
            return auth == f'Bearer {_EXPECTED_TOKEN}'

        def do_GET(self):
            # health/liveness 용. federation은 GET SSE를 안 열지만(stateless)
            # 운영 헬스체크를 위해 200.
            if self.path.rstrip('/') in ('', '/health', '/mcp'):
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps(
                    {'status': 'ok', 'server': SERVER_NAME}).encode())
            else:
                self._reject(404, 'not found')

        def do_POST(self):
            # 경로 관용: /mcp/ 및 /mcp
            if self.path.rstrip('/') != _MCP_PATH.rstrip('/'):
                self._reject(404, f'expected POST {_MCP_PATH}')
                return
            if not self._check_auth():
                self._reject(401, 'unauthorized')
                return
            length = int(self.headers.get('Content-Length', 0) or 0)
            raw = self.rfile.read(length) if length else b''
            try:
                request = json.loads(raw.decode('utf-8'))
            except Exception:
                self._reject(400, 'invalid JSON')
                return
            try:
                response = _handle(request)
            except Exception as e:
                # JSON-RPC internal error
                response = {
                    'jsonrpc': '2.0', 'id': request.get('id'),
                    'error': {'code': -32603,
                              'message': f'{type(e).__name__}: {e}'}}
            if response is None:
                # notification → 202 Accepted, 빈 본문
                self.send_response(202)
                self.end_headers()
                return
            body = json.dumps(response).encode('utf-8')
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Content-Length', str(len(body)))
            # stateless — mcp-session-id 응답 헤더 미발급
            self.end_headers()
            self.wfile.write(body)

    return _BridgeHandler


def serve(host: str = '0.0.0.0', port: int = 9040,
           token: Optional[str] = None) -> None:
    """브리지 HTTP 서버 실행(블로킹)."""
    global _EXPECTED_TOKEN
    _EXPECTED_TOKEN = token or os.environ.get('ODB_MCP_TOKEN')
    server = ThreadingHTTPServer((host, port), _make_handler())
    sys.stderr.write(
        f'[bridge] {SERVER_NAME} on http://{host}:{port}{_MCP_PATH}'
        f' (auth={"on" if _EXPECTED_TOKEN else "off"})\n')
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        server.shutdown()


def main(argv=None) -> int:
    p = argparse.ArgumentParser(
        prog='odb-analyzer-http-bridge',
        description='stdio MCP(odb-analyzer)를 Streamable-HTTP로 노출.')
    p.add_argument('--host', default='0.0.0.0')
    p.add_argument('--port', type=int, default=9040)
    p.add_argument('--token', default=None,
                    help='Bearer 토큰(옵션). 미지정 시 ODB_MCP_TOKEN env.')
    args = p.parse_args(argv)
    serve(host=args.host, port=args.port, token=args.token)
    return 0


if __name__ == '__main__':
    sys.exit(main())
