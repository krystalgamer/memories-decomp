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
from workspace import local_environment


ROOT = Path(__file__).resolve().parents[2]
CONFIG_PATH = ROOT / "config/slus_01411/candidates.json"
PROFILE_PATH = ROOT / "config/slus_01411/compiler_profiles.json"
INVENTORY_PATH = ROOT / "config/slus_01411/functions.csv"
SOURCE_DIRECTORY = ROOT / "src/candidates"
TARGET_DIRECTORY = ROOT / "src/candidates_target"
BUILD_DIRECTORY = ROOT / "tmp/candidate-build"
TARGET_ELF = ROOT / "tmp/project-build/SLUS_014.11.elf"
OVERLAY_INVENTORY_DIRECTORY = ROOT / "config/slus_01411/overlays"

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
IDENTIFIER = re.compile(r"^[A-Za-z_]\w*$")
ASM_ALIAS = re.compile(
    r'\basm\s*\(\s*"(?P<name>[A-Za-z_]\w*)"\s*\)'
)
LOCAL_INCLUDE = re.compile(r'^\s*#\s*include\s+"(?P<path>[^"]+)"', re.MULTILINE)
EXCLUDED_HEADER_DIRECTORIES = {
    "candidates",
    "candidates_target",
    "overlays",
}


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
    canonical_contract_sha256: str
    canonical_contracts: dict[str, str]
    # None for a resident candidate; the overlay module name otherwise. An
    # overlay candidate lives in src/candidates/<module>/, is validated
    # against that module's inventory and headers, and is compiled by the
    # overlay build, whose ELF is what its undefined symbols must resolve in.
    module: str | None = None

    @property
    def key(self) -> str:
        return f"func_{self.address:08X}"

    @property
    def label(self) -> str:
        """The key, qualified by module for an overlay candidate."""
        return self.key if self.module is None else f"{self.module}/{self.key}"


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def strip_c_comments(text: str) -> str:
    output: list[str] = []
    index = 0
    quote: str | None = None
    escaped = False
    while index < len(text):
        char = text[index]
        follow = text[index + 1] if index + 1 < len(text) else ""
        if quote is not None:
            output.append(char)
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            index += 1
            continue
        if char in ('"', "'"):
            quote = char
            output.append(char)
            index += 1
            continue
        if char == "/" and follow == "*":
            output.extend((" ", " "))
            index += 2
            while index < len(text):
                char = text[index]
                follow = text[index + 1] if index + 1 < len(text) else ""
                if char == "*" and follow == "/":
                    output.extend((" ", " "))
                    index += 2
                    break
                output.append("\n" if char == "\n" else " ")
                index += 1
            continue
        if char == "/" and follow == "/":
            output.extend((" ", " "))
            index += 2
            while index < len(text) and text[index] != "\n":
                output.append(" ")
                index += 1
            continue
        output.append(char)
        index += 1
    return "".join(output)


def normalized_statement(text: str) -> str:
    lines = [
        line
        for line in text.splitlines()
        if not line.lstrip().startswith("#")
    ]
    return " ".join(" ".join(lines).split())


def top_level_statements(text: str) -> list[str]:
    text = strip_c_comments(text)
    statements: list[str] = []
    start = 0
    depth = 0
    braces: list[bool] = []
    quote: str | None = None
    escaped = False
    for index, char in enumerate(text):
        if quote is not None:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            continue
        if char in ('"', "'"):
            quote = char
            continue
        if char == "{":
            prefix = normalized_statement(text[start:index])
            transparent = bool(
                re.search(r'\bextern\s+"C"\s*$', prefix)
            )
            braces.append(transparent)
            if transparent:
                start = index + 1
            else:
                depth += 1
            continue
        if char == "}":
            transparent = braces.pop() if braces else False
            if not transparent and depth > 0:
                depth -= 1
            if transparent or depth == 0:
                start = index + 1
            continue
        if char != ";" or depth != 0:
            continue
        statement = normalized_statement(text[start:index + 1])
        if statement:
            statements.append(statement)
        start = index + 1
    return statements


