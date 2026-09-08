#!/usr/bin/env python3
"""Generate and verify self-contained human-facing resident candidate bundles."""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import shutil
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CANDIDATE_DIRECTORY = ROOT / "notes/candidates"
BUNDLE_DIRECTORY = CANDIDATE_DIRECTORY / "for_humans"
PROFILE_PATH = ROOT / "config/slus_01411/compiler_profiles.json"
INVENTORY_PATH = ROOT / "config/slus_01411/functions.csv"
ASSEMBLY_ROOT = ROOT / "tmp/splat/asm"

HEADING = re.compile(
    r"^## `(?P<name>[^`]+)` at (?P<address>0x[0-9A-Fa-f]{8})$",
    re.MULTILINE,
)
PROFILE = re.compile(r"^`(?P<profile>[^`]+)`,", re.MULTILINE)
C_BLOCK = re.compile(r"```(?:c|C)\s*\n(?P<body>.*?)```", re.DOTALL)
FUNCTION = re.compile(
    r"^nonmatching\s+(?P<name>\S+),\s+0x(?P<size>[0-9A-Fa-f]+)$"
)
INSTRUCTION = re.compile(
    r"^\s*/\*\s+[0-9A-Fa-f]+\s+"
    r"(?P<address>[0-9A-Fa-f]{8})\s+"
    r"(?P<word>[0-9A-Fa-f]{8})\s+\*/"
)


class BundleError(RuntimeError):
    pass


@dataclass(frozen=True)
class Candidate:
    note_path: Path
    note_name: str
    address: int
    size: int
    inventory_name: str
    profile_name: str
    source: str

    @property
    def key(self) -> str:
        return f"func_{self.address:08X}"

    @property
    def directory(self) -> Path:
        return BUNDLE_DIRECTORY / self.key


@dataclass(frozen=True)
class AssemblyBlock:
    path: Path
    name: str
    address: int
    size: int
    text: str
    words: bytes


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def load_profiles() -> dict[str, dict[str, object]]:
    configuration = json.loads(PROFILE_PATH.read_text(encoding="utf-8"))
    if configuration.get("schema") != 1:
        raise BundleError(f"{PROFILE_PATH.relative_to(ROOT)}: unsupported schema")
    profiles = configuration.get("profiles")
    if not isinstance(profiles, dict):
        raise BundleError(f"{PROFILE_PATH.relative_to(ROOT)}: profiles is not an object")
    return profiles


def load_inventory() -> dict[int, dict[str, str]]:
    with INVENTORY_PATH.open(encoding="utf-8", newline="") as handle:
        rows = list(csv.DictReader(handle))
    return {int(row["address"], 0): row for row in rows}


def self_contained_source(text: str, path: Path) -> str:
    blocks = [match.group("body") for match in C_BLOCK.finditer(text)]
    candidates = [
        block
        for block in blocks
        if "#include" in block or block.lstrip().startswith("//@ target")
    ]
    if len(candidates) != 1:
        raise BundleError(
            f"{path.relative_to(ROOT)}: expected one self-contained C block, "
            f"found {len(candidates)}"
        )
    source = candidates[0]
    if not source.endswith("\n"):
        source += "\n"
    return rewrite_includes(source, path)


def rewrite_includes(source: str, path: Path) -> str:
    """Make project headers resolve when candidate.c stays in its bundle."""

    replacements = {
        '"../types.h"': '"../../../../src/types.h"',
        '"../../src/types.h"': '"../../../../src/types.h"',
    }

    def replace(match: re.Match[str]) -> str:
        target = match.group("target")
        if target in replacements:
            resolved = replacements[target]
        elif target.startswith('"../psyq/'):
            resolved = '"../../../../src/psyq/' + target[len('"../psyq/') :]
        elif target.startswith('"') and "/" not in target:
            resolved = '"../../../../src/game/' + target[1:]
        else:
            raise BundleError(
                f"{path.relative_to(ROOT)}: unsupported candidate include {target}"
            )
        return match.group("prefix") + resolved

    return re.sub(
        r"^(?P<prefix>\s*#include\s+)(?P<target>\"[^\"]+\")$",
        replace,
        source,
        flags=re.MULTILINE,
    )


