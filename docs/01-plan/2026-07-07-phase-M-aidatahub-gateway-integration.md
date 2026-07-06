# Phase M — AIDataHub / MCP 게이트웨이 연동 운영 절차서

**Date**: 2026-07-07
**Scope**: 이미 구현된 `aidatahub_client.py` / `gateway_register.py` / `mcp_server.py`를 실제 AIDataHub 인스턴스와 HWAXPortal MCP 게이트웨이에 연결하기 위한 운영 절차.
**성격**: 신규 설계가 아니라 **연동 절차서**다. 각 단계는 실제 코드 시그니처와 REST payload를 그대로 인용한다. "현재 되는 것"과 "브리지가 있어야 되는 것"을 구분해 기술한다.

---

## 1. 개요

### 목적

사용자(또는 Claude)가 게이트웨이에서 **과제명·Rev·개발단계만 말하면**, odb-analyzer MCP가 AIDataHub에서 해당 ODB++ 설계를 찾아 내려받고 구조/배선/대리모델 분석을 돌려주는 것. 진입점은 MCP tool `analyze_project` 하나다.

### 구성 요소

| 요소 | 실체 | 위치 |
|---|---|---|
| REST 클라이언트 | `AIDataHubClient` (urllib only, X-API-Key) | `bga_router/integrations/aidatahub_client.py` |
| 게이트웨이 등록 helper | `build_*_upstream` / `register_upstream` | `bga_router/integrations/gateway_register.py` |
| MCP 서버 | `name='odb-analyzer'`, tool `analyze_project` 등 13종 | `bga_router/mcp_server.py` |
| 데이터 허브 | AIDataHub (`POST /api/records/import`, `GET /api/records`) | `http://<host>:8000` |
| 게이트웨이 | AIDataHub MCP federation (`POST /api/mcp/upstreams`) | 동일 호스트, 포털 뒤 `/ai-data-hub/` |

### 데이터 흐름

```
[사용자 / Claude]
   │  "과제 Z3 Rev B, 개발단계 DV — 배선 분석해줘"
   ▼
[HWAXPortal MCP 게이트웨이]                    (= AIDataHub MCP federation)
   │  노출 tool 호출: odb_analyzer__analyze_project(project=Z3, rev=B, stage=DV)
   ▼
[odb-analyzer MCP]                            (python -m bga_router.mcp_server, stdio)
   │  ① AIDataHubClient.find_records(project=Z3, rev=B, stage=DV)
   ▼
[AIDataHub REST]  GET /api/records?data_type=CAD&tag=project:Z3&tag=rev:B&tag=stage:DV
   │  → items[0].content.file_metadata.odb_dir  (로컬 ODB 경로)
   ▼
[odb-analyzer MCP]  ② resolve_odb_path → odb_dir
   │                ③ analysis 분기: inspect / packages / metamodel
   ▼
[분석 결과 JSON]  →  게이트웨이  →  사용자
```

핵심 제약을 먼저 못박는다. 3번 화살표(게이트웨이→MCP 호출)는 **게이트웨이가 stdio를 실제로 dispatch하지 못한다**. odb-analyzer는 stdio MCP이므로, 실동작에는 stdio→HTTP 브리지가 필요하다(5절, 6절). 등록·조회(1·2번 화살표)는 브리지 없이 지금 그대로 동작한다.

---

## 2. 데이터 모델 매핑

### 과제 메타 → AIDataHub 필드

| 과제 개념 | 예 | 저장 필드 | 조회 필터 가능? |
|---|---|---|---|
| 과제명 | `Z3` | `record.project` (+ `record.domain`) | **불가** |
| Rev | `B` | `record.version` | **불가** |
| 개발단계 | `DV` | 전용 필드 없음 | — |
| (조회용 태그) | — | `record.tags = ['project:Z3','rev:B','stage:DV']` | **가능 (`tag` 반복 파라미터)** |

### 결정적 제약 — 반드시 tag로도 실어라

`GET /api/records`의 필터 파라미터는 `data_type / team / group / year / agent / tag / q / include_deleted / limit / offset`뿐이다. **`project`·`version`으로는 필터가 안 된다**(저장은 되지만 검색 조건에 없음). 따라서 등록 시 과제 메타를 `tags`에 `project:` / `rev:` / `stage:` 접두 형태로 **반드시 중복 기록**해야 나중에 조회가 된다.

이 변환은 `aidatahub_client.py`의 `_tags_for()`가 자동으로 수행하므로, `register_odb(project=, rev=, stage=)`만 채우면 tag가 자동 생성된다. 수기 curl로 등록할 때는 직접 넣어야 한다(3절 curl 예시 참조).

### 자동 채번 ID