def extern_symbol(statement: str) -> str:
    if not statement.startswith("extern "):
        raise CandidateBuildError(
            f"not an extern declaration: {statement}"
        )
    alias = ASM_ALIAS.search(statement)
    if alias is not None:
        return alias.group("name")
    declaration = re.split(
        r"\b(?:asm|__attribute__)\s*\(",
        statement,
        maxsplit=1,
    )[0].rstrip()
    pointer = re.search(
        r"\(\s*\*\s*(?P<name>[A-Za-z_]\w*)\s*\)",
        declaration,
    )
    if pointer is not None:
        return pointer.group("name")
    function = re.search(
        r"\b(?P<name>[A-Za-z_]\w*)\s*\(",
        declaration,
    )
    if function is not None:
        return function.group("name")
    declaration = declaration.rstrip(";").rstrip()
    declaration = re.sub(r"(?:\[[^\]]*\]\s*)+$", "", declaration)
    name = re.search(r"(?P<name>[A-Za-z_]\w*)\s*$", declaration)
    if name is None:
        raise CandidateBuildError(
            f"cannot find extern symbol in: {statement}"
        )
    return name.group("name")


def candidate_extern_symbols(text: str) -> list[str]:
    symbols = {
        extern_symbol(statement)
        for statement in top_level_statements(text)
        if statement.startswith("extern ")
    }
    return sorted(symbols)


def declaration_identifier(statement: str) -> str:
    declaration = re.split(r"\basm\s*\(", statement, maxsplit=1)[0].rstrip()
    pointer = re.search(
        r"\(\s*\*\s*(?P<name>[A-Za-z_]\w*)\s*\)",
        declaration,
    )
    if pointer is not None:
        return pointer.group("name")
    function = re.search(
        r"\b(?P<name>[A-Za-z_]\w*)\s*\(",
        declaration,
    )
    if function is not None:
        return function.group("name")
    declaration = declaration.rstrip(";").rstrip()
    declaration = re.sub(r"(?:\[[^\]]*\]\s*)+$", "", declaration)
    name = re.search(r"(?P<name>[A-Za-z_]\w*)\s*$", declaration)
    if name is not None:
        return name.group("name")
    raise CandidateBuildError(
        f"cannot find declaration identifier in: {statement}"
    )


def candidate_contract_symbols(source: Path, text: str) -> list[str]:
    symbols = set(candidate_extern_symbols(text))
    identifiers = set(re.findall(r"\b[A-Za-z_]\w*\b", text))
    pending = [source]
    visited: set[Path] = set()
    while pending:
        path = pending.pop()
        if path in visited:
            continue
        visited.add(path)
        contents = path.read_text(encoding="utf-8", errors="surrogateescape")
        for match in LOCAL_INCLUDE.finditer(contents):
            included = (path.parent / match.group("path")).resolve()
            if included.is_relative_to(ROOT) and included.is_file():
                pending.append(included)
        if path == source:
            continue
        for statement in top_level_statements(contents):
            if not statement.startswith("extern ") or ASM_ALIAS.search(statement) is None:
                continue
            if declaration_identifier(statement) in identifiers:
                symbols.add(extern_symbol(statement))
    return sorted(symbols)


def canonical_declaration_index(
    symbols: set[str],
    header_root: Path | None = None,
    overlay_module: str | None = None,
) -> dict[str, list[tuple[str, str]]]:
    """Canonical declarations of symbols, by header.

    Resident candidates see only resident headers. An overlay candidate also
    sees its own module's headers under overlays/<module>/, and no other
    module's, because those are the headers its source can include.
    """
    header_root = header_root or ROOT / "src"
    index = {symbol: [] for symbol in symbols}
    for path in sorted(header_root.rglob("*.h")):
        relative_path = path.relative_to(header_root)
        parts = relative_path.parts
        own_overlay = (
            overlay_module is not None
            and len(parts) > 2
            and parts[0] == "overlays"
            and parts[1] == overlay_module
        )
        if parts and parts[0] in EXCLUDED_HEADER_DIRECTORIES and not own_overlay:
            continue
        relative = relative_path.as_posix()
        for statement in top_level_statements(
            path.read_text(encoding="utf-8", errors="surrogateescape")
        ):
            statement_symbols = symbols & set(
                re.findall(r"\b[A-Za-z_]\w*\b", statement)
            )
            for symbol in statement_symbols:
                index[symbol].append((relative, statement))
    for declarations in index.values():
        declarations.sort()
    return index


