"""Resolve overlay C units from manifests and their tracked Splat layouts.

Like the existing overlay wiring check, this reads the inline subsegment form
used by the tracked layouts without requiring Splat, PyYAML or retail inputs.
"""

from __future__ import annotations

import csv
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

# Every MODEL.MRG image starts with one word, its position in the developers'
# build list. All images build it from one shared source; the value comes from
# a per-region table instead of a per-image data_c.json.
MODEL_MODULE_HEADER = "src/overlays/model_variant/module_header.c"
MODEL_MODULE_PROFILE = "gcc_2_8_1_g0"
MODEL_MODULE_WORDS = "overlays/model_module_words.csv"
MODEL_MODULE_WORDS_HEADER = ["stem", "word"]
MODEL_MODULE_WORD = re.compile(r"^[1-9][0-9]*$")
MODEL_STEM = re.compile(r"^model_[a-z0-9_]+$")
SEGMENT_NAME = re.compile(r"^(\s*)-\s*name:\s*(\S+)\s*$")
SEGMENT_VRAM = re.compile(r"^\s*vram:\s*(0[xX][0-9A-Fa-f]+|[0-9]+)\s*$")

_MODEL_WORDS_CACHE: dict[tuple[Path, int, int], dict[str, int]] = {}


def model_module_words(root: Path, region: str) -> dict[str, int]:
    """Read config/<region>/overlays/model_module_words.csv (stem -> word)."""
    relative = f"{region}/{MODEL_MODULE_WORDS}"
    path = resolve_within(root, relative)
    if not path.is_file():
        return {}
    status = path.stat()
    key = (path, status.st_mtime_ns, status.st_size)
    if key not in _MODEL_WORDS_CACHE:
        with path.open(newline="", encoding="utf-8") as handle:
            rows = list(csv.reader(handle))
        if not rows or rows[0] != MODEL_MODULE_WORDS_HEADER:
            raise OverlaySourceError(
                f"{relative}: header must be {','.join(MODEL_MODULE_WORDS_HEADER)}"
            )
        words: dict[str, int] = {}
        for number, row in enumerate(rows[1:], 2):
            if (
                len(row) != 2
                or MODEL_STEM.match(row[0]) is None
                or MODEL_MODULE_WORD.match(row[1]) is None
            ):
                raise OverlaySourceError(f"{relative}:{number}: invalid row {row!r}")
            if row[0] in words:
                raise OverlaySourceError(
                    f"{relative}:{number}: duplicate stem {row[0]}"
                )
            words[row[0]] = int(row[1])
        if list(words) != sorted(words):
            raise OverlaySourceError(f"{relative}: rows must be sorted by stem")
        _MODEL_WORDS_CACHE[key] = words
    return _MODEL_WORDS_CACHE[key]


def segment_vram(text: str, name: str) -> int | None:
    """The vram of the layout segment called name, in hex or decimal form."""
    lines = text.splitlines()
    for index, line in enumerate(lines):
        match = SEGMENT_NAME.match(line)
        if match is None or match.group(2) != name:
            continue
        indent = len(match.group(1))
        for following in lines[index + 1 :]:
            other = SEGMENT_NAME.match(following)
            if other is not None and len(other.group(1)) <= indent:
                break
            vram = SEGMENT_VRAM.match(following)
            if vram is not None:
                return int(vram.group(1), 0)
        return None
    return None


def model_module_defines(
    root: Path, layout: Path, text: str, wired: list[tuple[str, str]]
) -> tuple[str, ...] | None:
    """Defines for the shared MODEL module word, if the layout wires it."""
    region = region_directory(layout.relative_to(root))
    word = model_module_words(root, region).get(layout.stem)
    if not any(kind != "c" and source == MODEL_MODULE_HEADER for kind, source in wired):
        if word is not None:
            raise OverlaySourceError(
                f"{layout.name}: {region}/{MODEL_MODULE_WORDS} lists it, "
                f"but it does not wire {MODEL_MODULE_HEADER}"
            )
        return None
    if word is None:
        raise OverlaySourceError(
            f"{layout.name}: wires {MODEL_MODULE_HEADER} but has no row in "
            f"{region}/{MODEL_MODULE_WORDS}"
        )
    vram = segment_vram(text, "module_header")
    if vram is None:
        raise OverlaySourceError(
            f"{layout.name}: {MODEL_MODULE_HEADER} needs a module_header segment vram"
        )
    return (f"MODEL_MODULE_WORD={word}", f"MODEL_MODULE_WORD_LABEL=D_{vram:08X}")


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
    try:
        model_defines = model_module_defines(
            root, layout, layout.read_text(encoding="utf-8"), wired
        )
    except DefineError as error:
        raise OverlaySourceError(str(error)) from error
    if model_defines is not None:
        if MODEL_MODULE_HEADER in data or MODEL_MODULE_HEADER in text:
            raise OverlaySourceError(
                f"{layout.name}: {MODEL_MODULE_HEADER} comes from "
                f"{MODEL_MODULE_WORDS}; do not list it in a manifest"
            )
        if MODEL_MODULE_PROFILE not in profiles:
            raise OverlaySourceError(
                f"{layout.name}: unknown profile {MODEL_MODULE_PROFILE}"
            )
        data[MODEL_MODULE_HEADER] = MODEL_MODULE_PROFILE
        entry_defines[MODEL_MODULE_HEADER] = model_defines
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
