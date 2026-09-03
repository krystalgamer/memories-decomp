#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

import build_baseline
from workspace import WorkspaceError, require_workspace_root, resolve_within


CACHE_PATH = "tmp/project-build/incremental-cache.json"
TARGET_PATH = "game/SLUS_014.11"
OUTPUT_ELF = "tmp/project-build/SLUS_014.11.elf"
OUTPUT_MAP = "tmp/project-build/SLUS_014.11.map"
OUTPUT_EXE = "tmp/project-build/SLUS_014.11"
INCLUDE_PATTERN = re.compile(
    r'^\s*#\s*include\s*"([^"]+)"',
    re.MULTILINE,
)


class IncrementalBuildError(RuntimeError):
    pass


@dataclass(frozen=True)
class Component:
    kind: str
    source: str
    object_name: str
    profile: str | None = None


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def digest_value(value: Any) -> str:
    encoded = json.dumps(
        value,
        sort_keys=True,
        separators=(",", ":"),
    ).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()


def include_digest(root: Path, source: Path) -> str:
    visited: set[Path] = set()
    entries: list[tuple[str, str]] = []

    def visit(path: Path) -> None:
        resolved = path.resolve()
        try:
            relative = resolved.relative_to(root)
        except ValueError as error:
            raise IncrementalBuildError(
                f"include escapes repository: {path}"
            ) from error
        if resolved in visited:
            return
        visited.add(resolved)
        if not resolved.is_file():
            raise IncrementalBuildError(f"missing source/include: {relative}")
        entries.append((str(relative), sha256(resolved)))
        text = resolved.read_text(encoding="utf-8")
        for include in INCLUDE_PATTERN.findall(text):
            visit(resolved.parent / include)

    visit(source)
    return digest_value(entries)


def tree_digest(directory: Path) -> str:
    entries = [
        (str(path.relative_to(directory)), sha256(path))
        for path in sorted(directory.rglob("*"))
        if path.is_file()
    ]
    return digest_value(entries)


def load_components(root: Path) -> list[Component]:
    components = [
        Component("asm", "tmp/splat/asm/header.s", "header.o"),
        Component(
            "asm",
            "tmp/splat/asm/data/initial_data.data.s",
            "initial_data.o",
        ),
    ]
    seen = {"header.o", "initial_data.o"}
    for index, segment in enumerate(build_baseline.load_text_segments(root)):
        if not isinstance(segment, dict):
            raise IncrementalBuildError(f"text segment {index} is not an object")
        kind = segment.get("kind")
        source = segment.get("source")
        object_name = segment.get("object")
        profile = segment.get("profile")
        if (
            kind not in {"asm", "c"}
            or not isinstance(source, str)
            or not isinstance(object_name, str)
            or not object_name.endswith(".o")
            or "/" in object_name
            or object_name in seen
            or (kind == "c" and not isinstance(profile, str))
            or (kind == "asm" and profile is not None)
        ):
            raise IncrementalBuildError(f"invalid text segment {index}")
        seen.add(object_name)
        components.append(Component(kind, source, object_name, profile))
    for component in (
        Component(
            "asm",
            "tmp/splat/asm/data/initialized_data.data.s",
            "initialized_data.o",
        ),
        Component("binary", "tmp/splat/assets/bss_image.bin", "bss_image.o"),
        Component(
            "binary",
            "tmp/splat/assets/reserved_zero.bin",
            "reserved_zero.o",
        ),
        Component(
            "binary",
            "tmp/splat/assets/overlays/overlay_slots.bin",
            "overlay_slots.o",
        ),
        Component("binary", "tmp/splat/assets/tail_data.bin", "tail_data.o"),
    ):
        if component.object_name in seen:
            raise IncrementalBuildError(
                f"duplicate object name: {component.object_name}"
            )
        seen.add(component.object_name)
        components.append(component)
    return components


def load_cache(root: Path) -> dict[str, str]:
    path = resolve_within(root, CACHE_PATH)
    if not path.is_file():
        return {}
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError):
        return {}
    if not isinstance(value, dict):
        return {}
    objects = value.get("objects")
    if value.get("schema") != 1 or not isinstance(objects, dict):
        return {}
    if not all(
        isinstance(name, str) and isinstance(signature, str)
        for name, signature in objects.items()
    ):
        return {}
    return objects


