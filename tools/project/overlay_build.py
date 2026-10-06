#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import multiprocessing
import multiprocessing.connection
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path
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
from overlay_sources import OverlaySourceError, c_segments
from overlay_extract import OVERLAY_MANIFESTS
from workspace import (
    WorkspaceError,
    local_environment,
    require_workspace_root,
    resolve_within,
)


class OverlayBuildError(RuntimeError):
    pass


_IN_PROCESS_SPLAT: Any = None


def job_count(makeflags: str | None = None) -> int:
    """Honor make's -jN (as forwarded in MAKEFLAGS); default to sequential."""
    flags = os.environ.get("MAKEFLAGS", "") if makeflags is None else makeflags
    jobs = 1
    for match in re.finditer(r"(?:^|\s)(?:-[A-Za-z]*j|--jobs=)(\d+)", flags):
        jobs = int(match.group(1))
    return max(1, jobs)


def _build_child(root: Path, module: dict[str, Any], connection: Any) -> None:
    error = None
    try:
        os.environ.update(local_environment(root))
        build_module(root, module)
    except (
        OverlayBuildError, OverlaySourceError, BuildError, WorkspaceError,
        OSError, TypeError, ValueError, json.JSONDecodeError,
    ) as caught:
        error = f"{module.get('name')}: {caught}"
    connection.send(error)
    connection.close()


def build_modules(root: Path, modules: list[dict[str, Any]], jobs: int) -> None:
    if jobs <= 1 or len(modules) <= 1:
        for module in modules:
            build_module(root, module)
        return
    global _IN_PROCESS_SPLAT
    import splat.scripts.split as split_script

    _IN_PROCESS_SPLAT = split_script
    context = multiprocessing.get_context("fork")
    pending = iter(modules)
    running: dict[Any, tuple[Any, Any]] = {}
    failure: str | None = None
    try:
        while True:
            while failure is None and len(running) < jobs:
                module = next(pending, None)
                if module is None:
                    break
                # One fresh fork per module isolates Splat's process-global
                # state without starting a new interpreter for every module.
                receiver, sender = context.Pipe(duplex=False)
                process = context.Process(
                    target=_build_child, args=(root, module, sender)
                )
                process.start()
                sender.close()
                running[process.sentinel] = (process, receiver)
            if not running:
                break
            for sentinel in multiprocessing.connection.wait(list(running)):
                process, receiver = running.pop(sentinel)
                process.join()
                try:
                    result = receiver.recv()
                except EOFError:
                    result = f"overlay build worker exited with {process.exitcode}"
                receiver.close()
                if result is not None and failure is None:
                    failure = result
    finally:
        for process, receiver in running.values():
            process.terminate()
            process.join()
            receiver.close()
        _IN_PROCESS_SPLAT = None
    if failure is not None:
        raise OverlayBuildError(failure)


