#!/usr/bin/env python3
"""Validate and build tracked, non-linked resident C candidates."""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

from build_baseline import BuildError, compile_c, tool
from workspace import WorkspaceError, local_environment, require_workspace_root


ROOT = Path(__file__).resolve().parents[2]
CONFIG_PATH = ROOT / "config/slus_01411/candidates.json"
PROFILE_PATH = ROOT / "config/slus_01411/compiler_profiles.json"
INVENTORY_PATH = ROOT / "config/slus_01411/functions.csv"
SOURCE_DIRECTORY = ROOT / "src/candidates"
TARGET_DIRECTORY = ROOT / "src/candidates_target"
BUILD_DIRECTORY = ROOT / "tmp/candidate-build"
TARGET_ELF = ROOT / "tmp/project-build/SLUS_014.11.elf"

FUNCTION = re.compile(
    r"^nonmatching\s+(?P<name>\S+),\s+0x(?P<size>[0-9A-Fa-f]+)$"
)
INSTRUCTION = re.compile(
    r"^\s*/\*\s+[0-9A-Fa-f]+\s+"
    r"(?P<address>[0-9A-Fa-f]{8})\s+"
    r"(?P<word>[0-9A-Fa-f]{8})\s+\*/"
)
SHA256 = re.compile(r"^[0-9a-f]{64}$")
SECTION = re.compile(
    r"^\s*\d+\s+(?P<name>\.\S+)\s+(?P<size>[0-9A-Fa-f]+)\s+"
)
RELOCATION = re.compile(
    r"^\s*(?P<offset>[0-9A-Fa-f]{8})\s+"
    r"(?P<kind>R_MIPS_\S+)\s+(?P<value>\S+)\s*$"
)


class CandidateBuildError(RuntimeError):
    pass


@dataclass(frozen=True)
class Candidate:
    address: int
    name: str
    size: int
    profile: str
    source: Path
    target: Path
    candidate_build_sha256: str
    target_bytes_sha256: str

    @property
    def key(self) -> str:
        return f"func_{self.address:08X}"


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def load_profiles() -> dict[str, dict[str, object]]:
    configuration = json.loads(PROFILE_PATH.read_text(encoding="utf-8"))
    if configuration.get("schema") != 1:
        raise CandidateBuildError(
            f"{PROFILE_PATH.relative_to(ROOT)}: unsupported schema"
        )
    profiles = configuration.get("profiles")
    if not isinstance(profiles, dict):
        raise CandidateBuildError(
            f"{PROFILE_PATH.relative_to(ROOT)}: profiles is not an object"
        )
    return profiles


def load_inventory() -> dict[int, dict[str, str]]:
    with INVENTORY_PATH.open(encoding="utf-8", newline="") as handle:
        return {
            int(row["address"], 0): row
            for row in csv.DictReader(handle)
        }


def configured_path(value: object, directory: Path, suffix: str) -> Path:
    if not isinstance(value, str):
        raise CandidateBuildError("candidate path must be a string")
    path = (ROOT / value).resolve()
    try:
        path.relative_to(directory.resolve())
    except ValueError as error:
        raise CandidateBuildError(
            f"candidate path escapes {directory.relative_to(ROOT)}"
        ) from error
    if path.suffix != suffix:
        raise CandidateBuildError(f"{value}: expected a {suffix} file")
    if not path.is_file():
        raise CandidateBuildError(f"{value}: file is missing")
    return path


def target_words(candidate: Candidate) -> bytes:
    lines = candidate.target.read_text(encoding="utf-8").splitlines()
    declaration = next(
        (match for line in lines if (match := FUNCTION.fullmatch(line)) is not None),
        None,
    )
    if declaration is None:
        raise CandidateBuildError(
            f"{candidate.target.relative_to(ROOT)}: "
            "target has no function declaration"
        )
    if declaration.group("name") != candidate.name:
        raise CandidateBuildError(
            f"{candidate.target.relative_to(ROOT)}: "
            "target name differs from inventory"
        )
    if int(declaration.group("size"), 16) != candidate.size:
        raise CandidateBuildError(
            f"{candidate.target.relative_to(ROOT)}: "
            "target size differs from inventory"
        )
    instructions = [
        match
        for line in lines
        if (match := INSTRUCTION.match(line)) is not None
    ]
    if len(instructions) != candidate.size // 4:
        raise CandidateBuildError(
            f"{candidate.target.relative_to(ROOT)}: "
            "target instruction count differs"
        )
    addresses = [int(match.group("address"), 16) for match in instructions]
    expected = list(range(candidate.address, candidate.address + candidate.size, 4))
    if addresses != expected:
        raise CandidateBuildError(
            f"{candidate.target.relative_to(ROOT)}: "
            "target addresses are not contiguous"
        )
    if f"endlabel {candidate.name}" not in lines:
        raise CandidateBuildError(
            f"{candidate.target.relative_to(ROOT)}: target endlabel differs"
        )
    return b"".join(
        bytes.fromhex(instruction.group("word"))
        for instruction in instructions
    )


