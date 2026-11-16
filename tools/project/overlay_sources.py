"""Resolve overlay C units from manifests and their tracked Splat layouts.

Like the existing overlay wiring check, this reads the inline subsegment form
used by the tracked layouts without requiring Splat, PyYAML or retail inputs.
"""

from __future__ import annotations

import json
from pathlib import Path, PurePosixPath
import re
from typing import Any

from compile_defines import (
    DefineError,
    combine_defines,
    region_defines,
    region_directory,
    validate_defines,
)
from workspace import resolve_within


class OverlaySourceError(RuntimeError):
    pass


C_SUBSEGMENT = re.compile(
    r"^\s*-\s*\[[^,\]\n]+,\s*"
    r"(c|\.rodata|\.data|\.sdata|\.sbss|\.bss)\s*,\s*"
    r"([^\s,\]]+)\s*\]",
    re.MULTILINE,
)


def load_manifest(
    root: Path,
    path: Path,
    field: str,
    profiles: set[str],
    defines: dict[str, tuple[str, ...]] | None = None,
) -> dict[str, str]:
    """Map each unit to its profile; fill defines with its entry defines."""
    path = resolve_within(root, path.relative_to(root))
    if not path.exists():
        return {}
    label = path.relative_to(root).as_posix()
    manifest = json.loads(path.read_text(encoding="utf-8"))
    if (
        not isinstance(manifest, dict)
        or manifest.get("schema") != 1
        or not isinstance(manifest.get(field), list)
    ):
        raise OverlaySourceError(f"{label}: expected schema 1 and a {field} list")
    result: dict[str, str] = {}
    for entry in manifest[field]:
        if not isinstance(entry, dict):
            raise OverlaySourceError(f"{label}: entries must be objects")
        source = entry.get("source")
        profile = entry.get("profile")
        if (
            not isinstance(source, str)
            or not source.startswith("src/overlays/")
            or not source.endswith(".c")
            or ".." in PurePosixPath(source).parts
            or str(PurePosixPath(source)) != source
        ):
            raise OverlaySourceError(
                f"{label}: source must be a canonical C path under src/overlays/"
            )
        resolved = resolve_within(root, source, must_exist=True)
        if not resolved.is_file() or resolved.relative_to(root).as_posix() != source:
            raise OverlaySourceError(f"{label}: {source} is not a regular source path")
        if not isinstance(profile, str) or profile not in profiles:
            raise OverlaySourceError(f"{label}: {source} has unknown profile {profile}")
        try:
            entry_defines = validate_defines(entry.get("defines"), f"{label}: {source}")
        except DefineError as error:
            raise OverlaySourceError(str(error)) from error
        if defines is not None:
            if source in defines and defines[source] != entry_defines:
                raise OverlaySourceError(
                    f"{label}: {source} has conflicting define sets"
                )
            defines[source] = entry_defines
        if source in result:
            if field == "units":
                raise OverlaySourceError(f"{label}: duplicate data unit {source}")
            if result[source] != profile:
                raise OverlaySourceError(f"{label}: {source} has conflicting profiles")
        result[source] = profile
    return result


def layout_sources(root: Path, layout: Path) -> list[tuple[str, str]]:
    sources = [
        (kind, f"src/{name}.c")
        for kind, name in C_SUBSEGMENT.findall(layout.read_text(encoding="utf-8"))
    ]
    seen: set[tuple[str, str]] = set()
    for kind, source in sources:
        if (kind, source) in seen:
            raise OverlaySourceError(
                f"{layout.relative_to(root)}: duplicate {kind} subsegment for {source}"
            )
        seen.add((kind, source))
    return sources


def c_segments(root: Path, layout: Path) -> list[dict[str, Any]]:
    layout = resolve_within(root, layout.relative_to(root), must_exist=True)
    configuration = json.loads(
        resolve_within(
            root, "config/slus_01411/compiler_profiles.json", must_exist=True
        ).read_text(encoding="utf-8")
    )
    if (
        not isinstance(configuration, dict)
        or configuration.get("schema") != 1
        or not isinstance(configuration.get("profiles"), dict)
        or not configuration["profiles"]
    ):
        raise OverlaySourceError("invalid compiler profile manifest")
    profiles = set(configuration["profiles"])
    entry_defines: dict[str, tuple[str, ...]] = {}
    text = load_manifest(
        root,
        layout.with_name(f"{layout.stem}_matching_c.json"),
        "functions",
        profiles,
        entry_defines,
    )
    data = load_manifest(
        root,
        layout.with_name(f"{layout.stem}_data_c.json"),
        "units",
        profiles,
        entry_defines,
    )
    try:
        regional = region_defines(root, region_directory(layout.relative_to(root)))
    except DefineError as error:
        raise OverlaySourceError(str(error)) from error
    overlap = sorted(set(text) & set(data))
    if overlap:
        raise OverlaySourceError(
            f"{layout.name}: text units must not also be in data_c.json: "
            + ", ".join(overlap)
        )

    wired = layout_sources(root, layout)
    text_wired = {source for kind, source in wired if kind == "c"}
    data_wired = {source for kind, source in wired if kind != "c"} - text_wired
    for label, declared, mapped in (
        ("matching_c.json", set(text), text_wired),
        ("data_c.json", set(data), data_wired),
    ):
        missing = sorted(mapped - declared)
        unused = sorted(declared - mapped)
        if missing or unused:
            raise OverlaySourceError(
                f"{layout.name}: {label} wiring mismatch; "
                f"missing units: {', '.join(missing) or 'none'}; "
                f"unmapped units: {', '.join(unused) or 'none'}"
            )

    combined = {**text, **data}
    # One object can provide several sections; shared overworld sources are
    # compiled separately by each module, never reused across module links.
    ordered = dict.fromkeys(source for _kind, source in wired)
    segments = []
    for source in ordered:
        segment: dict[str, Any] = {
            "kind": "data" if source in data else "text",
            "source": source,
            "profile": combined[source],
            "object": str(PurePosixPath(source).with_suffix(".o")),
        }
        try:
            defines = combine_defines(
                regional, entry_defines.get(source, ()), f"{layout.name}: {source}"
            )
        except DefineError as error:
            raise OverlaySourceError(str(error)) from error
        if defines:
            segment["defines"] = list(defines)
        segments.append(segment)
    return segments