def write_cache(root: Path, signatures: dict[str, str]) -> None:
    path = resolve_within(root, CACHE_PATH)
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(".json.tmp")
    temporary.write_text(
        json.dumps(
            {"schema": 1, "objects": signatures},
            indent=2,
            sort_keys=True,
        )
        + "\n",
        encoding="utf-8",
    )
    temporary.replace(path)


def dependency_context(
    root: Path,
    profiles: dict[str, dict[str, object]],
) -> dict[str, Any]:
    assembler = build_baseline.tool(root, "as")
    objcopy = build_baseline.tool(root, "objcopy")
    maspsx = resolve_within(
        root,
        "tools/vendor/maspsx/maspsx.py",
        must_exist=True,
    )
    include_directory = resolve_within(root, "tmp/splat/include", must_exist=True)
    profile_context: dict[str, Any] = {}
    for name, profile in profiles.items():
        compiler = resolve_within(
            root,
            str(profile["compiler"]),
            must_exist=True,
        )
        filter_value = profile.get("assembly_filter")
        filter_hash = None
        if isinstance(filter_value, str):
            filter_hash = sha256(
                resolve_within(root, filter_value, must_exist=True)
            )
        profile_context[name] = {
            "configuration": profile,
            "compiler_sha256": sha256(compiler),
            "assembly_filter_sha256": filter_hash,
        }
    return {
        "build_baseline_sha256": sha256(Path(build_baseline.__file__)),
        "build_incremental_sha256": sha256(Path(__file__)),
        "assembler_sha256": sha256(assembler),
        "objcopy_sha256": sha256(objcopy),
        "maspsx_sha256": sha256(maspsx),
        "splat_include_sha256": tree_digest(include_directory),
        "profiles": profile_context,
    }


def component_signature(
    root: Path,
    component: Component,
    context: dict[str, Any],
) -> str:
    source = resolve_within(root, component.source, must_exist=True)
    value: dict[str, Any] = {
        "kind": component.kind,
        "source": component.source,
        "source_sha256": sha256(source),
        "object": component.object_name,
        "build_baseline_sha256": context["build_baseline_sha256"],
        "build_incremental_sha256": context["build_incremental_sha256"],
    }
    if component.kind == "asm":
        value.update(
            {
                "assembler_sha256": context["assembler_sha256"],
                "splat_include_sha256": context["splat_include_sha256"],
            }
        )
    elif component.kind == "binary":
        value["objcopy_sha256"] = context["objcopy_sha256"]
    else:
        if component.profile not in context["profiles"]:
            raise IncrementalBuildError(
                f"unknown compiler profile: {component.profile}"
            )
        value.update(
            {
                "source_and_includes_sha256": include_digest(root, source),
                "assembler_sha256": context["assembler_sha256"],
                "maspsx_sha256": context["maspsx_sha256"],
                "profile": component.profile,
                "profile_context": context["profiles"][component.profile],
            }
        )
    return digest_value(value)


def object_path(root: Path, object_name: str) -> Path:
    return resolve_within(
        root,
        f"{build_baseline.OBJECT_DIRECTORY}/{object_name}",
    )


def build_component(
    root: Path,
    component: Component,
    assembler: Path,
    objcopy: Path,
    profiles: dict[str, dict[str, object]],
) -> Path:
    final = object_path(root, component.object_name)
    temporary_name = f"{component.object_name}.incremental.tmp"
    temporary = object_path(root, temporary_name)
    temporary.unlink(missing_ok=True)
    try:
        if component.kind == "asm":
            built = build_baseline.assemble(
                root,
                assembler,
                component.source,
                str(temporary.relative_to(root)),
            )
        elif component.kind == "binary":
            built = build_baseline.binary_object(
                root,
                objcopy,
                component.source,
                str(temporary.relative_to(root)),
            )
        else:
            segment = {
                "source": component.source,
                "object": temporary_name,
                "profile": component.profile,
            }
            built = build_baseline.compile_c(
                root,
                assembler,
                segment,
                profiles,
            )
        if built != temporary or not temporary.is_file():
            raise IncrementalBuildError(
                f"builder returned an unexpected object for {component.object_name}"
            )
        temporary.replace(final)
    finally:
        temporary.unlink(missing_ok=True)
    return final


