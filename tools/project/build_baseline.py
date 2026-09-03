#!/usr/bin/env python3

from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path

from workspace import (
    WorkspaceError,
    local_environment,
    require_workspace_root,
    resolve_within,
)


class BuildError(RuntimeError):
    pass


TOOLCHAIN = "tools/toolchains/binutils-2.42/bin"
OBJECT_DIRECTORY = "tmp/project-build/obj"


def run(root: Path, command: list[str]) -> None:
    environment = os.environ.copy()
    environment.update(local_environment(root))
    try:
        subprocess.run(
            command,
            cwd=root,
            env=environment,
            check=True,
        )
    except subprocess.CalledProcessError as error:
        raise BuildError(
            f"command failed with exit code {error.returncode}: {command[0]}"
        ) from error


def tool(root: Path, name: str) -> Path:
    path = resolve_within(root, f"{TOOLCHAIN}/mipsel-none-elf-{name}", must_exist=True)
    if not path.is_file():
        raise BuildError(f"required tool is not a file: {path}")
    return path


def linker_compatibility_flags() -> list[str]:
    flags = ["--no-relax"]
    if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") != "1":
        flags.append("--no-warn-rwx-segments")
    return flags


def assembler_compatibility_flags() -> list[str]:
    if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") == "1":
        return ["-no-pad-sections", "-O1"]
    return []


def normalize_text_alignment(root: Path, output: Path) -> None:
    if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") != "1":
        return
    objcopy = tool(root, "objcopy")
    run(
        root,
        [
            str(objcopy),
            "--set-section-alignment",
            ".text=4",
            str(output),
        ],
    )


def require_generated_file(root: Path, relative_path: str) -> Path:
    path = resolve_within(root, relative_path, must_exist=True)
    if not path.is_file():
        raise BuildError(f"required generated input is not a file: {relative_path}")
    return path


def assemble(
    root: Path,
    assembler: Path,
    source_path: str,
    output_path: str,
) -> Path:
    source = require_generated_file(root, source_path)
    output = resolve_within(root, output_path)
    output.parent.mkdir(parents=True, exist_ok=True)
    include_directory = resolve_within(root, "tmp/splat/include", must_exist=True)
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
    return output


def assemble_c_output(
    root: Path,
    assembler: Path,
    source: Path,
    output: Path,
    data_limit: int,
) -> Path:
    output.parent.mkdir(parents=True, exist_ok=True)
    run(
        root,
        [
            str(assembler),
            "-EL",
            "-mips1",
            f"-G{data_limit}",
            *assembler_compatibility_flags(),
            "-o",
            str(output),
            str(source),
        ],
    )
    normalize_text_alignment(root, output)
    return output


