"""Object names of resident C translation units.

Splat names a C unit's object after its split.yaml segment, and the segment
name is the source path below src/ without its extension, so by default one
source builds one object. A resident matching_c.json entry may instead carry
an "object" key: the segment name its unit is placed and linked under. Two
units can then compile the same source, usually with different defines, into
distinct objects. Overlay manifests and data_c.json units do not take it.

An override must not name an existing source: Splat scans src/<object>.c for
the functions it defines, and a region that uses overrides sets
create_c_files: false so Splat does not write a stub file there either.
"""

from __future__ import annotations

import re
from typing import Any


OBJECT_PATTERN = re.compile(r"^[a-z0-9_]+(/[a-z0-9_]+)*$")


class ObjectNameError(ValueError):
    pass


def default_object(source: str) -> str:
    """Segment name Splat derives from a source path under src/."""
    if not source.startswith("src/") or not source.endswith(".c"):
        raise ObjectNameError(f"{source}: not a C source under src/")
    return source[len("src/") : -len(".c")]


def validate_object(value: Any, source: str, label: str) -> str | None:
    """Return a validated override; a missing key is None."""
    if value is None:
        return None
    if not isinstance(value, str) or OBJECT_PATTERN.match(value) is None:
        raise ObjectNameError(f"{label}: invalid object name {value!r}")
    if value == default_object(source):
        raise ObjectNameError(
            f"{label}: object {value} repeats the name its source already gives"
        )
    return value


def entry_object(entry: dict[str, Any], label: str) -> str:
    """Segment name a resident matching_c.json entry is built under."""
    source = entry.get("source")
    if not isinstance(source, str):
        raise ObjectNameError(f"{label}: entry has no source")
    override = validate_object(entry.get("object"), source, label)
    return override if override is not None else default_object(source)