`auto_seq=true`로 등록하면 서버가 다음 형식으로 id를 부여한다.

```
{DATA_TYPE}-{TEAM}-{GROUP}-{YEAR}-{SEQ:010d}
예) CAD-MX-ECAD-2026-0000000001
```

`register_odb`의 기본값은 `team='MX'`, `group='ECAD'`, `year=2026`, `data_type='CAD'`이다.

---

## 3. 등록 절차

### 3-1. 파이썬 클라이언트로 등록 (권장)

`AIDataHubClient.register_odb`의 실제 시그니처.

```python
register_odb(*, project: str, rev: str, stage: str, title: str,
             team: str = 'MX', group: str = 'ECAD', year: int = 2026,
             file_metadata: Optional[dict] = None,
             record_id: Optional[str] = None,
             extra_tags: Optional[List[str]] = None) -> dict
```

호출 예.

```python
from bga_router.integrations.aidatahub_client import AIDataHubClient

client = AIDataHubClient()  # AIDH_BASE_URL / AIDH_API_KEY 환경변수 사용
resp = client.register_odb(
    project='Z3', rev='B', stage='DV',
    title='Z3 mainboard ODB++ (DV)',
    file_metadata={'odb_dir': '/data/ecad/z3_revB_dv/odb'},
)
print(resp['id'])   # → CAD-MX-ECAD-2026-0000000001
```

내부적으로 다음 record를 만들어 `POST /api/records/import?auto_seq=true`(헤더 `X-API-Key`)로 보낸다.

```json
{
  "data_type": "CAD",
  "team": "MX", "group": "ECAD", "year": 2026,
  "title": "Z3 mainboard ODB++ (DV)",
  "project": "Z3",
  "version": "B",
  "domain": "Z3",
  "tags": ["project:Z3", "rev:B", "stage:DV"],
  "content": {
    "cad_type": "ECAD",
    "file_format": "ODB++",
    "file_metadata": { "odb_dir": "/data/ecad/z3_revB_dv/odb" }
  }
}
```

`API key`가 없으면 `register_odb`는 예외를 던진다(import 엔드포인트는 `need_key=True`). `AIDH_API_KEY` 환경변수 또는 `AIDataHubClient(api_key=...)`로 주입한다.

### 3-2. 수기 curl로 등록

동일 payload를 curl로 보낼 때. **`tags`에 project/rev/stage를 직접 넣어야 조회된다**.

```bash
curl -X POST "http://<host>:8000/api/records/import?auto_seq=true" \
  -H "X-API-Key: $AIDH_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "data_type": "CAD",
    "team": "MX", "group": "ECAD", "year": 2026,
    "title": "Z3 mainboard ODB++ (DV)",
    "project": "Z3", "version": "B", "domain": "Z3",
    "tags": ["project:Z3", "rev:B", "stage:DV"],
    "content": {
      "cad_type": "ECAD", "file_format": "ODB++",
      "file_metadata": { "odb_dir": "/data/ecad/z3_revB_dv/odb" }
    }
  }'
```

### 3-3. ODB++ 바이너리 배치

**REST에는 attachment를 올리는 POST 엔드포인트가 없다.** ODB++ 실물(디렉터리/tgz)은 REST로 전송하지 않고, 두 관례 중 하나로 배치한다.

1. **로컬 경로 기록 (현행 기본)** — ODB++를 분석 노드가 읽을 수 있는 공유 경로에 두고, 그 절대 경로를 `content.file_metadata.odb_dir`에 기록한다. `resolve_odb_path()`가 `odb_dir` → `local_path` → `record.odb_dir` 순으로 해석한다.
2. **attachments 관례** — `attachments_dir/{record_id}/`에 실물을 배치하고 `odb_dir`를 그 경로로 기록한다. record_id는 등록 응답의 `id`를 사용한다(선등록 후 배치, 또는 `record_id=`로 사전 지정).

어느 쪽이든 `analyze_project`가 경로를 해석하려면 `content.file_metadata.odb_dir`가 채워져 있어야 한다. 비어 있으면 MCP는 "레코드는 찾았으나 ODB 로컬 경로 미해석" note를 반환한다.

---

## 4. 조회·분석 절차

### 4-1. analyze_project MCP tool 흐름

`tool_analyze_project`(`mcp_server.py`)의 실제 동작 순서.

1. `project` 필수(없으면 ValueError). `rev`·`stage`는 선택.
2. `AIDataHubClient.find_records(project, rev, stage, limit)` 호출 → tag 필터 조회.
3. `items`가 비면 `found:0` + "등록 필요(register_odb)" note 반환.
4. `items[0]`을 채택 → `resolve_odb_path(record)` → `odb_dir`.
5. `odb_dir`가 없으면 note 반환. 있으면 `analysis` 분기 실행.
   - `inspect` (기본) → `tool_odb_inspect({'odb_dir': ...})` : 레이어/타입/feature/심볼 요약.
   - `packages` → `tool_package_features` : 패키지 물리 피처(크기/side/겹침).
   - `metamodel` → `tool_metamodel_infer` : 상·하단 패키지 쌍 위험도(기본 `thermal_shock_v0`).

