# odb_to_json C++ 툴 출력 디렉터리를 읽어 ODB++ 구조를 재라우팅 없이 요약하는 모듈.
"""odb_to_json 출력 디렉터리 인스펙터.

odb_to_json C++ 툴이 만든 디렉터리(index.json + 레이어별 <layer>.json)를 읽어
보드/레이어/피처/심볼 통계를 요약한다. 라우팅은 하지 않는다.
"""

import json
import os


def load_odb_index(json_dir):
    """json_dir 안의 index.json 을 읽어 dict 로 반환한다.

    index.json 이 없으면 FileNotFoundError 를 던진다.
    """
    index_path = os.path.join(json_dir, "index.json")
    if not os.path.isfile(index_path):
        raise FileNotFoundError(f"index.json not found in: {json_dir}")
    with open(index_path, "r", encoding="utf-8") as f:
        return json.load(f)


def _load_layer_json(json_dir, file_name):
    """개별 레이어 json 을 읽어 dict 로 반환한다. 실패하면 None 을 반환한다."""
    if not file_name:
        return None
    layer_path = os.path.join(json_dir, file_name)
    try:
        with open(layer_path, "r", encoding="utf-8") as f:
            return json.load(f)
    except (OSError, ValueError):
        return None


def summarize_odb(json_dir):
    """index.json + 개별 레이어 json 을 종합해 구조 요약을 반환한다.

    개별 <layer>.json 이 있으면 "type" 을 읽어 넣고, 없거나 파싱 실패하면
    해당 레이어는 index 통계만 사용하고 type 은 None 으로 둔다.
    """
    index = load_odb_index(json_dir)

    bbox = index.get("bbox") or {}
    xmin = bbox.get("xmin")
    ymin = bbox.get("ymin")
    xmax = bbox.get("xmax")
    ymax = bbox.get("ymax")

    board_bbox_mm = [xmin, ymin, xmax, ymax]
    if None in board_bbox_mm:
        board_size_mm = [None, None]
    else:
        board_size_mm = [xmax - xmin, ymax - ymin]

    layers_out = []
    totals = {"lines": 0, "pads": 0, "arcs": 0, "surfaces": 0}
    total_features = 0

    for layer in index.get("layers", []):
        name = layer.get("name")
        lines = layer.get("lines", 0)
        pads = layer.get("pads", 0)
        arcs = layer.get("arcs", 0)
        surfaces = layer.get("surfaces", 0)
        total = layer.get("total", lines + pads + arcs + surfaces)

        layer_json = _load_layer_json(json_dir, layer.get("file"))
        layer_type = layer_json.get("type") if layer_json else None

        layers_out.append({
            "name": name,
            "type": layer_type,
            "total": total,
            "lines": lines,
            "pads": pads,
            "arcs": arcs,
            "surfaces": surfaces,
        })

        totals["lines"] += lines
        totals["pads"] += pads
        totals["arcs"] += arcs
        totals["surfaces"] += surfaces
        total_features += total

    return {
        "job": index.get("job"),
        "step": index.get("step"),
        "board_bbox_mm": board_bbox_mm,
        "board_size_mm": board_size_mm,
        "layer_count": len(layers_out),
        "total_features": total_features,
        "layers": layers_out,
        "feature_totals": totals,
    }


def symbol_summary(json_dir):
    """모든 레이어의 symbols 를 모아 드릴 후보/패드 형상/유니크 심볼 수를 요약한다.

    Round 심볼의 w 를 드릴 사이즈 후보로 수집한다. 그 외 형상(Oblong, Rectangle 등)은
    pad_shapes 에 타입별로 카운트한다.
    """
    index = load_odb_index(json_dir)

    drill_sizes = set()
    pad_shapes = {}
    unique_symbols = set()

    for layer in index.get("layers", []):
        layer_json = _load_layer_json(json_dir, layer.get("file"))
        if not layer_json:
            continue
        symbols = layer_json.get("symbols") or {}
        for sym_name, sym in symbols.items():
            unique_symbols.add(sym_name)
            sym_type = sym.get("type")
            if sym_type == "Round":
                w = sym.get("w")
                if w is not None:
                    drill_sizes.add(w)
            else:
                pad_shapes[sym_type] = pad_shapes.get(sym_type, 0) + 1

    return {
        "drill_sizes_mm": sorted(drill_sizes),
        "pad_shapes": pad_shapes,
        "unique_symbols": len(unique_symbols),
    }


def inspect_odb(json_dir):
    """구조 요약(summarize_odb)과 심볼 요약(symbol_summary)을 통합해 반환한다."""
    return {
        "structure": summarize_odb(json_dir),
        "symbols": symbol_summary(json_dir),
    }
