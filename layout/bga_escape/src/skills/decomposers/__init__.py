"""Decomposer skills — import all to trigger @register_skill."""
try:
    from src.skills.decomposers import ring_decomposer  # noqa: F401
except ImportError:
    pass
from src.skills.decomposers import quadtree_decomposer  # noqa: F401
from src.skills.decomposers import quadrant_partition  # noqa: F401
try:
    from src.skills.decomposers import priority_net_decomposer  # noqa: F401
except ImportError:
    pass
try:
    from src.skills.decomposers import bbox_overlap_decomposer  # noqa: F401
except ImportError:
    pass
try:
    from src.skills.decomposers import sink_proximity_decomposer  # noqa: F401
except ImportError:
    pass
try:
    from src.skills.decomposers import hierarchical_decomposer  # noqa: F401
except ImportError:
    pass
try:
    from src.skills.decomposers import balanced_quadrant_partition  # noqa: F401
except ImportError:
    pass
try:
    from src.skills.decomposers import multi_pin_phase  # noqa: F401
except ImportError:
    pass