def compile_c(
    root: Path,
    assembler: Path,
    segment: dict[str, object],
    profiles: dict[str, dict[str, object]],
) -> Path:
    source = resolve_within(root, str(segment["source"]), must_exist=True)
    profile_name = segment.get("profile")
    if not isinstance(profile_name, str) or profile_name not in profiles:
        raise BuildError(f"invalid compiler profile for {source}")
    profile = profiles[profile_name]
    compiler = resolve_within(root, str(profile["compiler"]), must_exist=True)
    maspsx = resolve_within(
        root, "tools/vendor/maspsx/maspsx.py", must_exist=True
    )
    if not source.is_file() or not compiler.is_file() or not maspsx.is_file():
        raise BuildError(f"invalid C build input for {source}")

    object_name = str(segment["object"])
    output = resolve_within(root, f"{OBJECT_DIRECTORY}/{object_name}")
    raw_assembly = resolve_within(
        root, f"tmp/project-build/asm/{object_name}.compiler.s"
    )
    filtered_assembly = resolve_within(
        root, f"tmp/project-build/asm/{object_name}.filtered.s"
    )
    transformed_assembly = resolve_within(
        root, f"tmp/project-build/asm/{object_name}.maspsx.s"
    )
    raw_assembly.parent.mkdir(parents=True, exist_ok=True)

    compiler_flags = profile.get("compiler_flags")
    maspsx_flags = profile.get("maspsx_flags")
    aspsx_version = profile.get("aspsx_version")
    data_limit = profile.get("data_limit")
    assembly_filter = profile.get("assembly_filter")
    if (
        not isinstance(compiler_flags, list)
        or not all(isinstance(flag, str) for flag in compiler_flags)
        or not isinstance(maspsx_flags, list)
        or not all(isinstance(flag, str) for flag in maspsx_flags)
        or not isinstance(aspsx_version, str)
        or not isinstance(data_limit, int)
        or isinstance(data_limit, bool)
        or data_limit < 0
        or (
            assembly_filter is not None
            and not isinstance(assembly_filter, str)
        )
    ):
        raise BuildError(f"invalid C compiler configuration for {source}")

    run(
        root,
        [
            str(compiler),
            "-S",
            *compiler_flags,
            "-o",
            str(raw_assembly),
            str(source),
        ],
    )

    maspsx_input = raw_assembly
    environment = os.environ.copy()
    environment.update(local_environment(root))
    if assembly_filter is not None:
        filter_path = resolve_within(
            root, assembly_filter, must_exist=True
        )
        try:
            with raw_assembly.open("rb") as input_handle:
                with filtered_assembly.open("wb") as output_handle:
                    subprocess.run(
                        [sys.executable, str(filter_path)],
                        cwd=root,
                        env=environment,
                        stdin=input_handle,
                        stdout=output_handle,
                        check=True,
                    )
        except subprocess.CalledProcessError as error:
            raise BuildError(
                f"assembly filter failed with exit code "
                f"{error.returncode}: {source}"
            ) from error
        maspsx_input = filtered_assembly

    try:
        with maspsx_input.open("rb") as input_handle:
            with transformed_assembly.open("wb") as output_handle:
                subprocess.run(
                    [
                        sys.executable,
                        str(maspsx),
                        f"--aspsx-version={aspsx_version}",
                        *maspsx_flags,
                    ],
                    cwd=root,
                    env=environment,
                    stdin=input_handle,
                    stdout=output_handle,
                    check=True,
                )
    except subprocess.CalledProcessError as error:
        raise BuildError(
            f"maspsx failed with exit code {error.returncode}: {source}"
        ) from error

    return assemble_c_output(
        root,
        assembler,
        transformed_assembly,
        output,
        data_limit,
    )


def load_text_segments(root: Path) -> list[dict[str, object]]:
    manifest = resolve_within(
        root,
        "tmp/generated/text_sources.json",
        must_exist=True,
    )
    with manifest.open("r", encoding="utf-8") as handle:
        configuration = json.load(handle)
    if configuration.get("schema") != 1:
        raise BuildError(f"{manifest}: unsupported text-source schema")
    segments = configuration.get("segments")
    if not isinstance(segments, list) or not segments:
        raise BuildError(f"{manifest}: segments must be a non-empty list")
    return segments


def load_compiler_profiles(root: Path) -> dict[str, dict[str, object]]:
    manifest = resolve_within(
        root,
        "config/slus_01411/compiler_profiles.json",
        must_exist=True,
    )
    with manifest.open("r", encoding="utf-8") as handle:
        configuration = json.load(handle)
    if configuration.get("schema") != 1:
        raise BuildError(f"{manifest}: unsupported compiler-profile schema")
    profiles = configuration.get("profiles")
    if not isinstance(profiles, dict) or not profiles:
        raise BuildError(f"{manifest}: profiles must be a non-empty object")
    for name, profile in profiles.items():
        if not isinstance(name, str) or not isinstance(profile, dict):
            raise BuildError(f"{manifest}: invalid compiler profile")
    return profiles