def load_candidates() -> tuple[list[Candidate], dict[str, dict[str, object]]]:
    configuration = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
    if configuration.get("schema") != 1:
        raise CandidateBuildError(
            f"{CONFIG_PATH.relative_to(ROOT)}: unsupported schema"
        )
    items = configuration.get("candidates")
    if not isinstance(items, list) or not items:
        raise CandidateBuildError(
            f"{CONFIG_PATH.relative_to(ROOT)}: "
            "candidates must be a non-empty list"
        )

    profiles = load_profiles()
    inventory = load_inventory()
    candidates: list[Candidate] = []
    seen_addresses: set[int] = set()
    configured_sources: set[str] = set()
    configured_targets: set[str] = set()

    for item in items:
        if not isinstance(item, dict):
            raise CandidateBuildError("candidate entry must be an object")
        address_value = item.get("address")
        if not isinstance(address_value, str):
            raise CandidateBuildError("candidate address must be a string")
        try:
            address = int(address_value, 0)
        except ValueError as error:
            raise CandidateBuildError(
                f"invalid candidate address {address_value}"
            ) from error
        if address in seen_addresses:
            raise CandidateBuildError(f"duplicate candidate address {address:#010x}")
        seen_addresses.add(address)

        row = inventory.get(address)
        if row is None:
            raise CandidateBuildError(f"{address:#010x}: absent from functions.csv")
        if row["module"] != "game" or row["status"] != "unmatched_asm":
            raise CandidateBuildError(
                f"{address:#010x}: candidate must be unmatched game code"
            )
        profile = item.get("profile")
        if not isinstance(profile, str) or profile not in profiles:
            raise CandidateBuildError(f"{address:#010x}: unknown profile {profile}")

        source = configured_path(item.get("source"), SOURCE_DIRECTORY, ".c")
        target = configured_path(item.get("target"), TARGET_DIRECTORY, ".S")
        key = f"func_{address:08X}"
        if source.name != f"{key}.c" or target.name != f"{key}.S":
            raise CandidateBuildError(
                f"{address:#010x}: source and target filenames must use {key}"
            )
        source_relative = source.relative_to(ROOT).as_posix()
        target_relative = target.relative_to(ROOT).as_posix()
        configured_sources.add(source_relative)
        configured_targets.add(target_relative)

        candidate_hash = item.get("candidate_build_sha256")
        target_hash = item.get("target_bytes_sha256")
        if (
            not isinstance(candidate_hash, str)
            or SHA256.fullmatch(candidate_hash) is None
        ):
            raise CandidateBuildError(
                f"{address:#010x}: invalid candidate build hash"
            )
        if not isinstance(target_hash, str) or SHA256.fullmatch(target_hash) is None:
            raise CandidateBuildError(f"{address:#010x}: invalid target byte hash")

        candidate = Candidate(
            address=address,
            name=row["name"],
            size=int(row["size"], 0),
            profile=profile,
            source=source,
            target=target,
            candidate_build_sha256=candidate_hash,
            target_bytes_sha256=target_hash,
        )
        source_text = source.read_text(encoding="utf-8")
        if re.search(
            rf"\b{re.escape(candidate.name)}\s*\([^;{{}}]*\)\s*\{{",
            source_text,
        ) is None:
            raise CandidateBuildError(
                f"{source_relative}: does not define {candidate.name}"
            )
        if sha256(target_words(candidate)) != candidate.target_bytes_sha256:
            raise CandidateBuildError(f"{target_relative}: target byte hash differs")

        note = ROOT / f"notes/candidates/{key}.md"
        bundle = ROOT / f"notes/candidates/for_humans/{key}"
        if note.exists() or bundle.exists():
            raise CandidateBuildError(
                f"{key}: build-integrated candidates cannot retain note bundles"
            )
        candidates.append(candidate)

    actual_sources = {
        path.relative_to(ROOT).as_posix()
        for path in SOURCE_DIRECTORY.glob("func_*.c")
    }
    actual_targets = {
        path.relative_to(ROOT).as_posix()
        for path in TARGET_DIRECTORY.glob("func_*.S")
    }
    if actual_sources != configured_sources:
        raise CandidateBuildError(
            "candidate sources differ: "
            f"missing={sorted(configured_sources - actual_sources)}, "
            f"extra={sorted(actual_sources - configured_sources)}"
        )
    if actual_targets != configured_targets:
        raise CandidateBuildError(
            "candidate targets differ: "
            f"missing={sorted(configured_targets - actual_targets)}, "
            f"extra={sorted(actual_targets - configured_targets)}"
        )
    return candidates, profiles


