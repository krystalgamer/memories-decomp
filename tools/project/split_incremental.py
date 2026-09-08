#!/usr/bin/env python3

from __future__ import annotations

import importlib.metadata
import json
import os
import platform
import sys
from pathlib import Path
from typing import Any

import yaml
from packaging.requirements import Requirement
from packaging.utils import canonicalize_name
from splat.segtypes.common.c import C_FUNC_RE, C_GLOBAL_ASM_IDO_RE, CommonSegC
from splat.util import options as splat_options
from splat.util.compiler import IDO

import build_baseline
import clean
import generate_build_config
from build_incremental import digest_value, sha256
from workspace import WorkspaceError, require_workspace_root, resolve_within


class IncrementalSplitError(RuntimeError):
    pass


CACHE_PATH = "tmp/incremental/split-cache.json"
CONFIG_PATH = "config/slus_01411/split.yaml"
GENERATED_CONFIG = "tmp/generated/slus_01411.split.yaml"
OUTPUT_ROOTS = ("tmp/generated", "tmp/splat")
EXCLUDED_OUTPUTS = ("tmp/splat/build", "tmp/splat/cache")
REQUIRED_OUTPUTS = (
    GENERATED_CONFIG,
    "tmp/generated/text_sources.json",
    "tmp/generated/data_sources.json",
    "tmp/splat/slus_01411.ld",
    "tmp/splat/undefined_funcs_auto.txt",
    "tmp/splat/undefined_syms_auto.txt",
    "tmp/splat/asm/header.s",
    "tmp/splat/asm/data/initialized_data.data.s",
    "tmp/splat/assets/bss_image.bin",
    "tmp/splat/assets/reserved_zero.bin",
    "tmp/splat/assets/tail_data.bin",
)
INPUT_FILES = (
    CONFIG_PATH,
    "config/slus_01411/functions.csv",
    "config/slus_01411/matching_c.json",
    "config/slus_01411/data_c.json",
    "config/slus_01411/compiler_profiles.json",
    "config/slus_01411/image_map.json",
    "tools/project/generate_build_config.py",
    "tools/project/split_incremental.py",
    "tools/project/build_incremental.py",
    "tools/project/build_baseline.py",
    "tools/project/workspace.py",
    "tools/project/clean.py",
    "tools/bootstrap/tools.json",
    "tools/bootstrap/requirements.lock",
    "tools/bootstrap/bootstrap-requirements.lock",
    "tools/environments/python/bin/splat",
)


def resolved_relative_path(root: Path, path: Path) -> tuple[str, Path]:
    try:
        resolved = path.resolve(strict=True)
        return resolved.relative_to(root).as_posix(), resolved
    except (ValueError, OSError) as error:
        raise IncrementalSplitError(f"split input is not inside the workspace: {path}") from error


def relative_path(root: Path, path: Path) -> str:
    return resolved_relative_path(root, path)[0]


def source_file_paths(root: Path, directory: Path) -> dict[str, Path]:
    result: dict[str, Path] = {}
    for path in sorted(directory.rglob("*")):
        if "__pycache__" in path.parts or path.suffix in {".pyc", ".pyo"}:
            continue
        if path.is_file():
            name, resolved = resolved_relative_path(root, path)
            if path.suffix == ".c":
                result[name] = resolved
    return result


def splitter_tools() -> dict[str, Any]:
    """Hash the installed splitter dependency closure, not just version labels."""
    pending = [Requirement(name) for name in ("splat64[mips]", "PyYAML", "packaging")]
    visited: set[tuple[str, tuple[str, ...]]] = set()
    installed: dict[str, importlib.metadata.Distribution] = {}
    distributions: dict[str, str] = {}
    files: dict[str, str] = {}
    while pending:
        requested = pending.pop()
        name = canonicalize_name(requested.name)
        key = (name, tuple(sorted(requested.extras)))
        if key in visited:
            continue
        visited.add(key)
        if name not in installed:
            try:
                distribution = importlib.metadata.distribution(name)
            except importlib.metadata.PackageNotFoundError as error:
                raise IncrementalSplitError(f"missing splitter dependency: {name}") from error
            installed[name] = distribution
            distributions[name] = distribution.version
            if distribution.files is None:
                raise IncrementalSplitError(f"splitter dependency has no file inventory: {name}")
            for item in distribution.files:
                if "__pycache__" in item.parts or item.suffix in {".pyc", ".pyo"}:
                    continue
                path = Path(distribution.locate_file(item)).resolve(strict=True)
                filename = str(path)
                if path.is_file() and filename not in files:
                    files[filename] = sha256(path)
        distribution = installed[name]
        for dependency in distribution.requires or ():
            requirement = Requirement(dependency)
            if requirement.marker is None or any(
                requirement.marker.evaluate({"extra": extra})
                for extra in ("", *requested.extras)
            ):
                pending.append(requirement)
    executable = Path(sys.executable).resolve(strict=True)
    return {
        "distributions": distributions,
        "files": files,
        "python": {
            "executable": str(executable),
            "sha256": sha256(executable),
            "version": sys.version,
            "platform": platform.platform(),
            "pythonpath": os.environ.get("PYTHONPATH"),
            "hashseed": os.environ.get("PYTHONHASHSEED"),
        },
        "spimdisasm_environment": {
            name: value for name, value in os.environ.items()
            if name.startswith("SPIMDISASM_")
        },
    }