def canonical_contract_sites(
    symbols: list[str],
    declaration_index: dict[str, list[tuple[str, str]]],
) -> dict[str, list[str]]:
    return {
        symbol: sorted(
            {
                path
                for path, _ in declaration_index.get(symbol, [])
            }
        )
        for symbol in sorted(symbols)
    }


def canonical_symbol_contract_hash(
    symbol: str,
    declarations: list[tuple[str, str]],
) -> str:
    payload = json.dumps(
        {
            "symbol": symbol,
            "declarations": [
                {"path": path, "statement": statement}
                for path, statement in declarations
            ],
        },
        sort_keys=True,
        separators=(",", ":"),
    ).encode("utf-8")
    return sha256(b"candidate-contract-symbol-v1\0" + payload)


def canonical_contract_hashes(
    symbols: list[str],
    declaration_index: dict[str, list[tuple[str, str]]],
) -> dict[str, str]:
    return {
        symbol: canonical_symbol_contract_hash(
            symbol,
            declaration_index.get(symbol, []),
        )
        for symbol in sorted(symbols)
    }


def canonical_contract_hash(contracts: dict[str, str]) -> str:
    payload = json.dumps(
        contracts,
        sort_keys=True,
        separators=(",", ":"),
    ).encode("ascii")
    return sha256(b"candidate-contract-v1\0" + payload)


def canonical_contract_difference(
    expected: dict[str, str],
    actual: dict[str, str],
) -> tuple[list[str], list[str], list[str]]:
    expected_names = set(expected)
    actual_names = set(actual)
    added = sorted(actual_names - expected_names)
    removed = sorted(expected_names - actual_names)
    changed = sorted(
        name
        for name in expected_names & actual_names
        if expected[name] != actual[name]
    )
    return added, removed, changed


def validate_canonical_contract_metadata(
    address: int,
    configured_contract_hash: object,
    configured_contracts: object,
    contracts: dict[str, str],
    contract_sites: dict[str, list[str]],
) -> None:
    if (
        not isinstance(configured_contract_hash, str)
        or SHA256.fullmatch(configured_contract_hash) is None
    ):
        raise CandidateBuildError(
            f"{address:#010x}: invalid canonical contract hash"
        )
    if not isinstance(configured_contracts, dict):
        raise CandidateBuildError(
            f"{address:#010x}: canonical contracts must be an object"
        )
    if any(
        not isinstance(name, str)
        or IDENTIFIER.fullmatch(name) is None
        or not isinstance(value, str)
        or SHA256.fullmatch(value) is None
        for name, value in configured_contracts.items()
    ):
        raise CandidateBuildError(
            f"{address:#010x}: invalid canonical contract entry"
        )
    added, removed, changed = canonical_contract_difference(
        configured_contracts,
        contracts,
    )
    if added or removed or changed:
        affected = added + removed + changed
        current_sites = {
            name: contract_sites.get(name, [])
            for name in affected
        }
        raise CandidateBuildError(
            f"{address:#010x}: canonical contracts differ: "
            f"added={added}, removed={removed}, changed={changed}, "
            f"current_sites={current_sites}"
        )
    contract_hash = canonical_contract_hash(contracts)
    if configured_contract_hash != contract_hash:
        raise CandidateBuildError(
            f"{address:#010x}: canonical contract hash differs: "
            f"{contract_hash}"
        )


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


def load_inventory(module: str | None = None) -> dict[int, dict[str, str]]:
    path = (
        INVENTORY_PATH
        if module is None
        else OVERLAY_INVENTORY_DIRECTORY / f"{module}_functions.csv"
    )
    if not path.is_file():
        raise CandidateBuildError(
            f"{path.relative_to(ROOT)}: no inventory for module {module}"
        )
    with path.open(encoding="utf-8", newline="") as handle:
        return {
            int(row["address"], 0): row
            for row in csv.DictReader(handle)
        }


def candidate_module(item: dict[str, object]) -> str | None:
    module = item.get("module")
    if module is None:
        return None
    if not isinstance(module, str) or IDENTIFIER.fullmatch(module) is None:
        raise CandidateBuildError(f"invalid candidate module {module!r}")
    return module