def link(
    root: Path,
    objects: list[Path],
) -> Path:
    linker = build_baseline.tool(root, "ld")
    objcopy = build_baseline.tool(root, "objcopy")
    linker_script = resolve_within(
        root,
        "linker/slus_01411.ld",
        must_exist=True,
    )
    output_elf = resolve_within(root, OUTPUT_ELF)
    output_map = resolve_within(root, OUTPUT_MAP)
    output_exe = resolve_within(root, OUTPUT_EXE)
    temporary_elf = output_elf.with_suffix(".elf.incremental.tmp")
    temporary_map = output_map.with_suffix(".map.incremental.tmp")
    temporary_exe = output_exe.with_suffix(".incremental.tmp")
    for path in (temporary_elf, temporary_map, temporary_exe):
        path.unlink(missing_ok=True)
    try:
        build_baseline.run(
            root,
            [
                str(linker),
                "-EL",
                "-G0",
                *build_baseline.linker_compatibility_flags(),
                "-T",
                str(linker_script),
                "-Map",
                str(temporary_map),
                "-o",
                str(temporary_elf),
                *[str(path) for path in objects],
            ],
        )
        build_baseline.run(
            root,
            [
                str(objcopy),
                "-O",
                "binary",
                str(temporary_elf),
                str(temporary_exe),
            ],
        )
        expected_size = 0x1D0800
        if temporary_exe.stat().st_size != expected_size:
            raise IncrementalBuildError(
                f"rebuilt executable has size {temporary_exe.stat().st_size:#x}, "
                f"expected {expected_size:#x}"
            )
        temporary_elf.replace(output_elf)
        temporary_map.replace(output_map)
        temporary_exe.replace(output_exe)
    finally:
        for path in (temporary_elf, temporary_map, temporary_exe):
            path.unlink(missing_ok=True)
    return output_exe


def seed_existing(
    root: Path,
    components: list[Component],
    signatures: dict[str, str],
) -> None:
    missing = [
        component.object_name
        for component in components
        if not object_path(root, component.object_name).is_file()
    ]
    if missing:
        raise IncrementalBuildError(
            f"cannot seed cache; {len(missing)} objects are missing"
        )
    output = resolve_within(root, OUTPUT_EXE, must_exist=True)
    target = resolve_within(root, TARGET_PATH, must_exist=True)
    if sha256(output) != sha256(target):
        raise IncrementalBuildError(
            "cannot seed cache from a nonmatching executable"
        )
    write_cache(root, signatures)
    print(f"incremental cache seeded: {len(components)} objects")


def build_incrementally(root: Path, *, seed: bool) -> Path | None:
    profiles = build_baseline.load_compiler_profiles(root)
    components = load_components(root)
    context = dependency_context(root, profiles)
    signatures = {
        component.object_name: component_signature(root, component, context)
        for component in components
    }
    if seed:
        seed_existing(root, components, signatures)
        return None

    cache = load_cache(root)
    active_names = set(signatures)
    cache = {
        name: signature
        for name, signature in cache.items()
        if name in active_names
    }
    assembler = build_baseline.tool(root, "as")
    objcopy = build_baseline.tool(root, "objcopy")
    objects: list[Path] = []
    rebuilt = 0
    reused = 0
    for component in components:
        output = object_path(root, component.object_name)
        if (
            output.is_file()
            and output.stat().st_size > 0
            and cache.get(component.object_name)
            == signatures[component.object_name]
        ):
            reused += 1
        else:
            output = build_component(
                root,
                component,
                assembler,
                objcopy,
                profiles,
            )
            cache[component.object_name] = signatures[component.object_name]
            write_cache(root, cache)
            rebuilt += 1
        objects.append(output)

    output = link(root, objects)
    write_cache(root, signatures)
    print(
        f"incremental build: rebuilt={rebuilt} reused={reused} "
        f"output={output.relative_to(root)}"
    )
    return output


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Rebuild only changed PSX objects, then relink the full executable."
    )
    parser.add_argument(
        "--seed-existing",
        action="store_true",
        help="trust an already matched clean build and record its object signatures",
    )
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        build_incrementally(root, seed=args.seed_existing)
    except (
        IncrementalBuildError,
        build_baseline.BuildError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