def source_shape(path: Path) -> dict[str, list[str]]:
    # Splat does not preprocess C. Its own parser observes only these names;
    # ordinary body/header edits belong to the object cache, not the split.
    text = CommonSegC.strip_c_comments(path.read_text(encoding="utf-8"))
    definitions = {match.group(1) for match in C_FUNC_RE.finditer(text)}
    if splat_options.opts.compiler == IDO:
        asm = {match.group(2) for match in C_GLOBAL_ASM_IDO_RE.finditer(text)}
        rodata = asm
    else:
        try:
            asm = set(CommonSegC.find_include_asm(text))
            rodata = set(CommonSegC.find_include_rodata(text))
        except IndexError as error:
            raise IncrementalSplitError(f"unterminated Splat include macro in {path}") from error
    return {
        "functions": sorted(definitions),
        "include_asm": sorted(asm),
        "include_rodata": sorted(rodata),
    }


def input_signature(root: Path) -> str:
    config_path = resolve_within(root, CONFIG_PATH, must_exist=True)
    config = yaml.safe_load(config_path.read_text(encoding="utf-8"))
    if not isinstance(config, dict):
        raise IncrementalSplitError("invalid resident split configuration")
    splat_options.initialize(config, [resolve_within(root, GENERATED_CONFIG)])
    options = splat_options.opts
    if options.base_path.resolve() != root:
        raise IncrementalSplitError("incremental splitting requires the repository base path")
    if config["options"].get("basename") != "slus_01411":
        raise IncrementalSplitError("incremental splitting requires the resident basename")
    for name in (
        "asm_path", "asset_path", "data_path", "nonmatchings_path", "matchings_path",
        "generated_asm_macros_directory", "ld_script_path", "ld_symbol_header_path",
        "ld_partial_scripts_path", "elf_section_list_path",
        "undefined_funcs_auto_path", "undefined_syms_auto_path",
    ):
        path = getattr(options, name)
        if path is None:
            continue
        resolved = path.resolve()
        if not resolved.is_relative_to(root / "tmp/splat"):
            raise IncrementalSplitError(f"{name} must remain below tmp/splat")
        if any(resolved.is_relative_to(root / excluded) for excluded in EXCLUDED_OUTPUTS):
            raise IncrementalSplitError(f"{name} must not use an excluded output directory")
    for name in (
        "build_path", "lib_path", "o_path", "cache_path", "elf_path",
        "ld_partial_build_segments_path",
    ):
        path = getattr(options, name)
        if path is not None and not path.resolve().is_relative_to(root / "tmp/splat"):
            raise IncrementalSplitError(f"{name} must remain below tmp/splat")
    if options.ld_partial_linking:
        raise IncrementalSplitError(
            "incremental splitting requires the resident single-linker layout; use make split"
        )
    if options.extensions_path is not None:
        raise IncrementalSplitError(
            "incremental splitting cannot fingerprint arbitrary extension inputs; use make split"
        )

    source_directory = options.src_path.resolve(strict=True)
    relative_path(root, source_directory)
    resolved_sources = source_file_paths(root, source_directory)

    # Also repeat the generator's ownership/group checks on cache hits.
    functions = generate_build_config.load_matching_functions(
        root,
        resolved_sources=resolved_sources,
    )
    inputs = {
        name: sha256(resolve_within(root, name, must_exist=True))
        for name in INPUT_FILES
    }
    for path in (
        options.target_path,
        *options.symbol_addrs_paths,
        *options.reloc_addrs_paths,
    ):
        name = relative_path(root, path)
        inputs[name] = sha256(root / name)
    source_paths = {str(function["source"]) for function in functions}
    # Include C files owning dotted data sections even if no function is mapped
    # to them yet. Extra source shapes are conservative; bodies are not hashed.
    source_paths.update(resolved_sources)
    shapes = {}
    for name in sorted(source_paths):
        source = resolved_sources.get(name)
        if source is None:
            source = resolve_within(root, name, must_exist=True)
        shapes[name] = source_shape(source)
    return digest_value({"schema": 1, "inputs": inputs, "sources": shapes, "tools": splitter_tools()})