def load_candidates(
    profiles: dict[str, dict[str, object]],
    inventory: dict[int, dict[str, str]],
) -> list[Candidate]:
    candidates: list[Candidate] = []
    for path in sorted(CANDIDATE_DIRECTORY.glob("func_*.md")):
        text = path.read_text(encoding="utf-8")
        heading = HEADING.search(text)
        profile = PROFILE.search(text)
        if heading is None:
            raise BundleError(f"{path.relative_to(ROOT)}: missing candidate heading")
        if profile is None:
            raise BundleError(f"{path.relative_to(ROOT)}: missing compiler profile")
        address = int(heading.group("address"), 0)
        inventory_row = inventory.get(address)
        if inventory_row is None:
            raise BundleError(
                f"{path.relative_to(ROOT)}: address is absent from functions.csv"
            )
        if inventory_row["status"] == "matching_c":
            raise BundleError(
                f"{path.relative_to(ROOT)}: matching functions cannot have bundles"
            )
        profile_name = profile.group("profile")
        if profile_name not in profiles:
            raise BundleError(
                f"{path.relative_to(ROOT)}: unknown profile {profile_name}"
            )
        candidates.append(
            Candidate(
                note_path=path,
                note_name=heading.group("name"),
                address=address,
                size=int(inventory_row["size"], 0),
                inventory_name=inventory_row["name"],
                profile_name=profile_name,
                source=self_contained_source(text, path),
            )
        )
    return candidates


def assembly_blocks() -> dict[int, AssemblyBlock]:
    if not ASSEMBLY_ROOT.is_dir():
        raise BundleError(
            f"{ASSEMBLY_ROOT.relative_to(ROOT)} is absent; run make split first"
        )
    blocks: dict[int, AssemblyBlock] = {}
    for path in sorted(ASSEMBLY_ROOT.rglob("*.s")):
        lines = path.read_text(encoding="utf-8").splitlines()
        first_function = next(
            (index for index, line in enumerate(lines) if FUNCTION.fullmatch(line)),
            None,
        )
        if first_function is None:
            continue
        preamble = lines[:first_function]
        index = first_function
        while index < len(lines):
            match = FUNCTION.fullmatch(lines[index])
            if match is None:
                index += 1
                continue
            name = match.group("name")
            size = int(match.group("size"), 16)
            end = next(
                (
                    candidate
                    for candidate in range(index + 1, len(lines))
                    if lines[candidate].strip() == f"endlabel {name}"
                ),
                None,
            )
            if end is None:
                raise BundleError(f"{path.relative_to(ROOT)}: {name} has no endlabel")
            instruction_rows = [
                instruction
                for line in lines[index : end + 1]
                if (instruction := INSTRUCTION.match(line)) is not None
            ]
            if not instruction_rows:
                raise BundleError(f"{path.relative_to(ROOT)}: {name} has no instructions")
            address = int(instruction_rows[0].group("address"), 16)
            words = b"".join(
                bytes.fromhex(instruction.group("word"))
                for instruction in instruction_rows
            )
            if len(words) != size:
                raise BundleError(
                    f"{path.relative_to(ROOT)}: {name} has {len(words):#x} "
                    f"instruction bytes, expected {size:#x}"
                )
            if address in blocks:
                raise BundleError(f"duplicate assembly function at {address:#010x}")
            text = "\n".join([*preamble, *lines[index : end + 1]]) + "\n"
            blocks[address] = AssemblyBlock(path, name, address, size, text, words)
            index = end + 1
    return blocks


def build_metadata(
    candidate: Candidate,
    profile: dict[str, object],
    block: AssemblyBlock,
) -> dict[str, object]:
    return {
        "schema": 1,
        "function": {
            "address": f"0x{candidate.address:08X}",
            "size": f"0x{candidate.size:X}",
            "candidate_name": candidate.note_name,
            "inventory_name": candidate.inventory_name,
            "target_name": block.name,
        },
        "candidate_note": candidate.note_path.relative_to(ROOT).as_posix(),
        "profile": candidate.profile_name,
        "compiler": profile.get("compiler"),
        "compiler_flags": profile.get("compiler_flags"),
        "assembly_filter": profile.get("assembly_filter"),
        "aspsx_version": profile.get("aspsx_version"),
        "maspsx_flags": profile.get("maspsx_flags"),
        "data_limit": profile.get("data_limit"),
        "assembler": "tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as",
        "assembler_flags": [
            "-EL",
            "-mips1",
            f"-G{profile.get('data_limit')}",
        ],
        "candidate_sha256": sha256(candidate.source.encode()),
        "target_instruction_count": candidate.size // 4,
        "target_bytes_sha256": sha256(block.words),
    }


