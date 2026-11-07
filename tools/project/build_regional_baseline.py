#!/usr/bin/env python3

from __future__ import annotations

import json
import os
import re
import subprocess
from dataclasses import dataclass
from pathlib import Path

from build_baseline import BuildError, compile_c, load_compiler_profiles
from workspace import local_environment, resolve_within

TOOLCHAIN = "tools/toolchains/binutils-2.42/bin"


@dataclass(frozen=True)
class RegionalBuildConfiguration:
    splat_directory: str
    output_directory: str
    matching_config: str
    output_name: str
    linker_script: str
    asm_directory: str
    expected_size: int
    link_symbols: str | None = None


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


def object_path(
    root: Path, source: Path, configuration: RegionalBuildConfiguration
) -> Path:
    relative = source.relative_to(root).with_suffix(".o")
    return resolve_within(
        root, f"{configuration.splat_directory}/build/{relative}"
    )


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


def assemble(
    root: Path, source: Path, configuration: RegionalBuildConfiguration
) -> None:
    output = object_path(root, source, configuration)
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
                    f"{configuration.splat_directory}/include",
                    must_exist=True,
                )
            ),
            "-o",
            str(output),
            str(source),
        ],
    )
    normalize_alignment(root, output)


def wrap_binary(
    root: Path, source: Path, configuration: RegionalBuildConfiguration
) -> None:
    output = object_path(root, source, configuration)
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


def compile_matching_sources(
    root: Path, configuration: RegionalBuildConfiguration
) -> list[str]:
    config_path = resolve_within(
        root, configuration.matching_config, must_exist=True
    )
    with config_path.open("r", encoding="utf-8") as handle:
        config = json.load(handle)
    if config.get("schema") != 1 or not isinstance(config.get("functions"), list):
        raise BuildError(f"{config_path}: unsupported matching-C configuration")

    profiles = load_compiler_profiles(root)
    assembler = tool(root, "as")
    # Grouped functions share one object, so one profile must build it.
    compiled: dict[str, object] = {}
    objects: list[str] = []
    owned = owned_data_sections(root, configuration)
    for index, function in enumerate(
        config["functions"] + owned_data_units(root, configuration, config["functions"])
    ):
        if not isinstance(function, dict):
            raise BuildError(f"matching function {index} must be an object")
        source_value = function.get("source")
        if not isinstance(source_value, str):
            raise BuildError(f"matching function {index} has no source")
        source = Path(source_value)
        if source.suffix != ".c" or source.is_absolute():
            raise BuildError(f"matching function {index} has an invalid source")
        if source_value in compiled:
            if compiled[source_value] != function.get("profile"):
                raise BuildError(
                    f"{source_value}: grouped functions use multiple profiles"
                )
            continue
        compiled[source_value] = function.get("profile")
        compile_c(
            root,
            assembler,
            {
                **function,
                "object": f"{source.stem}.o",
            },
            profiles,
            object_directory=(
                f"{configuration.splat_directory}/build/"
                f"{source.parent.as_posix()}"
            ),
            asm_directory=configuration.asm_directory,
        )
        object_name = (
            f"{configuration.splat_directory}/build/"
            f"{source.parent.as_posix()}/{source.stem}.o"
        )
        weaken_data_definitions(
            root,
            resolve_within(root, object_name, must_exist=True),
            {
                section
                for section in (".bss", ".sbss", ".data", ".sdata")
                if f"{object_name}({section})" not in owned
            },
        )
        objects.append(object_name)
    return objects


def weaken_data_definitions(root: Path, object_path: Path, sections: set[str]) -> None:
    """Make an object's data definitions weak in a regional build.

    Several regional wrappers include the same shared source, and every copy
    defines that source's North American data blocks. The region places those
    names through its link file or its own data image and discards the copies,
    so they only need to stop colliding with the region's definitions. A
    section the region's split.yaml gives this object keeps strong symbols.
    """
    listing = subprocess.run(
        [str(tool(root, "objdump")), "-t", str(object_path)],
        check=True,
        capture_output=True,
        text=True,
    ).stdout
    names = []
    for line in listing.splitlines():
        match = re.match(r"^[0-9a-fA-F]+\s+g\s+O?\s*(\.\w+)\s+[0-9a-fA-F]+\s+(\S+)$", line)
        if match and match.group(1) in sections:
            names.append(match.group(2))
    if names:
        run(
            root,
            [
                str(tool(root, "objcopy")),
                *[f"--weaken-symbol={name}" for name in names],
                str(object_path),
            ],
        )


def owned_data_sections(root: Path, configuration: RegionalBuildConfiguration) -> set[str]:
    """Return the object section listings the region's split.yaml places itself.

    A unit that owns a zero-initialized or initialized block in this region is
    listed at its address with a dotted section type; its definitions are the
    region's storage and stay strong.
    """
    split = resolve_within(
        root, str(Path(configuration.matching_config).parent / "split.yaml"), must_exist=True
    )
    owned = set()
    for offset, section, unit in re.findall(
        r"-\s*\[(0x[0-9A-Fa-f]+),\s*(\.sbss|\.bss|\.sdata|\.data),\s*([^\]\s]+)\]",
        split.read_text(encoding="utf-8"),
    ):
        owned.add(f"{configuration.splat_directory}/build/src/{unit}.o({section})")
    return owned


