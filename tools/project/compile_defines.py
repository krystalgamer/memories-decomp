"""Preprocessor defines a C unit is compiled with, taken from configuration.

A region's resident manifest, config/<region>/matching_c.json, may carry a
top-level "defines" list that applies to every C unit the region compiles,
resident and overlay alike. Any matching_c.json or data_c.json entry may carry
its own "defines" list, which is appended after the region's. compile_c passes
the effective list to the compiler as -D flags in that order, after the
profile's own flags.
"""

from __future__ import annotations

import json
import re
from pathlib import Path, PurePosixPath
from typing import Any

from workspace import resolve_within


DEFINE_PATTERN = re.compile(r"^[A-Z_][A-Z0-9_]*(=[^\s]+)?$")


class DefineError(ValueError):
    pass


def validate_defines(value: Any, label: str) -> tuple[str, ...]:
    """Return a validated define list; a missing list is empty."""
    if value is None:
        return ()
    if not isinstance(value, list):
        raise DefineError(f"{label}: defines must be a list")
    names: set[str] = set()
    for define in value:
        if not isinstance(define, str) or DEFINE_PATTERN.match(define) is None:
            raise DefineError(f"{label}: invalid define {define!r}")
        name = define.partition("=")[0]
        if name in names:
            raise DefineError(f"{label}: duplicate define {name}")
        names.add(name)
    return tuple(value)


def combine_defines(
    region: tuple[str, ...], entry: tuple[str, ...], label: str
) -> tuple[str, ...]:
    """Region defines followed by an entry's, each name given once."""
    return validate_defines([*region, *entry], label)


def define_flags(defines: tuple[str, ...] | list[str]) -> list[str]:
    return [f"-D{define}" for define in defines]


def region_directory(path: str | Path) -> str:
    """The config/<region> directory a tracked manifest or layout belongs to."""
    parts = PurePosixPath(Path(path).as_posix()).parts
    if "config" not in parts:
        raise DefineError(f"{path}: not inside a config/<region> directory")
    index = parts.index("config")
    if index + 1 >= len(parts) - 1:
        raise DefineError(f"{path}: not inside a config/<region> directory")
    return f"config/{parts[index + 1]}"


_REGION_CACHE: dict[tuple[Path, int, int], tuple[str, ...]] = {}


def region_defines(root: Path, region: str) -> tuple[str, ...]:
    """The top-level defines of config/<region>/matching_c.json, if any."""
    manifest = f"{region}/matching_c.json"
    path = resolve_within(root, manifest)
    if not path.is_file():
        return ()
    status = path.stat()
    key = (path, status.st_mtime_ns, status.st_size)
    if key not in _REGION_CACHE:
        configuration = json.loads(path.read_text(encoding="utf-8"))
        if not isinstance(configuration, dict):
            raise DefineError(f"{manifest}: expected an object")
        _REGION_CACHE[key] = validate_defines(
            configuration.get("defines"), manifest
        )
    return _REGION_CACHE[key]