def validate_target(text: str, metadata: dict[str, object], path: Path) -> None:
    function = metadata["function"]
    if not isinstance(function, dict):
        raise BundleError(f"{path.relative_to(ROOT)}: function metadata is invalid")
    expected_name = function["target_name"]
    expected_size = int(str(function["size"]), 0)
    expected_address = int(str(function["address"]), 0)
    declaration = next(
        (
            FUNCTION.fullmatch(line)
            for line in text.splitlines()
            if FUNCTION.fullmatch(line) is not None
        ),
        None,
    )
    if declaration is None:
        raise BundleError(f"{path.relative_to(ROOT)}: target has no function declaration")
    if (
        declaration.group("name") != expected_name
        or int(declaration.group("size"), 16) != expected_size
    ):
        raise BundleError(f"{path.relative_to(ROOT)}: target declaration disagrees")
    instructions = [
        match
        for line in text.splitlines()
        if (match := INSTRUCTION.match(line)) is not None
    ]
    if len(instructions) != expected_size // 4:
        raise BundleError(f"{path.relative_to(ROOT)}: target instruction count differs")
    addresses = [int(match.group("address"), 16) for match in instructions]
    if addresses != list(range(expected_address, expected_address + expected_size, 4)):
        raise BundleError(f"{path.relative_to(ROOT)}: target addresses are not contiguous")
    words = b"".join(bytes.fromhex(match.group("word")) for match in instructions)
    if sha256(words) != metadata["target_bytes_sha256"]:
        raise BundleError(f"{path.relative_to(ROOT)}: target byte hash differs")
    if f"endlabel {expected_name}" not in text.splitlines():
        raise BundleError(f"{path.relative_to(ROOT)}: target endlabel differs")


def tracked_target_block(text: str, candidate: Candidate) -> AssemblyBlock:
    lines = text.splitlines()
    declaration = next(
        (
            FUNCTION.fullmatch(line)
            for line in lines
            if FUNCTION.fullmatch(line) is not None
        ),
        None,
    )
    if declaration is None:
        raise BundleError(f"{candidate.key}: target has no function declaration")
    instructions = [
        match
        for line in lines
        if (match := INSTRUCTION.match(line)) is not None
    ]
    if not instructions:
        raise BundleError(f"{candidate.key}: target has no instructions")
    return AssemblyBlock(
        path=candidate.directory / "target.S",
        name=declaration.group("name"),
        address=int(instructions[0].group("address"), 16),
        size=int(declaration.group("size"), 16),
        text=text,
        words=b"".join(
            bytes.fromhex(instruction.group("word"))
            for instruction in instructions
        ),
    )


def expected_files(
    candidate: Candidate,
    profiles: dict[str, dict[str, object]],
    blocks: dict[int, AssemblyBlock],
) -> dict[str, str]:
    block = blocks.get(candidate.address)
    if block is None:
        raise BundleError(f"no generated assembly at {candidate.address:#010x}")
    if block.size != candidate.size:
        raise BundleError(
            f"{candidate.key}: target size {block.size:#x}, inventory "
            f"{candidate.size:#x}"
        )
    metadata = build_metadata(candidate, profiles[candidate.profile_name], block)
    return {
        "candidate.c": candidate.source,
        "target.S": block.text,
        "build.json": json.dumps(metadata, indent=2) + "\n",
    }