def candidate_directory(root: Path, module: str | None) -> Path:
    return root if module is None else root / module


def overlay_elf(module: str) -> Path:
    return ROOT / f"tmp/overlays/{module}/build/{module}.elf"


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


def note_candidate_paths(root: Path) -> list[str]:
    return sorted(
        path.relative_to(root).as_posix()
        for directory in (
            root / "notes/candidates",
            root / "notes/overlays/candidates",
        )
        for pattern in ("func_*.md", "for_humans/func_*")
        for path in directory.glob(pattern)
    )


def load_candidates(
    verify_contracts: bool = True,
) -> tuple[list[Candidate], dict[str, dict[str, object]]]:
    note_candidates = note_candidate_paths(ROOT)
    if note_candidates:
        raise CandidateBuildError(
            "candidates must be build-integrated under src/candidates: "
            f"{note_candidates}"
        )

    configuration = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
    schema = configuration.get("schema")
    if schema not in (1, 2):
        raise CandidateBuildError(
            f"{CONFIG_PATH.relative_to(ROOT)}: unsupported schema"
        )
    if verify_contracts and schema != 2:
        raise CandidateBuildError(
            f"{CONFIG_PATH.relative_to(ROOT)}: "
            "schema 2 canonical contract metadata is required"
        )
    items = configuration.get("candidates")
    if not isinstance(items, list) or not items:
        raise CandidateBuildError(
            f"{CONFIG_PATH.relative_to(ROOT)}: "
            "candidates must be a non-empty list"
        )

    profiles = load_profiles()
    inventories: dict[str | None, dict[int, dict[str, str]]] = {}
    candidates: list[Candidate] = []
    seen_addresses: set[tuple[str | None, int]] = set()
    configured_sources: set[str] = set()
    configured_targets: set[str] = set()
    source_texts: dict[tuple[str | None, int], str] = {}
    source_symbols: dict[tuple[str | None, int], list[str]] = {}

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
        module = candidate_module(item)
        source = configured_path(
            item.get("source"), candidate_directory(SOURCE_DIRECTORY, module), ".c"
        )
        source_text = source.read_text(encoding="utf-8")
        source_texts[(module, address)] = source_text
        source_symbols[(module, address)] = candidate_contract_symbols(
            source,
            source_text,
        )

    declaration_indices: dict[str | None, dict[str, list[tuple[str, str]]]] = {}
    for module in {key[0] for key in source_symbols}:
        module_symbols = {
            symbol
            for key, symbols in source_symbols.items()
            if key[0] == module
            for symbol in symbols
        }
        declaration_indices[module] = canonical_declaration_index(
            module_symbols, overlay_module=module
        )

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
        module = candidate_module(item)
        if (module, address) in seen_addresses:
            raise CandidateBuildError(f"duplicate candidate address {address:#010x}")
        seen_addresses.add((module, address))

        if module not in inventories:
            inventories[module] = load_inventory(module)
        row = inventories[module].get(address)
        if row is None:
            raise CandidateBuildError(
                f"{address:#010x}: absent from "
                f"{'functions.csv' if module is None else module + '_functions.csv'}"
            )
        expected_module = "game" if module is None else f"overlay/{module}"
        if row["module"] != expected_module or row["status"] != "unmatched_asm":
            raise CandidateBuildError(
                f"{address:#010x}: candidate must be unmatched "
                f"{'game' if module is None else module} code"
            )
        profile = item.get("profile")
        if not isinstance(profile, str) or profile not in profiles:
            raise CandidateBuildError(f"{address:#010x}: unknown profile {profile}")

        source = configured_path(
            item.get("source"), candidate_directory(SOURCE_DIRECTORY, module), ".c"
        )
        target = configured_path(
            item.get("target"), candidate_directory(TARGET_DIRECTORY, module), ".S"
        )
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

        contracts = canonical_contract_hashes(
            source_symbols[(module, address)],
            declaration_indices[module],
        )
        contract_sites = canonical_contract_sites(
            source_symbols[(module, address)],
            declaration_indices[module],
        )
        contract_hash = canonical_contract_hash(contracts)
        configured_contract_hash = item.get("canonical_contract_sha256")
        configured_contracts = item.get("canonical_contracts")
        if verify_contracts:
            validate_canonical_contract_metadata(
                address,
                configured_contract_hash,
                configured_contracts,
                contracts,
                contract_sites,
            )

        candidate = Candidate(
            address=address,
            name=row["name"],
            size=int(row["size"], 0),
            profile=profile,
            source=source,
            target=target,
            candidate_build_sha256=candidate_hash,
            target_bytes_sha256=target_hash,
            canonical_contract_sha256=contract_hash,
            canonical_contracts=contracts,
            module=module,
        )
        source_text = source_texts[(module, address)]
        if re.search(
            rf"\b{re.escape(candidate.name)}\s*\([^;{{}}]*\)\s*\{{",
            source_text,
        ) is None:
            raise CandidateBuildError(
                f"{source_relative}: does not define {candidate.name}"
            )
        if sha256(target_words(candidate)) != candidate.target_bytes_sha256:
            raise CandidateBuildError(f"{target_relative}: target byte hash differs")

        candidates.append(candidate)

    actual_sources = {
        path.relative_to(ROOT).as_posix()
        for pattern in ("func_*.c", "*/func_*.c")
        for path in SOURCE_DIRECTORY.glob(pattern)
    }
    actual_targets = {
        path.relative_to(ROOT).as_posix()
        for pattern in ("func_*.S", "*/func_*.S")
        for path in TARGET_DIRECTORY.glob(pattern)
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


def print_contract_hashes(candidates: list[Candidate]) -> None:
    for candidate in candidates:
        contracts = json.dumps(
            candidate.canonical_contracts,
            sort_keys=True,
            separators=(",", ":"),
        )
        print(
            f"{candidate.label} "
            f"canonical_contract_sha256="
            f"{candidate.canonical_contract_sha256} "
            f"canonical_contracts={contracts}"
        )


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


def candidate_fingerprint_payload(
    sections: list[tuple[str, bytes, list[str]]],
) -> bytes:
    if len(sections) == 1 and sections[0][0] == ".text":
        _, content, relocations = sections[0]
        return (
            b"candidate-build-v1\0"
            + content
            + b"\0"
            + "\n".join(relocations).encode("ascii")
            + b"\n"
        )

    payload = bytearray(b"candidate-build-v2\0")
    for name, content, relocations in sections:
        name_bytes = name.encode("ascii")
        relocation_bytes = (
            "\n".join(relocations) + "\n"
        ).encode("ascii")
        payload.extend(len(name_bytes).to_bytes(2, "big"))
        payload.extend(name_bytes)
        payload.extend(len(content).to_bytes(8, "big"))
        payload.extend(content)
        payload.extend(len(relocation_bytes).to_bytes(8, "big"))
        payload.extend(relocation_bytes)
    return bytes(payload)


def object_section_bytes(
    objcopy: Path,
    object_path: Path,
    candidate: Candidate,
    section: str,
) -> bytes:
    section_path = (
        object_path.parent.parent
        / "fingerprints"
        / f"{candidate.key}.{section.removeprefix('.')}"
    )
    section_path.parent.mkdir(parents=True, exist_ok=True)
    section_path.unlink(missing_ok=True)
    run_output(
        [
            str(objcopy),
            "-O",
            "binary",
            f"--only-section={section}",
            str(object_path),
            str(section_path),
        ]
    )
    return section_path.read_bytes()


def object_section_relocations(
    objdump: Path,
    object_path: Path,
    section: str,
) -> list[str]:
    return [
        " ".join(match.groups())
        for line in run_output(
            [str(objdump), "-r", f"--section={section}", str(object_path)]
        ).splitlines()
        if (match := RELOCATION.match(line)) is not None
    ]


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
            f"{candidate.label}: candidate has unsupported allocated sections "
            f"{unsupported}"
        )

    fingerprint_sections = [
        name
        for name in (".text", ".rodata", ".rdata")
        if sections.get(name, 0) != 0
    ]
    section_payloads = []
    for name in fingerprint_sections:
        content = object_section_bytes(
            objcopy,
            object_path,
            candidate,
            name,
        )
        if len(content) != sections[name]:
            raise CandidateBuildError(
                f"{candidate.label}: {name} extraction size differs"
            )
        section_payloads.append(
            (
                name,
                content,
                object_section_relocations(objdump, object_path, name),
            )
        )
    return sha256(candidate_fingerprint_payload(section_payloads))


