#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

import build_baseline
from workspace import WorkspaceError, require_workspace_root, resolve_within


# `make split` removes all of tmp/splat before every build, so cached objects
# and their signatures are kept outside it and copied back into the paths
# Splat's generated linker script names.
CACHE_DIRECTORY = "tmp/incremental"
CACHE_OBJECT_DIRECTORY = f"{CACHE_DIRECTORY}/obj"
CACHE_PATH = f"{CACHE_DIRECTORY}/cache.json"
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
        # Some vendored Psy-Q headers carry Shift-JIS comments, so decode
        # permissively purely to find nested includes; the recorded hash above
        # still covers the exact bytes.
        text = resolved.read_text(encoding="utf-8", errors="replace")
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
    components = [Component("asm", "tmp/splat/asm/header.s", "header.o")]
    seen = {"header.o"}
    # The leading data blob is split wherever a matching C object owns pre-text
    # read-only data, so track every piece of it in the same order as a clean
    # baseline build.
    for path in sorted((root / "tmp/splat/asm/data").glob("initial_data*.s")):
        object_name = f"{path.stem}.o"
        if object_name in seen:
            raise IncrementalBuildError(f"duplicate object name: {object_name}")
        seen.add(object_name)
        components.append(
            Component("asm", f"tmp/splat/asm/data/{path.name}", object_name)
        )
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
    trailing_components = [
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
    ]
    trailing_components.extend(
        Component(
            "binary",
            f"tmp/splat/assets/{asset}.bin",
            object_name,
        )
        for asset, object_name in build_baseline.load_overlay_assets(root)
    )
    trailing_components.append(
        Component("binary", "tmp/splat/assets/tail_data.bin", "tail_data.o")
    )
    for component in trailing_components:
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


def object_path(root: Path, component: Component) -> Path:
    """Object location Splat's generated linker script names for a source."""
    return resolve_within(root, build_baseline.splat_object(component.source))


def cached_object_path(root: Path, component: Component) -> Path:
    """Object location that survives the clean Splat performs on every build."""
    trimmed = component.source[: component.source.rindex(".")]
    return resolve_within(root, f"{CACHE_OBJECT_DIRECTORY}/{trimmed}.o")


def copy_object(source: Path, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    temporary = destination.with_name(f"{destination.name}.incremental.tmp")
    temporary.unlink(missing_ok=True)
    try:
        shutil.copyfile(source, temporary)
        temporary.replace(destination)
    finally:
        temporary.unlink(missing_ok=True)


def build_component(
    root: Path,
    component: Component,
    assembler: Path,
    objcopy: Path,
    profiles: dict[str, dict[str, object]],
) -> Path:
    final = object_path(root, component)
    final.parent.mkdir(parents=True, exist_ok=True)
    if component.kind == "c":
        # C objects are written straight to the path Splat's script names so a
        # rebuilt object is byte-identical to a clean baseline build. Removing
        # the previous object first keeps a failed compile from leaving stale
        # bytes behind a cache entry that was never updated.
        final.unlink(missing_ok=True)
        built = build_baseline.compile_c(
            root,
            assembler,
            {
                "source": component.source,
                "object": component.object_name,
                "profile": component.profile,
            },
            profiles,
            use_splat_object_paths=True,
        )
        if built != final or not final.is_file():
            raise IncrementalBuildError(
                f"builder returned an unexpected object for {component.object_name}"
            )
        return final

    temporary = final.with_name(f"{final.name}.incremental.tmp")
    temporary.unlink(missing_ok=True)
    try:
        if component.kind == "asm":
            built = build_baseline.assemble(
                root,
                assembler,
                component.source,
                str(temporary.relative_to(root)),
            )
        else:
            built = build_baseline.binary_object(
                root,
                objcopy,
                component.source,
                str(temporary.relative_to(root)),
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
    missing = [str(path.relative_to(root)) for path in objects if not path.is_file()]
    if missing:
        raise IncrementalBuildError(f"missing objects for link: {missing[0]}")
    linker_script = resolve_within(
        root,
        "tmp/splat/slus_01411.ld",
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
                *[
                    argument
                    for relative in (
                        "tmp/splat/undefined_funcs_auto.txt",
                        "tmp/splat/undefined_syms_auto.txt",
                        "config/slus_01411/c_symbols.ld",
                        "config/slus_01411/link_symbols.ld",
                    )
                    for argument in (
                        "-T",
                        str(resolve_within(root, relative, must_exist=True)),
                    )
                ],
                "-Map",
                str(temporary_map),
                "-o",
                str(temporary_elf),
                # No object arguments. Splat's script names every input file
                # itself, so the linker loads them from there; passing them
                # again loads each one twice and every symbol becomes
                # multiply defined.
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
        if not object_path(root, component).is_file()
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
    for component in components:
        copy_object(
            object_path(root, component),
            cached_object_path(root, component),
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
        output = object_path(root, component)
        cached = cached_object_path(root, component)
        if (
            cached.is_file()
            and cached.stat().st_size > 0
            and cache.get(component.object_name)
            == signatures[component.object_name]
        ):
            copy_object(cached, output)
            reused += 1
        else:
            output = build_component(
                root,
                component,
                assembler,
                objcopy,
                profiles,
            )
            copy_object(output, cached)
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