def write_bundles(
    candidates: list[Candidate],
    profiles: dict[str, dict[str, object]],
) -> None:
    blocks = assembly_blocks()
    BUNDLE_DIRECTORY.mkdir(parents=True, exist_ok=True)
    wanted = {candidate.key for candidate in candidates}
    for path in BUNDLE_DIRECTORY.iterdir():
        if path.is_dir() and path.name.startswith("func_") and path.name not in wanted:
            shutil.rmtree(path)
    for candidate in candidates:
        candidate.directory.mkdir(parents=True, exist_ok=True)
        for name, content in expected_files(candidate, profiles, blocks).items():
            path = candidate.directory / name
            if not path.is_file() or path.read_text(encoding="utf-8") != content:
                path.write_text(content, encoding="utf-8")
    print(f"human candidate bundles: wrote {len(candidates)}")


def check_bundles(
    candidates: list[Candidate],
    profiles: dict[str, dict[str, object]],
) -> None:
    expected_directories = {candidate.key for candidate in candidates}
    actual_directories = {
        path.name
        for path in BUNDLE_DIRECTORY.iterdir()
        if path.is_dir() and path.name.startswith("func_")
    }
    if actual_directories != expected_directories:
        raise BundleError(
            "bundle directories differ: "
            f"missing={sorted(expected_directories - actual_directories)}, "
            f"extra={sorted(actual_directories - expected_directories)}"
        )
    blocks = assembly_blocks() if ASSEMBLY_ROOT.is_dir() else None
    for candidate in candidates:
        expected_names = {"candidate.c", "target.S", "build.json"}
        actual_names = {path.name for path in candidate.directory.iterdir()}
        if actual_names != expected_names:
            raise BundleError(
                f"{candidate.directory.relative_to(ROOT)}: expected "
                f"{sorted(expected_names)}, found {sorted(actual_names)}"
            )
        source_path = candidate.directory / "candidate.c"
        source = source_path.read_text(encoding="utf-8")
        if source != candidate.source:
            raise BundleError(
                f"{source_path.relative_to(ROOT)}: candidate source is stale"
            )
        metadata_path = candidate.directory / "build.json"
        metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
        target_path = candidate.directory / "target.S"
        target = target_path.read_text(encoding="utf-8")
        tracked_block = tracked_target_block(target, candidate)
        if tracked_block.name != candidate.inventory_name:
            raise BundleError(
                f"{target_path.relative_to(ROOT)}: target name "
                f"{tracked_block.name} differs from inventory name "
                f"{candidate.inventory_name}"
            )
        expected_metadata = build_metadata(
            candidate,
            profiles[candidate.profile_name],
            tracked_block,
        )
        if metadata != expected_metadata:
            raise BundleError(
                f"{metadata_path.relative_to(ROOT)}: build metadata is stale"
            )
        validate_target(target, metadata, target_path)
        if blocks is not None:
            expected = expected_files(candidate, profiles, blocks)
            for name, content in expected.items():
                path = candidate.directory / name
                if path.read_text(encoding="utf-8") != content:
                    raise BundleError(
                        f"{path.relative_to(ROOT)}: generated content is stale"
                    )
    print(f"human candidate bundles: OK ({len(candidates)})")


def compile_bundles(
    candidates: list[Candidate],
    profiles: dict[str, dict[str, object]],
) -> None:
    from build_baseline import compile_c, tool

    assembler = tool(ROOT, "as")
    for candidate in candidates:
        compile_c(
            ROOT,
            assembler,
            {
                "source": candidate.directory.joinpath("candidate.c")
                .relative_to(ROOT)
                .as_posix(),
                "profile": candidate.profile_name,
                "object": f"{candidate.key}.o",
            },
            profiles,
            object_directory="tmp/candidate-human-build/obj",
            asm_directory="tmp/candidate-human-build/asm",
        )
    print(f"human candidate bundle builds: OK ({len(candidates)})")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--compile-check", action="store_true")
    args = parser.parse_args()
    try:
        profiles = load_profiles()
        candidates = load_candidates(profiles, load_inventory())
        if args.compile_check:
            check_bundles(candidates, profiles)
            compile_bundles(candidates, profiles)
        elif args.check:
            check_bundles(candidates, profiles)
        else:
            write_bundles(candidates, profiles)
    except (
        BundleError,
        OSError,
        UnicodeError,
        ValueError,
        KeyError,
        TypeError,
        json.JSONDecodeError,
        csv.Error,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
