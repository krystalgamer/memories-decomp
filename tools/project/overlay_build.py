#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path, PurePosixPath
from typing import Any

from build_baseline import (
    BuildError,
    assembler_compatibility_flags,
    compile_c,
    linker_compatibility_flags,
    load_compiler_profiles,
    normalize_text_alignment,
    run,
    tool,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within


class OverlayBuildError(RuntimeError):
    pass


def load_modules(root: Path) -> list[dict[str, Any]]:
    path = resolve_within(
        root, "config/slus_01411/overlays.json", must_exist=True
    )
    with path.open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)
    modules = manifest.get("modules")
    if manifest.get("schema") != 1 or not isinstance(modules, list):
        raise OverlayBuildError(f"invalid overlay manifest: {path.relative_to(root)}")
    build_modules = [module for module in modules if module.get("layout") is not None]
    if not build_modules:
        raise OverlayBuildError("overlay manifest has no build-ready modules")
    return build_modules


def module_field(module: dict[str, Any], field: str) -> str:
    value = module.get(field)
    if not isinstance(value, str) or not value:
        raise OverlayBuildError(f"overlay field {field} must be a non-empty string")
    return value


def module_paths(
    root: Path, module: dict[str, Any]
) -> tuple[str, Path, Path, Path, Path, Path]:
    name = module_field(module, "name")
    module_root = resolve_within(root, f"tmp/overlays/{name}")
    target = resolve_within(root, module_field(module, "output"), must_exist=True)
    config = resolve_within(root, module_field(module, "layout"), must_exist=True)
    built_elf = resolve_within(root, f"tmp/overlays/{name}/build/{name}.elf")
    built_binary = resolve_within(root, f"tmp/overlays/{name}/build/{name}.bin")
    return name, module_root, target, config, built_elf, built_binary


def matching_c_segments(root: Path, module: dict[str, Any]) -> list[dict[str, str]]:
    name = module_field(module, "name")
    relative_path = f"config/slus_01411/overlays/{name}_matching_c.json"
    path = root / relative_path
    if not path.is_file():
        return []
    with path.open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)
    functions = manifest.get("functions")
    if manifest.get("schema") != 1 or not isinstance(functions, list):
        raise OverlayBuildError(f"invalid overlay C manifest: {relative_path}")

    segments: list[dict[str, str]] = []
    sources: set[str] = set()
    for entry in functions:
        if not isinstance(entry, dict):
            raise OverlayBuildError(f"{relative_path}: entries must be objects")
        source = entry.get("source")
        profile = entry.get("profile")
        if (
            not isinstance(source, str)
            or not source.startswith("src/overlays/")
            or not source.endswith(".c")
            or ".." in PurePosixPath(source).parts
        ):
            raise OverlayBuildError(
                f"{relative_path}: source must be a C file under src/overlays/"
            )
        if not isinstance(profile, str) or not profile:
            raise OverlayBuildError(f"{relative_path}: {source} has no profile")
        if source in sources:
            continue
        sources.add(source)
        segments.append(
            {
                "source": source,
                "profile": profile,
                "object": str(PurePosixPath(source).with_suffix(".o")),
            }
        )
    return segments


def compile_sources(
    root: Path, module_root: Path, segments: list[dict[str, str]]
) -> list[Path]:
    if not segments:
        return []
    assembler = tool(root, "as")
    profiles = load_compiler_profiles(root)
    build_root = module_root.relative_to(root) / "build"
    objects: list[Path] = []
    for segment in segments:
        objects.append(
            compile_c(
                root,
                assembler,
                segment,
                profiles,
                object_directory=str(build_root),
                asm_directory=str(build_root / "asm"),
            )
        )
    return objects


def assemble_sources(root: Path, module_root: Path) -> list[Path]:
    assembler = tool(root, "as")
    include_directory = resolve_within(
        root, module_root.relative_to(root) / "include", must_exist=True
    )
    asm_directory = resolve_within(
        root, module_root.relative_to(root) / "asm", must_exist=True
    )
    sources = sorted(asm_directory.rglob("*.s"))
    if not sources:
        raise OverlayBuildError(
            f"no generated assembly below {asm_directory.relative_to(root)}"
        )

    objects: list[Path] = []
    for source in sources:
        source_relative = source.relative_to(root)
        output = resolve_within(
            root,
            module_root.relative_to(root)
            / "build"
            / source_relative.with_suffix(".o"),
        )
        output.parent.mkdir(parents=True, exist_ok=True)
        run(
            root,
            [
                str(assembler),
                "-EL",
                "-march=r3000",
                "-mabi=32",
                "-G0",
                *assembler_compatibility_flags(),
                "-I",
                str(include_directory),
                "-o",
                str(output),
                str(source),
            ],
        )
        normalize_text_alignment(root, output)
        objects.append(output)
    return objects


def build_module(root: Path, module: dict[str, Any]) -> None:
    name, module_root, _target, config, built_elf, built_binary = module_paths(
        root, module
    )
    splat = resolve_within(
        root, "tools/environments/python/bin/splat", must_exist=True
    )
    run(root, [str(splat), "split", str(config)])
    compile_sources(root, module_root, matching_c_segments(root, module))
    assemble_sources(root, module_root)

    linker = tool(root, "ld")
    linker_script = resolve_within(
        root, module_root.relative_to(root) / f"{name}.ld", must_exist=True
    )
    undefined_functions = resolve_within(
        root,
        module_root.relative_to(root) / "undefined_funcs_auto.txt",
        must_exist=True,
    )
    undefined_symbols = resolve_within(
        root,
        module_root.relative_to(root) / "undefined_syms_auto.txt",
        must_exist=True,
    )
    built_elf.parent.mkdir(parents=True, exist_ok=True)
    run(
        root,
        [
            str(linker),
            "-EL",
            *linker_compatibility_flags(),
            "-T",
            str(linker_script),
            "-T",
            str(undefined_functions),
            "-T",
            str(undefined_symbols),
            "-o",
            str(built_elf),
        ],
    )

    objcopy = tool(root, "objcopy")
    run(root, [str(objcopy), "-O", "binary", str(built_elf), str(built_binary)])
    print(f"overlay build: {name} -> {built_binary.relative_to(root)}")


def verify_module(root: Path, module: dict[str, Any]) -> None:
    name, _module_root, target, _config, _built_elf, built_binary = module_paths(
        root, module
    )
    if not built_binary.is_file():
        raise OverlayBuildError(
            f"{name}: missing build output {built_binary.relative_to(root)}; "
            "run make build-overlays"
        )
    target_bytes = target.read_bytes()
    built_bytes = built_binary.read_bytes()
    if built_bytes != target_bytes:
        raise OverlayBuildError(f"{name}: rebuilt module does not match its input")

    actual_hash = hashlib.sha256(built_bytes).hexdigest()
    expected_hash = module_field(module, "sha256")
    if actual_hash != expected_hash:
        raise OverlayBuildError(
            f"{name}: rebuilt SHA-256 is {actual_hash}, expected {expected_hash}"
        )
    print(f"overlay match: {name} {actual_hash}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build and verify configured runtime overlay modules."
    )
    parser.add_argument("command", choices=("build", "verify"))
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        modules = load_modules(root)
        for module in modules:
            if args.command == "build":
                build_module(root, module)
            else:
                verify_module(root, module)
    except (
        OverlayBuildError,
        BuildError,
        WorkspaceError,
        OSError,
        TypeError,
        ValueError,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