tool 입력 스키마(발췌).

```
analyze_project(project*, rev, stage,
                analysis=inspect|packages|metamodel,
                metamodel='thermal_shock_v0',
                base_url, api_key, limit=10, min_overlap_ratio=0.0)
```

### 4-2. find_records 직접 조회 (curl)

`find_records`가 만드는 실제 요청은 다음과 같다. `tag`는 반복 파라미터(`doseq`)로 나간다.

```bash
curl "http://<host>:8000/api/records?data_type=CAD&tag=project:Z3&tag=rev:B&tag=stage:DV&limit=20"
```

응답의 `items[]`에서 첫 레코드의 `content.file_metadata.odb_dir`가 분석 대상 경로다. 단건 상세는 다음.

```bash
curl "http://<host>:8000/api/records/CAD-MX-ECAD-2026-0000000001"
```

### 4-3. 파이썬 조회

```python
found = client.find_records(project='Z3', rev='B', stage='DV', limit=10)
rec = found['items'][0]
odb_dir = client.resolve_odb_path(rec)   # content.file_metadata.odb_dir
```

---

## 5. 게이트웨이 등록

### 5-1. 게이트웨이 = AIDataHub MCP federation

MCP 게이트웨이는 별도 서비스가 아니라 AIDataHub 안의 MCP federation이다. upstream을 `POST /api/mcp/upstreams`로 등록하면, 게이트웨이가 부팅/ping 시 그 upstream의 `tools/list`를 받아 `{alias}__{tool}` 이름으로 재노출한다. 즉 우리 tool은 `odb_analyzer__analyze_project`로 보이게 된다.

upstream 스키마 필드(마이그레이션 0023 기준): `alias`, `transport(http|stdio)`, `url`, `command`, `command_args`, `auth{type, env_var}`, `description_prefix`, `tls_verify`, `enabled`, `rate_limit_per_min`. **`alias`는 `^[a-z][a-z0-9_]{2,30}$`** — dash 불가라 `odb_analyzer`처럼 언더스코어를 쓴다.

### 5-2. 결정적 제약 — stdio는 스키마만 받고 미동작

게이트웨이는 stdio transport row를 **받아 저장은 하지만 실제로 연결하지 않는다**. 두 지점에서 막힌다.

- `mcp_federation.py:339` `_open_session()` — `transport != 'http'`이면 `NotImplementedError("... Phase 2: stdio")`.
- `mcp_federation.py:696` `register_all_upstreams()` — `transport != 'http'`이면 `continue`로 skip(로그 "reserved for Phase 2").

게다가 0023 스키마에는 **`cwd`·`env` 필드가 없다**. 우리 MCP는 `cwd=.../layout/bga_escape`에서 실행돼야 import가 풀리므로, stdio row로는 cwd를 전달할 수단조차 없다.

결론: **stdio 직접 등록은 실동작하지 않는다.** `build_stdio_upstream()`은 Phase 2 대비용 payload 생성기일 뿐이며, 스스로 `_note`에 "게이트웨이 미구현"을 적어 둔다.

### 5-3. 실동작 경로 — stdio→HTTP 브리지 후 http로 등록

odb-analyzer(stdio)를 mcp-proxy 같은 브리지로 감싸 HTTP MCP 엔드포인트로 노출하고, 게이트웨이에는 `transport=http`로 등록한다. 브리지 프로세스가 cwd를 잡으므로 게이트웨이 스키마의 cwd 부재 문제도 우회된다.

브리지 기동(참고 커맨드, `gateway_register.bridge_command()`).

```bash
cd /home/koopark/claude/KooSimulationDataReader/layout/bga_escape
mcp-proxy --sse-port 9040 -- python -m bga_router.mcp_server
# → http://localhost:9040/ 에 MCP 엔드포인트 노출
```

등록 payload(`build_http_upstream(url, alias='odb_analyzer', token_env='ODB_MCP_TOKEN', tls_verify=True)`가 생성).

```json
{
  "alias": "odb_analyzer",
  "transport": "http",
  "url": "http://localhost:9040/mcp/",
  "auth": { "type": "bearer", "env_var": "ODB_MCP_TOKEN" },
  "description_prefix": "[ODB] ",
  "tls_verify": true,
  "enabled": true
}
```