def build_candidates(
    candidates: list[Candidate],
    profiles: dict[str, dict[str, object]],
    print_hashes: bool,
    overlays: bool = False,
) -> None:
    """Compile each candidate and compare its fingerprint.

    The resident build compiles resident candidates against the resident
    ELF. The overlay build (--overlays) compiles overlay candidates; each
    one's undefined symbols must resolve in its own module's ELF, which
    carries the resident addresses the module references, or in the
    resident ELF when that has been built too.
    """
    selected = [
        candidate
        for candidate in candidates
        if (candidate.module is not None) == overlays
    ]
    assembler = tool(ROOT, "as")
    nm = tool(ROOT, "nm")
    objcopy = tool(ROOT, "objcopy")
    objdump = tool(ROOT, "objdump")
    build_root = ROOT / ("tmp/candidate-build-overlays" if overlays else "tmp/candidate-build")
    shutil.rmtree(build_root, ignore_errors=True)
    symbol_tables: dict[str | None, set[str]] = {}

    def target_symbols(module: str | None) -> set[str]:
        if module not in symbol_tables:
            elves = [TARGET_ELF] if module is None else [overlay_elf(module)]
            if not elves[0].is_file():
                raise CandidateBuildError(
                    f"{elves[0].relative_to(ROOT)} is absent; run the "
                    f"{'normal' if module is None else 'overlay'} build first"
                )
            if module is not None and TARGET_ELF.is_file():
                elves.append(TARGET_ELF)
            symbol_tables[module] = set().union(
                *(defined_symbols(nm, elf) for elf in elves)
            )
        return symbol_tables[module]

    for candidate in selected:
        directory = build_root if candidate.module is None else build_root / candidate.module
        object_path = compile_c(
            ROOT,
            assembler,
            {
                "source": candidate.source.relative_to(ROOT).as_posix(),
                "profile": candidate.profile,
                "object": f"{candidate.key}.o",
            },
            profiles,
            object_directory=(directory / "obj").relative_to(ROOT).as_posix(),
            asm_directory=(directory / "asm").relative_to(ROOT).as_posix(),
        )
        build_hash = candidate_build_hash(
            objcopy,
            objdump,
            object_path,
            candidate,
        )
        missing_symbols = sorted(
            undefined_symbols(nm, object_path) - target_symbols(candidate.module)
        )
        if missing_symbols:
            raise CandidateBuildError(
                f"{candidate.label}: undefined symbols absent from target: "
                f"{missing_symbols}"
            )
        if print_hashes:
            print(f"{candidate.label} candidate_build_sha256={build_hash}")
        elif build_hash != candidate.candidate_build_sha256:
            raise CandidateBuildError(
                f"{candidate.label}: candidate build hash differs: {build_hash}"
            )

    if not print_hashes:
        kind = "overlay candidate builds" if overlays else "candidate builds"
        print(f"{kind}: OK ({len(selected)})")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--print-hashes", action="store_true")
    parser.add_argument("--print-contract-hashes", action="store_true")
    parser.add_argument(
        "--overlays",
        action="store_true",
        help="build overlay candidates against the overlay build instead of "
        "resident candidates against the resident build",
    )
    args = parser.parse_args()
    selected_modes = sum(
        (
            args.check,
            args.print_hashes,
            args.print_contract_hashes,
        )
    )
    if selected_modes > 1:
        parser.error(
            "--check, --print-hashes and --print-contract-hashes "
            "are mutually exclusive"
        )
    try:
        if Path.cwd().resolve() != ROOT:
            raise CandidateBuildError("run this command from the repository root")
        candidates, profiles = load_candidates(
            verify_contracts=not args.print_contract_hashes,
        )
        if args.print_contract_hashes:
            print_contract_hashes(candidates)
        elif args.check:
            print(f"candidate sources: OK ({len(candidates)})")
        else:
            build_candidates(
                candidates, profiles, args.print_hashes, overlays=args.overlays
            )
    except (
        CandidateBuildError,
        BuildError,
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