def owned_data_units(
    root: Path,
    configuration: RegionalBuildConfiguration,
    functions: list[object],
) -> list[dict[str, object]]:
    """Data-only units the region's split.yaml places, with their profiles.

    A unit with no text (an initialized table, for instance) is not in the
    matching-C manifest. The region's data_c.json names its profile, falling
    back to the North American one.
    """
    function_sources = {
        function.get("source") for function in functions if isinstance(function, dict)
    }
    profiles: dict[str, object] = {}
    for manifest in (
        "config/slus_01411/data_c.json",
        str(Path(configuration.matching_config).parent / "data_c.json"),
    ):
        path = root / manifest
        if path.is_file():
            with path.open("r", encoding="utf-8") as handle:
                for unit in json.load(handle).get("units", []):
                    profiles[unit["source"]] = unit["profile"]
    units = []
    for section in sorted(owned_data_sections(root, configuration)):
        match = re.search(r"/build/src/(.+)\.o\(", section)
        source = f"src/{match.group(1)}.c" if match else None
        if source is None or source in function_sources or source.startswith("src/psyq/"):
            continue
        if source not in profiles:
            raise BuildError(f"{source}: owns data but has no data_c.json profile")
        function_sources.add(source)
        units.append({"source": source, "profile": profiles[source]})
    return units


def regional_linker_script(
    generated: Path, output: Path, text_objects: list[str], owned: set[str] = frozenset()
) -> Path:
    """Drop compiled text units' .data and .bss from a regional script.

    The shared sources define the North American layout's data blocks. A
    regional executable still places every one of those names through its own
    link file, so the blocks are discarded there just as the unlisted .sbss
    already is, instead of being appended to the image. Regional data-only
    units keep their .data.
    """
    text = generated.read_text(encoding="utf-8")

    def keep_bss(match: re.Match[str]) -> str:
        listing = match.group(2)
        if listing in owned:
            return f"{match.group(1)}. = ALIGN(., 8);\n{match.group(0)}"
        return ""

    kept = re.sub(
        r"^([ \t]*)(\S+/src/\S+\.o\(\.bss\));[ \t]*\n", keep_bss, text, flags=re.M
    )
    # A generated <unit>_bss_commons subsegment stands in for its unit's .bss,
    # which the game's linker started on an 8-byte boundary like any object's.
    kept = re.sub(
        r"^([ \t]*)(\S+_bss_commons\.bss\.o\(\.bss\);)[ \t]*\n",
        lambda m: f"{m.group(1)}. = ALIGN(., 8);\n{m.group(0)}",
        kept,
        flags=re.M,
    )
    for name in text_objects:
        if f"{name}(.data)" in owned:
            continue
        kept = re.sub(
            r"^[ \t]*" + re.escape(name) + r"\(\.data\);[ \t]*\n", "", kept, flags=re.M
        )
    output.write_text(kept, encoding="utf-8")
    return output


def build(root: Path, configuration: RegionalBuildConfiguration) -> Path:
    splat = resolve_within(
        root, configuration.splat_directory, must_exist=True
    )
    for source in sorted((splat / "asm").rglob("*.s")):
        assemble(root, source, configuration)
    for source in sorted((splat / "assets").rglob("*.bin")):
        wrap_binary(root, source, configuration)
    text_objects = compile_matching_sources(root, configuration)

    output_directory = resolve_within(root, configuration.output_directory)
    output_directory.mkdir(parents=True, exist_ok=True)
    output_elf = output_directory / f"{configuration.output_name}.elf"
    output_map = output_directory / f"{configuration.output_name}.map"
    output_exe = output_directory / configuration.output_name
    linker_scripts = [
        regional_linker_script(
            resolve_within(
                root,
                f"{configuration.splat_directory}/{configuration.linker_script}",
                must_exist=True,
            ),
            output_directory / f"{configuration.output_name}.ld",
            text_objects,
            owned_data_sections(root, configuration),
        ),
        resolve_within(
            root,
            f"{configuration.splat_directory}/undefined_funcs_auto.txt",
            must_exist=True,
        ),
        resolve_within(
            root,
            f"{configuration.splat_directory}/undefined_syms_auto.txt",
            must_exist=True,
        ),
    ]
    if configuration.link_symbols is not None:
        linker_scripts.append(
            resolve_within(root, configuration.link_symbols, must_exist=True)
        )
    run(
        root,
        [
            str(tool(root, "ld")),
            "-EL",
            "-G0",
            *linker_compatibility_flags(),
            *[
                argument
                for script in linker_scripts
                for argument in ("-T", str(script))
            ],
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
    actual_size = output_exe.stat().st_size
    if actual_size != configuration.expected_size:
        raise BuildError(
            f"rebuilt executable is {actual_size:#x} bytes, expected "
            f"{configuration.expected_size:#x}"
        )
    return output_exe