def build_text_objects(root: Path, assembler: Path) -> list[Path]:
    objects: list[Path] = []
    seen_objects: set[str] = set()
    profiles = load_compiler_profiles(root)
    for index, segment in enumerate(load_text_segments(root)):
        if not isinstance(segment, dict):
            raise BuildError(f"text segment {index} must be an object")
        kind = segment.get("kind")
        source = segment.get("source")
        object_name = segment.get("object")
        if (
            kind not in ("asm", "c")
            or not isinstance(source, str)
            or not isinstance(object_name, str)
            or not object_name.endswith(".o")
            or "/" in object_name
            or object_name in seen_objects
        ):
            raise BuildError(f"invalid text segment {index}")
        seen_objects.add(object_name)
        if kind == "asm":
            objects.append(
                assemble(
                    root,
                    assembler,
                    source,
                    f"{OBJECT_DIRECTORY}/{object_name}",
                )
            )
        else:
            objects.append(compile_c(root, assembler, segment, profiles))
    return objects


def binary_object(
    root: Path,
    objcopy: Path,
    source_path: str,
    output_path: str,
) -> Path:
    source = require_generated_file(root, source_path)
    output = resolve_within(root, output_path)
    output.parent.mkdir(parents=True, exist_ok=True)
    output_format = (
        "elf32-tradlittlemips"
        if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") == "1"
        else "elf32-littlemips"
    )
    run(
        root,
        [
            str(objcopy),
            "-I",
            "binary",
            "-O",
            output_format,
            "-B",
            "mips",
            str(source),
            str(output),
        ],
    )
    return output


def build(root: Path) -> Path:
    assembler = tool(root, "as")
    linker = tool(root, "ld")
    objcopy = tool(root, "objcopy")

    objects = [
        assemble(
            root,
            assembler,
            "tmp/splat/asm/header.s",
            f"{OBJECT_DIRECTORY}/header.o",
        ),
        assemble(
            root,
            assembler,
            "tmp/splat/asm/data/initial_data.data.s",
            f"{OBJECT_DIRECTORY}/initial_data.o",
        ),
        *build_text_objects(root, assembler),
        assemble(
            root,
            assembler,
            "tmp/splat/asm/data/initialized_data.data.s",
            f"{OBJECT_DIRECTORY}/initialized_data.o",
        ),
        binary_object(
            root,
            objcopy,
            "tmp/splat/assets/bss_image.bin",
            f"{OBJECT_DIRECTORY}/bss_image.o",
        ),
        binary_object(
            root,
            objcopy,
            "tmp/splat/assets/reserved_zero.bin",
            f"{OBJECT_DIRECTORY}/reserved_zero.o",
        ),
        binary_object(
            root,
            objcopy,
            "tmp/splat/assets/overlays/overlay_slots.bin",
            f"{OBJECT_DIRECTORY}/overlay_slots.o",
        ),
        binary_object(
            root,
            objcopy,
            "tmp/splat/assets/tail_data.bin",
            f"{OBJECT_DIRECTORY}/tail_data.o",
        ),
    ]

    linker_script = resolve_within(
        root, "linker/slus_01411.ld", must_exist=True
    )
    output_elf = resolve_within(root, "tmp/project-build/SLUS_014.11.elf")
    output_map = resolve_within(root, "tmp/project-build/SLUS_014.11.map")
    output_exe = resolve_within(root, "tmp/project-build/SLUS_014.11")

    run(
        root,
        [
            str(linker),
            "-EL",
            "-G0",
            *linker_compatibility_flags(),
            "-T",
            str(linker_script),
            "-Map",
            str(output_map),
            "-o",
            str(output_elf),
            *[str(path) for path in objects],
        ],
    )
    run(
        root,
        [
            str(objcopy),
            "-O",
            "binary",
            str(output_elf),
            str(output_exe),
        ],
    )

    expected_size = 0x1D0800
    actual_size = output_exe.stat().st_size
    if actual_size != expected_size:
        raise BuildError(
            f"rebuilt executable is {actual_size:#x} bytes, expected {expected_size:#x}"
        )
    return output_exe


def main() -> int:
    try:
        root = require_workspace_root()
        output = build(root)
    except (
        BuildError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"built: {output.relative_to(root)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
