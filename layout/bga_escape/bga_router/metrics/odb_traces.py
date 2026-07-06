# ODB에 이미 설계된 net 배선을 재라우팅 없이 분석하는 지표 (odb_to_em_json 입력)
"""Analyze already-designed nets from ``odb_to_em_json`` output.

This module reads the per-net polygon geometry produced by the C++
``odb_to_em_json`` tool and derives rough routing metrics *without*
re-routing. It answers questions like "how many layers does this net
span?", "does it use a via?", and "roughly how long is it?" purely from
the exported fill polygons.

Input schema (measured / confirmed)::

    {
      "job": "P3_EUR_REV03", "units": "MM", "region": null,
      "stackup": {...},
      "layers": {
        "COMP": {"z_bottom":.., "z_top":.., "nets": {
            "net019": {"polygons": [
                {"type":"fill", "outer": [[x,y],...], "holes": [...]}
            ]}, ...
        }},
        "LAY2": {...}, ...
      }
    }

A single net name may appear on multiple layers (COMP + LAY2, ...).
``polygon["outer"]`` is a closed ring of mm coordinates ``[[x,y],...]``.

All functions here are pure except ``load_em_data`` (file I/O). Only the
stdlib + :mod:`math` are used — no numpy/scipy.
"""

from __future__ import annotations

import json
import math
from typing import Optional

from bga_router.metrics.path_geometry import is_power_ground_net

# net이 이 개수 이상의 폴리곤을 가지면 평면(plane) 후보로 본다.
_PLANE_POLYGON_THRESHOLD = 50


def load_em_data(path: str) -> dict:
    """Read an ``odb_to_em_json`` JSON file into a dict."""
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def _iter_net_polygons(em_data: dict, net_name: str):
    """Yield ``(layer_name, polygon)`` for every polygon of ``net_name``.

    Layers are iterated in sorted order for determinism.
    """
    layers = em_data.get("layers", {})
    for layer_name in sorted(layers):
        nets = layers[layer_name].get("nets", {})
        net = nets.get(net_name)
        if net is None:
            continue
        for poly in net.get("polygons", []):
            yield layer_name, poly


def net_layers(em_data: dict, net_name: str) -> list[str]:
    """Return the sorted list of layer names where ``net_name`` appears."""
    layers = em_data.get("layers", {})
    found = [
        layer_name
        for layer_name in layers
        if net_name in layers[layer_name].get("nets", {})
    ]
    return sorted(found)


def polygon_bbox(outer: list) -> tuple[float, float, float, float]:
    """Return ``(xmin, ymin, xmax, ymax)`` of a ring ``[[x,y],...]``."""
    xs = [p[0] for p in outer]
    ys = [p[1] for p in outer]
    return (min(xs), min(ys), max(xs), max(ys))


def polygon_perimeter_mm(outer: list) -> float:
    """Return the ring perimeter (sum of adjacent euclidean distances).

    The ring is treated as closed: the segment from the last point back
    to the first is included. If ``outer`` already repeats the first
    point at the end, that closing segment has length 0 and does not
    change the result.
    """
    n = len(outer)
    if n < 2:
        return 0.0
    total = 0.0
    for i in range(n):
        x0, y0 = outer[i]
        x1, y1 = outer[(i + 1) % n]
        total += math.hypot(x1 - x0, y1 - y0)
    return total


def net_bbox_mm(em_data: dict, net_name: str) -> tuple[float, float, float, float]:
    """Return the union bbox over all layers/polygons of ``net_name``.

    ``(xmin, ymin, xmax, ymax)``. Raises ``KeyError`` if the net has no
    polygons anywhere.
    """
    xmin = ymin = math.inf
    xmax = ymax = -math.inf
    found = False
    for _layer, poly in _iter_net_polygons(em_data, net_name):
        outer = poly.get("outer", [])
        if not outer:
            continue
        bxmin, bymin, bxmax, bymax = polygon_bbox(outer)
        xmin = min(xmin, bxmin)
        ymin = min(ymin, bymin)
        xmax = max(xmax, bxmax)
        ymax = max(ymax, bymax)
        found = True
    if not found:
        raise KeyError(f"net {net_name!r} has no polygons with geometry")
    return (xmin, ymin, xmax, ymax)