REST 등록. `register_upstream()`은 payload에서 `_`로 시작하는 키를 제거한 뒤 `POST /api/mcp/upstreams`(헤더 `X-API-Key`)로 보낸다.

```bash
curl -X POST "http://<host>:8000/api/mcp/upstreams" \
  -H "X-API-Key: $AIDH_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "alias": "odb_analyzer",
    "transport": "http",
    "url": "http://localhost:9040/mcp/",
    "auth": { "type": "bearer", "env_var": "ODB_MCP_TOKEN" },
    "description_prefix": "[ODB] ",
    "tls_verify": true,
    "enabled": true
  }'
```

파이썬으로 등록할 때.

```python
from bga_router.integrations.gateway_register import (
    build_http_upstream, register_upstream)

payload = build_http_upstream('http://localhost:9040/mcp/')
register_upstream(payload)   # AIDH_BASE_URL / AIDH_API_KEY 사용
```

### 5-4. ping 검증

등록(HTTP 201) 자체는 즉시 dispatch를 뜻하지 않는다(FastMCP 동적 등록은 다음 부팅 또는 ping 시점). 등록 직후 health check로 `tools/list`가 실제로 도는지 확인한다.

```bash
curl -X POST "http://<host>:8000/api/mcp/upstreams/odb_analyzer/ping" \
  -H "X-API-Key: $AIDH_API_KEY"
```

성공하면 게이트웨이의 tool 목록에 `odb_analyzer__analyze_project`, `odb_analyzer__route`, `odb_analyzer__odb_inspect` 등이 나타난다. 그때부터 사용자가 과제명을 물으면 `analyze_project`가 호출된다.

---

## 6. 제약 및 후속 작업

### 현재 상태 요약

| 항목 | 상태 |
|---|---|
| ODB 레코드 등록 (`register_odb` / import) | **동작** |
| 과제명·Rev·단계 조회 (`find_records` / tag) | **동작** (tag 필수) |
| ODB 바이너리 REST 업로드 | **없음** — 로컬 경로/attachments 관례로 대체 |
| `analyze_project` 로컬 실행 (MCP client 직접) | **동작** |
| 게이트웨이 stdio 직접 등록 | **미동작** (스키마만 수용) |
| 게이트웨이 HTTP 브리지 경유 | **동작** (브리지 배포 필요) |

### stdio 네이티브 지원을 위한 게이트웨이 확장점 (후속)

브리지 없이 stdio를 직접 물리려면 AIDataHub 게이트웨이 3곳을 손봐야 한다. 이건 우리 저장소가 아니라 AIDataHub 측 작업이다.

| # | 위치 | 현재 | 필요 |
|---|---|---|---|
| 1 | `mcp_federation.py:339` `_open_session` | `transport!='http'` → `NotImplementedError` | stdio 클라이언트(`stdio_client`) 세션 구현 |
| 2 | `mcp_federation.py:696` `register_all_upstreams` | `transport!='http'` → `continue` skip | stdio row도 register 대상에 포함 |
| 3 | `alembic/0023_mcp_upstreams` 스키마 | `cwd`·`env` 컬럼 없음 | `cwd TEXT`, `env JSONB` 추가 (stdio 실행 컨텍스트 전달) |

3개가 모두 반영돼야 `build_stdio_upstream()` payload가 의미를 갖는다. 그 전까지는 5-3 HTTP 브리지가 유일한 실동작 경로다.

### 기타 후속

- 브리지 배포 자동화(mcp-proxy 또는 FastMCP HTTP 래퍼)는 운영 배포 항목이며 본 저장소 범위 밖이다. `bridge_command()`는 참고용 커맨드만 제공한다.
- ODB 바이너리 대용량 전송은 현재 공유 스토리지 전제. REST attachment가 생기면 `resolve_odb_path` 관례를 그쪽으로 확장.

---

## 7. 환경변수

| 변수 | 용도 | 기본값 / 비고 |
|---|---|---|
| `AIDH_BASE_URL` | AIDataHub REST/게이트웨이 base URL | `http://localhost:8000`. 포털 뒤에서는 `/ai-data-hub/` prefix |
| `AIDH_API_KEY` | REST 인증 헤더 `X-API-Key`. import 등록에 필수 | 없으면 `register_odb`·`register_upstream`에서 인증 실패 |
| `ODB_MCP_TOKEN` | 게이트웨이→브리지 HTTP 호출용 bearer 토큰 | upstream `auth.env_var`가 참조하는 이름. 게이트웨이 프로세스 환경에 존재해야 함 |

`AIDataHubClient`와 `gateway_register`는 셋 다 인자로 직접 주입도 가능하다(예: `AIDataHubClient(base_url=, api_key=)`). 환경변수 미설정 시 base URL은 `http://localhost:8000`으로 폴백한다.