def run_output(command: list[str]) -> str:
    environment = os.environ.copy()
    environment.update(local_environment(ROOT))
    try:
        completed = subprocess.run(
            command,
            cwd=ROOT,
            env=environment,
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
    except subprocess.CalledProcessError as error:
        detail = error.stderr.strip() or error.stdout.strip()
        raise CandidateBuildError(
            f"command failed with exit code {error.returncode}: "
            f"{command[0]}: {detail}"
        ) from error
    return completed.stdout


def undefined_symbols(nm: Path, object_path: Path) -> set[str]:
    return {
        line.split()[-1]
        for line in run_output([str(nm), "-u", str(object_path)]).splitlines()
        if line.split()
    }


def defined_symbols(nm: Path, elf_path: Path) -> set[str]:
    symbols: set[str] = set()
    for line in run_output([str(nm), str(elf_path)]).splitlines():
        fields = line.split()
        if len(fields) >= 3 and fields[-2] != "U":
            symbols.add(fields[-1])
    return symbols


def candidate_build_hash(
    objcopy: Path,
    objdump: Path,
    object_path: Path,
    candidate: Candidate,
) -> str:
    sections = {
        match.group("name"): int(match.group("size"), 16)
        for line in run_output([str(objdump), "-h", str(object_path)]).splitlines()
        if (match := SECTION.match(line)) is not None
    }
    unsupported = {
        name: sections.get(name, 0)
        for name in (
            ".data",
            ".rodata",
            ".rdata",
            ".sdata",
            ".bss",
            ".sbss",
            ".lit4",
            ".lit8",
        )
        if sections.get(name, 0) != 0
    }
    if unsupported:
        raise CandidateBuildError(
            f"{candidate.key}: candidate has allocated non-text sections "
            f"{unsupported}"
        )

    text_path = BUILD_DIRECTORY / "fingerprints" / f"{candidate.key}.text"
    text_path.parent.mkdir(parents=True, exist_ok=True)
    run_output(
        [
            str(objcopy),
            "-O",
            "binary",
            "--only-section=.text",
            str(object_path),
            str(text_path),
        ]
    )
    relocations = [
        " ".join(match.groups())
        for line in run_output(
            [str(objdump), "-r", "--section=.text", str(object_path)]
        ).splitlines()
        if (match := RELOCATION.match(line)) is not None
    ]
    payload = (
        b"candidate-build-v1\0"
        + text_path.read_bytes()
        + b"\0"
        + "\n".join(relocations).encode("ascii")
        + b"\n"
    )
    return sha256(payload)


def build_candidates(
    candidates: list[Candidate],
    profiles: dict[str, dict[str, object]],
    print_hashes: bool,
) -> None:
    if not TARGET_ELF.is_file():
        raise CandidateBuildError(
            f"{TARGET_ELF.relative_to(ROOT)} is absent; "
            "run the normal build first"
        )
    shutil.rmtree(BUILD_DIRECTORY, ignore_errors=True)
    assembler = tool(ROOT, "as")
    nm = tool(ROOT, "nm")
    objcopy = tool(ROOT, "objcopy")
    objdump = tool(ROOT, "objdump")
    target_symbols = defined_symbols(nm, TARGET_ELF)

    for candidate in candidates:
        object_path = compile_c(
            ROOT,
            assembler,
            {
                "source": candidate.source.relative_to(ROOT).as_posix(),
                "profile": candidate.profile,
                "object": f"{candidate.key}.o",
            },
            profiles,
            object_directory="tmp/candidate-build/obj",
            asm_directory="tmp/candidate-build/asm",
        )
        build_hash = candidate_build_hash(
            objcopy,
            objdump,
            object_path,
            candidate,
        )
        missing_symbols = sorted(
            undefined_symbols(nm, object_path) - target_symbols
        )
        if missing_symbols:
            raise CandidateBuildError(
                f"{candidate.key}: undefined symbols absent from target: "
                f"{missing_symbols}"
            )
        if print_hashes:
            print(f"{candidate.key} candidate_build_sha256={build_hash}")
        elif build_hash != candidate.candidate_build_sha256:
            raise CandidateBuildError(
                f"{candidate.key}: candidate build hash differs: {build_hash}"
            )

    if not print_hashes:
        print(f"candidate builds: OK ({len(candidates)})")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--print-hashes", action="store_true")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        if root != ROOT:
            raise CandidateBuildError(f"unexpected repository root: {root}")
        candidates, profiles = load_candidates()
        if args.check:
            print(f"candidate sources: OK ({len(candidates)})")
        else:
            build_candidates(candidates, profiles, args.print_hashes)
    except (
        CandidateBuildError,
        BuildError,
        WorkspaceError,
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