def analyze_net(em_data: dict, net_name: str) -> dict:
    """Derive rough routing metrics for one already-designed net.

    ``est_trace_length_mm`` is the bbox diagonal ``hypot(width, height)``.
    This is a *rough* indicator obtained without any routing: it only
    approximates the extent a multi-layer net spans, not its true routed
    copper length. Use it as a coarse relative metric, not an exact value.
    """
    layers = net_layers(em_data, net_name)
    layer_span = len(layers)

    polygon_count = 0
    total_perimeter = 0.0
    for _layer, poly in _iter_net_polygons(em_data, net_name):
        polygon_count += 1
        total_perimeter += polygon_perimeter_mm(poly.get("outer", []))

    try:
        xmin, ymin, xmax, ymax = net_bbox_mm(em_data, net_name)
    except KeyError:
        # Net is registered on a layer but has no polygon geometry
        # (empty ``polygons`` list, or polygons with empty ``outer``).
        # Report a degenerate zero bbox instead of crashing so callers
        # like :func:`summarize_traces` stay robust on such nets.
        xmin = ymin = xmax = ymax = 0.0
    width = xmax - xmin
    height = ymax - ymin

    return {
        "net": net_name,
        "layers": layers,
        "layer_span": layer_span,
        "has_via": layer_span >= 2,
        "polygon_count": polygon_count,
        "bbox_mm": [xmin, ymin, xmax, ymax],
        "extent_mm": [width, height],
        "total_perimeter_mm": total_perimeter,
        "est_trace_length_mm": math.hypot(width, height),
    }


def classify_net(em_data: dict, net_name: str) -> str:
    """Classify a net as ``power_ground`` / ``plane_candidate`` / ``signal``.

    Power/ground naming (via :func:`is_power_ground_net`) wins first. A
    net with a large polygon count is treated as a plane candidate.
    Everything else is a signal net.
    """
    if is_power_ground_net(net_name):
        return "power_ground"
    polygon_count = sum(1 for _ in _iter_net_polygons(em_data, net_name))
    if polygon_count >= _PLANE_POLYGON_THRESHOLD:
        return "plane_candidate"
    return "signal"


def summarize_traces(em_data: dict, *, max_nets: Optional[int] = None) -> dict:
    """Summarize every net in ``em_data`` (rough, no re-routing).

    ``max_nets`` caps how many nets get a full :func:`analyze_net` entry
    (nets are taken in sorted order); ``None`` means all. The class
    counts and multi-layer list still reflect *all* nets regardless of
    ``max_nets``, so the summary stays honest on large boards.
    """
    layers = em_data.get("layers", {})
    layer_names = sorted(layers)

    all_nets: set[str] = set()
    for layer_name in layer_names:
        all_nets.update(layers[layer_name].get("nets", {}))
    sorted_nets = sorted(all_nets)

    by_class = {"signal": 0, "power_ground": 0, "plane_candidate": 0}
    multi_layer_nets: list[str] = []
    for net_name in sorted_nets:
        by_class[classify_net(em_data, net_name)] += 1
        if len(net_layers(em_data, net_name)) >= 2:
            multi_layer_nets.append(net_name)

    selected = sorted_nets if max_nets is None else sorted_nets[:max_nets]
    nets = {net_name: analyze_net(em_data, net_name) for net_name in selected}

    return {
        "net_count": len(sorted_nets),
        "nets": nets,
        "by_class": by_class,
        "multi_layer_nets": multi_layer_nets,
        "layer_names": layer_names,
    }
