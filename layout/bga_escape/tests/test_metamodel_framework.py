# 메타모델 프레임워크(base/registry/feature_spec)의 계약을 검증하는 테스트.
import pytest

from bga_router.metamodel import (
    Metamodel,
    register_metamodel,
    get_metamodel,
    list_metamodels,
    FeatureVectorSpec,
)


class DummyModel(Metamodel):
    name = "dummy"
    version = "1.0.0"
    input_features = ["a", "b"]
    output_keys = ["y"]
    kind = "analytic"

    def predict(self, features: dict) -> dict:
        y = features.get("a", 0) + features.get("b", 0)
        return {
            "y": y,
            "confidence": 0.9,
            "provenance": "DummyModel v1.0.0",
            "flags": [],
        }


def test_register_and_get():
    model = DummyModel()
    returned = register_metamodel(model)
    assert returned is model
    fetched = get_metamodel("dummy")
    assert fetched is model
    result = fetched.predict({"a": 2, "b": 3})
    assert result["y"] == 5
    assert 0.0 <= result["confidence"] <= 1.0
    assert isinstance(result["provenance"], str)
    assert isinstance(result["flags"], list)


def test_register_overwrites_same_name():
    m1 = DummyModel()
    m2 = DummyModel()
    register_metamodel(m1)
    register_metamodel(m2)
    assert get_metamodel("dummy") is m2


def test_list_metamodels_contains_describe():
    register_metamodel(DummyModel())
    entries = list_metamodels()
    assert any(e["name"] == "dummy" for e in entries)
    entry = next(e for e in entries if e["name"] == "dummy")
    assert entry["kind"] == "analytic"


def test_get_unknown_raises_keyerror_with_known_list():
    register_metamodel(DummyModel())
    with pytest.raises(KeyError) as exc:
        get_metamodel("nope_not_here")
    msg = str(exc.value)
    assert "nope_not_here" in msg
    assert "dummy" in msg  # 알려진 목록 포함


def test_describe_keys():
    d = DummyModel().describe()
    assert set(d.keys()) == {
        "name",
        "version",
        "input_features",
        "output_keys",
        "kind",
    }
    assert d["name"] == "dummy"
    assert d["version"] == "1.0.0"
    assert d["input_features"] == ["a", "b"]
    assert d["output_keys"] == ["y"]
    assert d["kind"] == "analytic"


def test_feature_spec_normal_mapping():
    spec = FeatureVectorSpec(mapping={"a": "pitch", "b": "width"})
    features, flags = spec.build({"pitch": 0.4, "width": 0.1})
    assert features == {"a": 0.4, "b": 0.1}
    assert flags == []


def test_feature_spec_uses_default():
    spec = FeatureVectorSpec(
        mapping={"a": "pitch", "b": "width"},
        defaults={"b": 0.05},
    )
    features, flags = spec.build({"pitch": 0.4})
    assert features == {"a": 0.4, "b": 0.05}
    assert flags == []


def test_feature_spec_missing_required_raises():
    spec = FeatureVectorSpec(mapping={"a": "pitch", "b": "width"})
    with pytest.raises(ValueError) as exc:
        spec.build({"pitch": 0.4})
    msg = str(exc.value)
    assert "b" in msg
    assert "width" in msg


def test_feature_spec_out_of_range_flags():
    spec = FeatureVectorSpec(
        mapping={"a": "pitch", "b": "width"},
        ranges={"a": (0.0, 1.0), "b": (0.0, 1.0)},
    )
    features, flags = spec.build({"pitch": 5.0, "width": 0.1})
    assert features == {"a": 5.0, "b": 0.1}  # 추론은 진행
    assert "a_out_of_range" in flags
    assert "b_out_of_range" not in flags


def test_feature_spec_default_out_of_range_flags():
    spec = FeatureVectorSpec(
        mapping={"a": "pitch"},
        ranges={"a": (0.0, 1.0)},
        defaults={"a": 9.9},
    )
    features, flags = spec.build({})
    assert features == {"a": 9.9}
    assert "a_out_of_range" in flags