def allowed_output(name: str) -> bool:
    path = Path(name)
    if path.is_absolute() or ".." in path.parts or path.as_posix() != name:
        return False
    return (
        any(name.startswith(directory + "/") for directory in OUTPUT_ROOTS)
        and not any(name == excluded or name.startswith(excluded + "/")
                    for excluded in EXCLUDED_OUTPUTS)
    )


def output_snapshot(root: Path) -> dict[str, str]:
    outputs: dict[str, str] = {}
    for directory in OUTPUT_ROOTS:
        base = resolve_within(root, directory)
        if not base.is_dir() or (root / directory).is_symlink():
            return {}
        pending = [base]
        while pending:
            for path in sorted(pending.pop().iterdir()):
                name = path.relative_to(root).as_posix()
                if not allowed_output(name):
                    continue
                if path.is_symlink():
                    return {}
                if path.is_dir():
                    pending.append(path)
                elif path.is_file():
                    outputs[name] = sha256(path)
                else:
                    return {}
    if not all(name in outputs for name in REQUIRED_OUTPUTS):
        return {}
    return outputs


def state_file(root: Path) -> Path:
    path = resolve_within(root, CACHE_PATH)
    if path != root / CACHE_PATH:
        raise IncrementalSplitError("split cache path must not contain symlinks")
    return path


def load_state(root: Path) -> tuple[dict[str, Any] | None, str]:
    path = state_file(root)
    try:
        state = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        return None, "no split cache"
    except (UnicodeError, json.JSONDecodeError):
        return None, "invalid split cache"
    if not isinstance(state, dict) or state.get("schema") != 1:
        return None, "unsupported split cache"
    outputs = state.get("outputs")
    signature = state.get("input_signature")
    if (
        not isinstance(signature, str)
        or len(signature) != 64
        or not isinstance(outputs, dict)
        or not all(isinstance(name, str) and allowed_output(name)
                   and isinstance(value, str) and len(value) == 64
                   for name, value in outputs.items())
        or not all(name in outputs for name in REQUIRED_OUTPUTS)
    ):
        return None, "invalid split cache"
    return state, ""


def regenerate(root: Path) -> None:
    clean.remove_target(root, "generated")
    clean.remove_target(root, "splat")
    build_baseline.run(root, [sys.executable, "tools/project/generate_build_config.py"])
    splat = resolve_within(root, "tools/environments/python/bin/splat", must_exist=True)
    build_baseline.run(root, [str(splat), "split", GENERATED_CONFIG])


def prepare(root: Path) -> bool:
    """Return True only when validated generated output can be reused."""
    signature = input_signature(root)
    state, reason = load_state(root)
    if state is not None:
        if state["input_signature"] != signature:
            reason = "split inputs changed"
        elif output_snapshot(root) != state["outputs"]:
            reason = "generated split output changed or is missing"
        else:
            print(f"incremental split: reused {len(state['outputs'])} generated files")
            return True

    state_path = state_file(root)
    state_path.unlink(missing_ok=True)
    print(f"incremental split: regenerating ({reason})")
    regenerate(root)
    outputs = output_snapshot(root)
    if not outputs:
        raise IncrementalSplitError("split completed without the required generated outputs")
    if input_signature(root) != signature:
        raise IncrementalSplitError("split inputs changed during generation; retry the build")
    state_path.parent.mkdir(parents=True, exist_ok=True)
    if resolve_within(root, f"{CACHE_PATH}.tmp") != root / f"{CACHE_PATH}.tmp":
        raise IncrementalSplitError("split cache temporary path must not contain symlinks")
    generate_build_config.write_json(
        state_path,
        {"schema": 1, "input_signature": signature, "outputs": outputs},
    )
    return False


def main() -> int:
    try:
        prepare(require_workspace_root())
    except (
        IncrementalSplitError, build_baseline.BuildError, clean.CleanError,
        generate_build_config.GenerationError, WorkspaceError,
        OSError, UnicodeError, ValueError, TypeError, KeyError,
        json.JSONDecodeError, yaml.YAMLError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
