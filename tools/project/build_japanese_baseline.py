#!/usr/bin/env python3

from __future__ import annotations

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
SPLAT_DIRECTORY = "tmp/splat/slpm_86398"
OUTPUT_DIRECTORY = "tmp/project-build"


def run(root: Path, command: list[str]) -> None:
    environment = os.environ.copy()
    environment.update(local_environment(root))
    try:
        subprocess.run(command, cwd=root, env=environment, check=True)
    except subprocess.CalledProcessError as error:
        raise BuildError(
            f"command failed with exit code {error.returncode}: {command[0]}"
        ) from error


def tool(root: Path, name: str) -> Path:
    path = resolve_within(
        root,
        f"{TOOLCHAIN}/mipsel-none-elf-{name}",
        must_exist=True,
    )
    if not path.is_file():
        raise BuildError(f"required tool is not a file: {path}")
    return path


def assembler_compatibility_flags() -> list[str]:
    if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") == "1":
        return ["-no-pad-sections", "-O1"]
    return []


def linker_compatibility_flags() -> list[str]:
    flags = ["--no-relax"]
    if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") != "1":
        flags.append("--no-warn-rwx-segments")
    return flags


def object_path(root: Path, source: Path) -> Path:
    relative = source.relative_to(root).with_suffix(".o")
    return resolve_within(root, f"{SPLAT_DIRECTORY}/build/{relative}")


def normalize_alignment(root: Path, output: Path) -> None:
    if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") != "1":
        return
    objcopy = tool(root, "objcopy")
    run(
        root,
        [
            str(objcopy),
            "--set-section-alignment",
            ".text=4",
            "--set-section-alignment",
            ".data=4",
            "--set-section-alignment",
            ".rodata=4",
            str(output),
        ],
    )


def assemble(root: Path, source: Path) -> None:
    output = object_path(root, source)
    output.parent.mkdir(parents=True, exist_ok=True)
    run(
        root,
        [
            str(tool(root, "as")),
            "-EL",
            "-march=r3000",
            "-mabi=32",
            "-G0",
            *assembler_compatibility_flags(),
            "-I",
            str(
                resolve_within(
                    root,
                    f"{SPLAT_DIRECTORY}/include",
                    must_exist=True,
                )
            ),
            "-o",
            str(output),
            str(source),
        ],
    )
    normalize_alignment(root, output)


def wrap_binary(root: Path, source: Path) -> None:
    output = object_path(root, source)
    output.parent.mkdir(parents=True, exist_ok=True)
    output_format = (
        "elf32-tradlittlemips"
        if os.environ.get("USE_SYSTEM_MIPS_BINUTILS") == "1"
        else "elf32-littlemips"
    )
    run(
        root,
        [
            str(tool(root, "objcopy")),
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


def build(root: Path) -> Path:
    splat = resolve_within(root, SPLAT_DIRECTORY, must_exist=True)
    for source in sorted((splat / "asm").rglob("*.s")):
        assemble(root, source)
    for source in sorted((splat / "assets").rglob("*.bin")):
        wrap_binary(root, source)

    output_directory = resolve_within(root, OUTPUT_DIRECTORY)
    output_directory.mkdir(parents=True, exist_ok=True)
    output_elf = output_directory / "SLPM_863.98.elf"
    output_map = output_directory / "SLPM_863.98.map"
    output_exe = output_directory / "SLPM_863.98"
    run(
        root,
        [
            str(tool(root, "ld")),
            "-EL",
            "-G0",
            *linker_compatibility_flags(),
            "-T",
            str(
                resolve_within(
                    root,
                    f"{SPLAT_DIRECTORY}/slpm_86398.ld",
                    must_exist=True,
                )
            ),
            "-T",
            str(
                resolve_within(
                    root,
                    f"{SPLAT_DIRECTORY}/undefined_funcs_auto.txt",
                    must_exist=True,
                )
            ),
            "-T",
            str(
                resolve_within(
                    root,
                    f"{SPLAT_DIRECTORY}/undefined_syms_auto.txt",
                    must_exist=True,
                )
            ),
            "-Map",
            str(output_map),
            "-o",
            str(output_elf),
        ],
    )
    run(
        root,
        [
            str(tool(root, "objcopy")),
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
    except (BuildError, WorkspaceError, OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"built: {output.relative_to(root)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
