# AIDataHub REST 연동 — ODB 과제 레코드 등록/조회 (과제명/Rev/개발단계 기반)
"""Phase M — AIDataHub client.

AIDataHub(사내 데이터 허브)에 ODB 설계 데이터를 과제 메타와 함께
등록하고, 과제명(project)/Rev(version)/개발단계(stage)로 조회한다.

실사 확인 사항(중요).
- 과제명 → record.project, Rev → record.version, 개발단계 → 전용 필드
  없음. project/version은 저장되나 **조회 필터가 안 됨**.
- 따라서 조회 가능하도록 tags에 반드시 함께 실는다:
  tags=['project:Z3', 'rev:B', 'stage:DV'].
- 등록: POST /api/records/import (헤더 X-API-Key).
- 조회: GET /api/records?tag=project:Z3&tag=rev:B&...
- 단건: GET /api/records/{id}.

의존성 zero — urllib만 사용(requests 회피).
"""

from __future__ import annotations

import json
import os
import urllib.error
import urllib.parse
import urllib.request
from typing import Any, Dict, List, Optional


DEFAULT_BASE_URL = os.environ.get('AIDH_BASE_URL', 'http://localhost:8000')
DEFAULT_API_KEY = os.environ.get('AIDH_API_KEY')


def _tags_for(project: Optional[str], rev: Optional[str],
               stage: Optional[str], extra: Optional[List[str]] = None
               ) -> List[str]:
    """과제 메타를 조회 가능한 tag로 변환."""
    tags: List[str] = []
    if project:
        tags.append(f'project:{project}')
    if rev:
        tags.append(f'rev:{rev}')
    if stage:
        tags.append(f'stage:{stage}')
    if extra:
        tags.extend(extra)
    return tags


class AIDataHubClient:
    """AIDataHub REST 얇은 래퍼."""

    def __init__(self, base_url: Optional[str] = None,
                 api_key: Optional[str] = None, *, timeout_s: int = 30):
        self.base_url = (base_url or DEFAULT_BASE_URL).rstrip('/')
        self.api_key = api_key or DEFAULT_API_KEY
        self.timeout_s = timeout_s

    # -- low level ------------------------------------------------------
    def _request(self, method: str, path: str, *,
                  query: Optional[Dict[str, Any]] = None,
                  body: Optional[Any] = None,
                  need_key: bool = False) -> Any:
        url = self.base_url + path
        if query:
            # tag는 반복 파라미터 → doseq
            url += '?' + urllib.parse.urlencode(query, doseq=True)
        data = None
        headers = {'Accept': 'application/json'}
        if body is not None:
            data = json.dumps(body).encode('utf-8')
            headers['Content-Type'] = 'application/json'
        if self.api_key:
            headers['X-API-Key'] = self.api_key
        elif need_key:
            raise RuntimeError(
                'AIDataHub API key 필요(import 엔드포인트). '
                'AIDH_API_KEY 환경변수 또는 api_key 인자로 전달.')
        req = urllib.request.Request(url, data=data, headers=headers,
                                      method=method)
        try:
            with urllib.request.urlopen(req, timeout=self.timeout_s) as resp:
                raw = resp.read().decode('utf-8')
                return json.loads(raw) if raw else {}
        except urllib.error.HTTPError as e:
            detail = e.read().decode('utf-8', errors='replace')[:400]
            raise RuntimeError(
                f'AIDataHub {method} {path} → HTTP {e.code}: {detail}') from e
        except urllib.error.URLError as e:
            raise RuntimeError(
                f'AIDataHub 연결 실패 ({url}): {e.reason}') from e

    # -- register -------------------------------------------------------
    def register_odb(self, *, project: str, rev: str, stage: str,
                      title: str, team: str = 'MX', group: str = 'ECAD',
                      year: int = 2026, file_metadata: Optional[dict] = None,
                      record_id: Optional[str] = None,
                      extra_tags: Optional[List[str]] = None) -> Dict[str, Any]:
        """ODB 설계 CAD 레코드를 과제 메타와 함께 등록.

        POST /api/records/import (X-API-Key 필요). project/version 저장 +
        조회용 tags(project/rev/stage) 동시 부여.
        """
        record: Dict[str, Any] = {
            'data_type': 'CAD',
            'team': team, 'group': group, 'year': year,
            'title': title,
            'project': project,
            'version': rev,
            'domain': project,
            'tags': _tags_for(project, rev, stage, extra_tags),
            'content': {
                'cad_type': 'ECAD',
                'file_format': 'ODB++',
                'file_metadata': file_metadata or {},
            },
        }
        if record_id:
            record['id'] = record_id
        return self._request('POST', '/api/records/import',
                              query={'auto_seq': 'true'},
                              body=record, need_key=True)

    # -- query ----------------------------------------------------------
    def find_records(self, *, project: Optional[str] = None,
                      rev: Optional[str] = None, stage: Optional[str] = None,
                      data_type: str = 'CAD', limit: int = 20,
                      extra_tags: Optional[List[str]] = None
                      ) -> Dict[str, Any]:
        """과제명/Rev/개발단계로 레코드 조회 (tag 필터)."""
        query: Dict[str, Any] = {'data_type': data_type, 'limit': limit}
        tags = _tags_for(project, rev, stage, extra_tags)
        if tags:
            query['tag'] = tags       # 반복 파라미터
        return self._request('GET', '/api/records', query=query)

    def get_record(self, record_id: str) -> Dict[str, Any]:
        """단건 레코드 조회."""
        return self._request(
            'GET', '/api/records/' + urllib.parse.quote(record_id))

    def resolve_odb_path(self, record: Dict[str, Any]) -> Optional[str]:
        """레코드에서 ODB 로컬 경로 힌트 추출.

        content.file_metadata.odb_dir 또는 local_path 관례. 없으면 None
        (호출자가 attachment 경로를 별도 해석).
        """
        content = record.get('content') or {}
        fm = content.get('file_metadata') or {}
        return fm.get('odb_dir') or fm.get('local_path') or record.get('odb_dir')