def load_modules(root: Path, region: str = "usa") -> list[dict[str, Any]]:
    path = resolve_within(
        root, OVERLAY_MANIFESTS[region], must_exist=True
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
    asm_directory = resolve_within(
        root, module_root.relative_to(root) / "asm"
    )
    sources = sorted(asm_directory.rglob("*.s"))
    if not sources:
        return []
    assembler = tool(root, "as")
    include_directory = resolve_within(
        root, module_root.relative_to(root) / "include", must_exist=True
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


def convert_binary_assets(
    root: Path, module_root: Path, linker_script: Path
) -> list[Path]:
    """Convert Splat bin subsegments into the objects its linker script names."""
    assets = resolve_within(root, module_root.relative_to(root) / "assets")
    if not assets.exists():
        return []
    script = linker_script.read_text(encoding="utf-8")
    objcopy = tool(root, "objcopy")
    output_format = (
        "elf32-tradlittlemips"
        if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") == "1"
        else "elf32-littlemips"
    )
    objects: list[Path] = []
    for asset in sorted(assets.rglob("*.bin")):
        relative = asset.relative_to(root).with_suffix(".o")
        output = resolve_within(
            root, module_root.relative_to(root) / "build" / relative
        )
        if f"{output.relative_to(root)}(" not in script:
            raise OverlayBuildError(
                f"{asset.relative_to(root)}: binary asset is not linked"
            )
        output.parent.mkdir(parents=True, exist_ok=True)
        run(
            root,
            [
                str(objcopy), "-I", "binary", "-O", output_format,
                "-B", "mips", str(asset.relative_to(root)), str(output),
            ],
        )
        objects.append(output)
    return objects


def clear_generated_assembly(root: Path, module_root: Path) -> None:
    assembly = resolve_within(root, module_root.relative_to(root) / "asm")
    if assembly.exists():
        # Splat does not remove assembly files for functions newly replaced by C.
        shutil.rmtree(assembly)


def object_symbols(root: Path, path: Path) -> dict[str, str]:
    result = subprocess.run(
        [str(tool(root, "objdump")), "-t", str(path)],
        cwd=root, capture_output=True, text=True, check=False,
    )
    if result.returncode != 0:
        raise OverlayBuildError(
            f"cannot inspect symbols in {path.relative_to(root)}: "
            + result.stderr.strip()
        )
    if "SYMBOL TABLE:" not in result.stdout:
        raise OverlayBuildError(f"missing symbol table in {path.relative_to(root)}")
    symbols: dict[str, str] = {}
    for line in result.stdout.split("SYMBOL TABLE:", 1)[1].splitlines():
        if not line.strip():
            continue
        match = re.fullmatch(
            r"[0-9a-fA-F]+ (.{7}) (\S+)\s+[0-9a-fA-F]+ (.*)", line
        )
        if match is None:
            raise OverlayBuildError(
                f"invalid symbol record in {path.relative_to(root)}: {line}"
            )
        flags, section, name = match.groups()
        if "g" in flags or "w" in flags or section == "*COM*":
            if not name:
                raise OverlayBuildError(
                    f"unnamed global symbol in {path.relative_to(root)}"
                )
            symbols[name] = section
    return symbols


def verify_data_symbols(root: Path, objects: list[Path], elf: Path) -> None:
    """A hash can match while an absolute alias overrides a C definition."""
    if not objects:
        return
    linked = object_symbols(root, elf)
    data_sections = {".data", ".rodata", ".sdata", ".sbss", ".bss", "*COM*"}
    owners: dict[str, Path] = {}
    for obj in objects:
        for name, section in object_symbols(root, obj).items():
            if section not in data_sections and not any(
                section.startswith(prefix + ".")
                for prefix in data_sections if not prefix.startswith("*")
            ):
                continue
            if name in owners:
                raise OverlayBuildError(
                    f"duplicate C data definition {name}: "
                    f"{owners[name].relative_to(root)}, {obj.relative_to(root)}"
                )
            owners[name] = obj
            final_section = linked.get(name)
            if final_section is None or final_section.startswith("*"):
                raise OverlayBuildError(
                    f"{elf.relative_to(root)}: C data symbol {name} from "
                    f"{obj.relative_to(root)} is not section-defined "
                    f"({final_section or 'missing'}); remove overriding linker "
                    "assignments and mark owned Splat symbols defined"
                )


def run_splat(root: Path, config: Path) -> None:
    if _IN_PROCESS_SPLAT is None:
        splat = resolve_within(
            root, "tools/environments/python/bin/splat", must_exist=True
        )
        run(root, [str(splat), "split", str(config)])
        return
    # Only reached in a freshly forked worker, so Splat's global state is
    # never shared between modules.
    parser = argparse.ArgumentParser(prog="splat split")
    _IN_PROCESS_SPLAT.add_arguments_to_parser(parser)
    try:
        _IN_PROCESS_SPLAT.process_arguments(parser.parse_args([str(config)]))
    except SystemExit as error:
        if error.code not in (None, 0):
            raise BuildError(f"command failed with exit code {error.code}: splat") from error


def build_module(root: Path, module: dict[str, Any]) -> None:
    name, module_root, _target, config, built_elf, built_binary = module_paths(
        root, module
    )
    segments = c_segments(root, config)
    clear_generated_assembly(root, module_root)
    stale_assets = resolve_within(root, module_root.relative_to(root) / "assets")
    if stale_assets.exists():
        shutil.rmtree(stale_assets)
    run_splat(root, config)
    linker_script = resolve_within(
        root, module_root.relative_to(root) / f"{name}.ld", must_exist=True
    )
    c_objects = compile_sources(root, module_root, segments)
    asm_objects = assemble_sources(root, module_root)
    binary_objects = convert_binary_assets(root, module_root, linker_script)
    if not c_objects and not asm_objects and not binary_objects:
        raise OverlayBuildError(f"{name}: no C, generated assembly or binary objects")

    linker = tool(root, "ld")
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
    linker_scripts = [linker_script, undefined_functions, undefined_symbols]
    linker_symbols = module.get("linker_symbols")
    if linker_symbols is not None:
        if not isinstance(linker_symbols, str) or not linker_symbols:
            raise OverlayBuildError(
                "overlay field linker_symbols must be a non-empty string"
            )
        linker_scripts.append(
            resolve_within(root, linker_symbols, must_exist=True)
        )
    built_elf.parent.mkdir(parents=True, exist_ok=True)
    run(
        root,
        [
            str(linker),
            "-EL",
            *linker_compatibility_flags(),
            *(argument for script in linker_scripts for argument in ("-T", str(script))),
            "-o",
            str(built_elf),
        ],
    )
    verify_data_symbols(
        root,
        [obj for segment, obj in zip(segments, c_objects)
         if segment["kind"] == "data"],
        built_elf,
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
    parser.add_argument("--region", choices=tuple(OVERLAY_MANIFESTS), default="usa")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        modules = load_modules(root, args.region)
        if args.command == "build":
            build_modules(root, modules, job_count())
        else:
            for module in modules:
                verify_module(root, module)
    except (
        OverlayBuildError,
        OverlaySourceError,
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
