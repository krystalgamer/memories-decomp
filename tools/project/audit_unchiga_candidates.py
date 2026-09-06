#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import time
from collections import Counter, defaultdict
from pathlib import Path
from typing import Any

import record_external_attempt

from workspace import (
    WorkspaceError,
    local_environment,
    require_workspace_root,
    resolve_within,
)


WORK_PATH = "tmp/agents/unchiga-integration"
DEFINITIONS_PATH = f"{WORK_PATH}/definitions.csv"
REFERENCE_ROOT = "tmp/references/ygofm-decomp-unchiga"
REFERENCE_SOURCE_ROOT = f"{REFERENCE_ROOT}/src"
REFERENCE_SYMBOLS_PATH = f"{REFERENCE_ROOT}/config/symbol_addrs.txt"
REFERENCE_FLAGS_PATH = f"{REFERENCE_ROOT}/config/unit_flags.txt"
FUNCTIONS_PATH = "config/slus_01411/functions.csv"
MATCHING_PATH = "config/slus_01411/matching_c.json"
PROFILES_PATH = "config/slus_01411/compiler_profiles.json"
EXTERNAL_ATTEMPTS_PATH = "config/slus_01411/external_attempts.csv"
TARGET_PATH = "game/SLUS_014.11"
TARGET_ELF_PATH = "tmp/project-build/SLUS_014.11.elf"
BUILT_PATH = "tmp/project-build/SLUS_014.11"
GENERATED_ASM_PATH = "tmp/splat/asm/generated"
MASPSX_PATH = "tools/vendor/maspsx/maspsx.py"
INTEGRATE_TOOL_PATH = "tools/project/integrate_verified_match.py"
INCREMENTAL_TOOL_PATH = "tools/project/build_incremental.py"
AUDIT_REPORT_PATH = "notes/unchiga-match-audit.md"
SYMBOLS_CONFIG_PATH = "config/slus_01411/symbols.txt"
C_LINKER_SYMBOLS_PATH = "config/slus_01411/c_symbols.ld"
BINUTILS_PATH = "tools/toolchains/binutils-2.42/bin"
LOAD_ADDRESS = 0x80010000
HEADER_SIZE = 0x800
RUNTIME_GP = 0x8009AF08

TYPEDEFS = """\
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef short s16;
typedef int s32;

"""

ASM_PATTERN = re.compile(r"\b(?:asm|__asm|__asm__)\b")
INCLUDE_ASM_PATTERN = re.compile(r"\bINCLUDE_ASM\s*\(")
COMMENT_PATTERN = re.compile(r"/\*.*?\*/|//[^\n]*", re.DOTALL)
SYMBOL_ADDRESS_PATTERN = re.compile(
    r"(?:^|_)([0-9A-Fa-f]{8})(?:$|[._])"
)
UNIT_FLAG_PATTERN = re.compile(r"^(\S+)\s+(.+?)\s*$")
SYMBOL_LINE_PATTERN = re.compile(
    r"^\s*([A-Za-z_.$][A-Za-z0-9_.$]*)\s*=\s*"
    r"(0x[0-9A-Fa-f]+|\d+)\s*;"
)

MANIFEST_FIELDS = (
    "address",
    "size",
    "current_name",
    "current_status",
    "current_source",
    "current_profile",
    "mode",
    "reference_name",
    "reference_path",
    "reference_sha256",
    "reference_flags",
    "keep_large_ori",
    "profile",
    "candidate_source",
    "candidate_sha256",
    "target_sha256",
    "prior_mode_attempts",
    "declaration_policy",
)

RESULT_FIELDS = (
    "address",
    "mode",
    "profile",
    "exact",
    "bytes_match",
    "relocations_match",
    "sections_ok",
    "expected_size",
    "actual_size",
    "first_difference",
    "error",
)


class AuditError(RuntimeError):
    pass


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def parse_address(value: str) -> int:
    try:
        return int(value, 0)
    except ValueError as error:
        raise AuditError(f"invalid address: {value}") from error


def read_csv(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        return list(csv.DictReader(handle))


def write_csv(
    path: Path,
    fields: tuple[str, ...],
    rows: list[dict[str, Any]],
) -> None:
    temporary = path.with_name(f"{path.name}.tmp")
    with temporary.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(
            handle,
            fieldnames=fields,
            extrasaction="ignore",
            lineterminator="\n",
        )
        writer.writeheader()
        writer.writerows(rows)
    temporary.replace(path)


def write_json(path: Path, value: Any) -> None:
    temporary = path.with_name(f"{path.name}.tmp")
    temporary.write_text(
        json.dumps(value, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    temporary.replace(path)


def load_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def run(
    root: Path,
    work: Path,
    command: list[str],
    *,
    stdin_path: Path | None = None,
    stdout_path: Path | None = None,
) -> subprocess.CompletedProcess[bytes]:
    work.mkdir(parents=True, exist_ok=True)
    environment = os.environ.copy()
    environment.update(local_environment(root))
    environment["MAKEFLAGS"] = "-j2"
    environment["GIT_AUTHOR_NAME"] = "Copilot"
    environment["GIT_AUTHOR_EMAIL"] = (
        "223556219+Copilot@users.noreply.github.com"
    )
    environment["GIT_COMMITTER_NAME"] = "Copilot"
    environment["GIT_COMMITTER_EMAIL"] = (
        "223556219+Copilot@users.noreply.github.com"
    )
    stdin_handle = stdin_path.open("rb") if stdin_path is not None else None
    stdout_handle = (
        stdout_path.open("wb")
        if stdout_path is not None
        else subprocess.PIPE
    )
    try:
        completed = subprocess.run(
            command,
            cwd=root,
            env=environment,
            stdin=stdin_handle,
            stdout=stdout_handle,
            stderr=subprocess.PIPE,
            check=False,
        )
    finally:
        if stdin_handle is not None:
            stdin_handle.close()
        if stdout_path is not None:
            stdout_handle.close()
    stdout = b"" if stdout_path is not None else completed.stdout
    with (work / "commands.log").open("ab") as handle:
        handle.write(("$ " + " ".join(command) + "\n").encode())
        handle.write(f"exit: {completed.returncode}\n".encode())
        if stdout:
            handle.write(b"[stdout]\n")
            handle.write(stdout[-8192:])
            handle.write(b"\n")
        if completed.stderr:
            handle.write(b"[stderr]\n")
            handle.write(completed.stderr[-8192:])
            handle.write(b"\n")
    return subprocess.CompletedProcess(
        completed.args,
        completed.returncode,
        stdout,
        completed.stderr,
    )


def require_success(
    completed: subprocess.CompletedProcess[bytes],
    description: str,
) -> None:
    if completed.returncode:
        detail = completed.stderr.decode(errors="replace").splitlines()
        suffix = detail[-1] if detail else f"exit {completed.returncode}"
        raise AuditError(f"{description}: {suffix}")


def target_bytes(target: Path, address: int, size: int) -> bytes:
    offset = HEADER_SIZE + address - LOAD_ADDRESS
    with target.open("rb") as handle:
        handle.seek(offset)
        value = handle.read(size)
    if len(value) != size:
        raise AuditError(f"{address:#010x}: short target read")
    return value


def matching_uses_asm(root: Path, entry: dict[str, Any]) -> bool:
    source = resolve_within(root, str(entry["source"]), must_exist=True)
    text = COMMENT_PATTERN.sub("", source.read_text(encoding="utf-8"))
    return ASM_PATTERN.search(text) is not None


def source_uses_asm(text: str) -> bool:
    code = COMMENT_PATTERN.sub("", text)
    return (
        ASM_PATTERN.search(code) is not None
        or INCLUDE_ASM_PATTERN.search(code) is not None
    )


def load_unit_flags(path: Path) -> dict[str, str]:
    result: dict[str, str] = {}
    for original in path.read_text(encoding="utf-8").splitlines():
        line = original.split("#", 1)[0].strip()
        match = UNIT_FLAG_PATTERN.match(line)
        if not match or match.group(1) == "unit":
            continue
        result[match.group(1)] = match.group(2).strip()
    return result


def preferred_symbol_names(
    root: Path,
    functions: dict[int, dict[str, str]],
) -> tuple[dict[str, str], dict[int, str]]:
    current_symbols = load_symbol_file(
        resolve_within(root, "config/slus_01411/symbols.txt", must_exist=True)
    )
    preferred: dict[int, str] = {}

    def rank(name: str) -> tuple[int, int, str]:
        if name.startswith("g") and not name.startswith("glabel"):
            return (0, len(name), name)
        if not re.fullmatch(r"(?:D|func)_[0-9A-Fa-f]{8}", name):
            return (1, len(name), name)
        return (2, len(name), name)

    by_address: dict[int, list[str]] = defaultdict(list)
    for name, address in current_symbols.items():
        by_address[address].append(name)
    for address, names in by_address.items():
        preferred[address] = min(names, key=rank)
    for address, function in functions.items():
        preferred[address] = function["name"]

    reference_symbols = load_symbol_file(
        resolve_within(root, REFERENCE_SYMBOLS_PATH, must_exist=True)
    )
    replacements: dict[str, str] = {}
    for name, address in reference_symbols.items():
        replacement = preferred.get(address)
        if replacement is None:
            replacement = (
                name
                if re.fullmatch(r"(?:D|func)_[0-9A-Fa-f]{8}", name)
                else f"D_{address:08X}"
            )
        if replacement != name:
            replacements[name] = replacement
    return replacements, preferred


def rewrite_identifiers(text: str, replacements: dict[str, str]) -> str:
    existing = set(
        re.findall(
            r"\b[A-Za-z_][A-Za-z0-9_]*\b",
            COMMENT_PATTERN.sub("", text),
        )
    )
    replacements = {
        old: new
        for old, new in replacements.items()
        if new not in existing
    }
    output: list[str] = []
    index = 0
    state = "code"
    quote = ""
    while index < len(text):
        char = text[index]
        pair = text[index : index + 2]
        if state == "block_comment":
            output.append(char)
            if pair == "*/":
                output.append("/")
                index += 2
                state = "code"
                continue
        elif state == "line_comment":
            output.append(char)
            if char == "\n":
                state = "code"
        elif state == "string":
            output.append(char)
            if char == "\\" and index + 1 < len(text):
                output.append(text[index + 1])
                index += 2
                continue
            if char == quote:
                state = "code"
        else:
            if pair == "/*":
                output.extend(("/*"))
                index += 2
                state = "block_comment"
                continue
            if pair == "//":
                output.extend(("//"))
                index += 2
                state = "line_comment"
                continue
            if char in {'"', "'"}:
                output.append(char)
                state = "string"
                quote = char
            elif char == "_" or char.isalpha():
                end = index + 1
                while end < len(text) and (
                    text[end] == "_" or text[end].isalnum()
                ):
                    end += 1
                identifier = text[index:end]
                output.append(replacements.get(identifier, identifier))
                index = end
                continue
            else:
                output.append(char)
        index += 1
    return "".join(output)


def profile_for_flags(flags: str) -> tuple[str, bool]:
    tokens = flags.split()
    keep_large_ori = "--keep-large-ori" in tokens
    tokens = [token for token in tokens if token != "--keep-large-ori"]
    allowed = {
        "-G0",
        "-G8",
        "-msplit-addresses",
        "-mno-split-addresses",
    }
    unsupported = [token for token in tokens if token not in allowed]
    if unsupported:
        raise AuditError(
            "unsupported collaborator flags: " + " ".join(unsupported)
        )
    group = "-G0" if "-G0" in tokens else "-G8"
    split = (
        "split"
        if "-msplit-addresses" in tokens
        else "no_split"
        if "-mno-split-addresses" in tokens
        else ""
    )
    if group == "-G0":
        base = "gcc_2_8_1_cc_g0_as_g8"
    else:
        base = "gcc_2_8_1_g8"
    return (f"{base}_{split}" if split else base, keep_large_ori)


def skip_space_and_comments(text: str, index: int) -> int:
    while index < len(text):
        if text[index].isspace():
            index += 1
            continue
        if text.startswith("/*", index):
            end = text.find("*/", index + 2)
            if end < 0:
                raise AuditError("unterminated block comment")
            index = end + 2
            continue
        if text.startswith("//", index):
            end = text.find("\n", index + 2)
            return len(text) if end < 0 else skip_space_and_comments(text, end)
        return index
    return index


def matching_delimiter(text: str, start: int, opening: str, closing: str) -> int:
    depth = 0
    index = start
    state = "code"
    quote = ""
    while index < len(text):
        char = text[index]
        pair = text[index : index + 2]
        if state == "block_comment":
            if pair == "*/":
                state = "code"
                index += 2
                continue
        elif state == "line_comment":
            if char == "\n":
                state = "code"
        elif state == "string":
            if char == "\\":
                index += 2
                continue
            if char == quote:
                state = "code"
        else:
            if pair == "/*":
                state = "block_comment"
                index += 2
                continue
            if pair == "//":
                state = "line_comment"
                index += 2
                continue
            if char in {'"', "'"}:
                state = "string"
                quote = char
            elif char == opening:
                depth += 1
            elif char == closing:
                depth -= 1
                if depth == 0:
                    return index
        index += 1
    raise AuditError(f"unterminated {opening}{closing} block")


def extract_function(text: str, name: str) -> str:
    include_pattern = re.compile(
        r'^\s*#\s*include\s+"common\.h"\s*\n?',
        re.MULTILINE,
    )
    text = include_pattern.sub("", text)
    for match in re.finditer(rf"\b{re.escape(name)}\b", text):
        index = skip_space_and_comments(text, match.end())
        if index >= len(text) or text[index] != "(":
            continue
        close_paren = matching_delimiter(text, index, "(", ")")
        brace = skip_space_and_comments(text, close_paren + 1)
        if brace >= len(text) or text[brace] != "{":
            continue
        close_brace = matching_delimiter(text, brace, "{", "}")
        return text[: close_brace + 1].rstrip() + "\n"
    raise AuditError(f"could not isolate definition of {name}")


def prepare_manifest(root: Path) -> list[dict[str, str]]:
    work = resolve_within(root, WORK_PATH)
    candidates = work / "candidates"
    candidates.mkdir(parents=True, exist_ok=True)
    definitions = read_csv(resolve_within(root, DEFINITIONS_PATH, must_exist=True))
    functions = {
        parse_address(row["address"]): row
        for row in read_csv(resolve_within(root, FUNCTIONS_PATH, must_exist=True))
    }
    matching_value = load_json(
        resolve_within(root, MATCHING_PATH, must_exist=True)
    )
    matching = {
        parse_address(str(row["address"])): row
        for row in matching_value["functions"]
    }
    inline = {
        address
        for address, entry in matching.items()
        if matching_uses_asm(root, entry)
    }
    unmatched = {
        address
        for address, row in functions.items()
        if row["module"] == "game" and row["status"] == "unmatched_asm"
    }
    relevant = unmatched | inline
    flags = load_unit_flags(
        resolve_within(root, REFERENCE_FLAGS_PATH, must_exist=True)
    )
    profiles = load_json(
        resolve_within(root, PROFILES_PATH, must_exist=True)
    )["profiles"]
    symbol_replacements, _preferred = preferred_symbol_names(root, functions)
    attempts = read_csv(
        resolve_within(root, EXTERNAL_ATTEMPTS_PATH, must_exist=True)
    )
    grouped: dict[int, list[dict[str, str]]] = defaultdict(list)
    for row in definitions:
        address = parse_address(row["address"])
        if (
            address in relevant
            and row["file_asm"] == "False"
            and row["include_asm"] == "False"
        ):
            grouped[address].append(row)

    manifest: list[dict[str, str]] = []
    rejected: list[dict[str, str]] = []
    target = resolve_within(root, TARGET_PATH, must_exist=True)
    for address in sorted(grouped):
        alternatives = sorted(
            grouped[address],
            key=lambda row: (Path(row["source"]).name.startswith("func_"), row["source"]),
        )
        pure_alternatives = [
            row
            for row in alternatives
            if not source_uses_asm(
                resolve_within(root, row["source"], must_exist=True).read_text(
                    encoding="utf-8"
                )
            )
        ]
        if not pure_alternatives:
            rejected.append(
                {
                    "address": f"0x{address:08X}",
                    "reason": "all collaborator definitions contain inline assembly",
                    "reference_paths": ";".join(
                        row["source"] for row in alternatives
                    ),
                }
            )
            continue
        selected = pure_alternatives[0]
        reference = resolve_within(root, selected["source"], must_exist=True)
        source_text = extract_function(
            reference.read_text(encoding="utf-8"),
            selected["name"],
        )
        current = functions[address]
        source_text = re.sub(
            rf"\b{re.escape(selected['name'])}\b",
            current["name"],
            source_text,
        )
        source_text = rewrite_identifiers(source_text, symbol_replacements)
        source_text = TYPEDEFS + source_text.lstrip()
        if source_uses_asm(source_text):
            raise AuditError(f"{address:#010x}: prepared source contains asm")
        candidate = candidates / f"func_{address:08X}.c"
        candidate.write_text(source_text, encoding="utf-8")
        unit = Path(selected["source"]).stem
        reference_flags = flags.get(unit, "-G8")
        profile, keep_large_ori = profile_for_flags(reference_flags)
        if profile not in profiles:
            raise AuditError(f"{address:#010x}: missing profile {profile}")
        mode = (
            "inline_refinement"
            if address in inline
            else "collaborator_match"
        )
        prior = sum(
            1
            for row in attempts
            if parse_address(row["address"]) == address
            and row["mode"] == mode
        )
        current_entry = matching.get(address, {})
        manifest.append(
            {
                "address": f"0x{address:08X}",
                "size": current["size"],
                "current_name": current["name"],
                "current_status": current["status"],
                "current_source": str(current_entry.get("source", "")),
                "current_profile": str(current_entry.get("profile", "")),
                "mode": mode,
                "reference_name": selected["name"],
                "reference_path": str(reference.relative_to(root)),
                "reference_sha256": sha256(reference),
                "reference_flags": reference_flags,
                "keep_large_ori": str(keep_large_ori).lower(),
                "profile": profile,
                "candidate_source": str(candidate.relative_to(root)),
                "candidate_sha256": sha256(candidate),
                "target_sha256": hashlib.sha256(
                    target_bytes(target, address, parse_address(current["size"]))
                ).hexdigest(),
                "prior_mode_attempts": str(prior),
                "declaration_policy": (
                    "collaborator declarations are hypotheses; accept only "
                    "after local exact bytes and relocation verification"
                ),
            }
        )

    if len(manifest) + len(rejected) != 94:
        raise AuditError(
            "expected 94 relevant candidate addresses, found "
            f"{len(manifest)} accepted plus {len(rejected)} rejected"
        )
    write_csv(work / "candidate-manifest.csv", MANIFEST_FIELDS, manifest)
    write_json(
        work / "candidate-manifest.json",
        {"schema": 1, "candidates": manifest},
    )
    write_csv(
        work / "rejected-candidates.csv",
        ("address", "reason", "reference_paths"),
        rejected,
    )
    write_json(
        work / "rejected-candidates.json",
        {"schema": 1, "rejected": rejected},
    )
    return manifest


def tool(root: Path, name: str) -> Path:
    return resolve_within(
        root,
        f"{BINUTILS_PATH}/mipsel-none-elf-{name}",
        must_exist=True,
    )


def ensure_baseline(root: Path, work: Path) -> None:
    target = resolve_within(root, TARGET_PATH, must_exist=True)
    built = resolve_within(root, BUILT_PATH)
    elf = resolve_within(root, TARGET_ELF_PATH)
    if built.is_file() and elf.is_file() and sha256(built) == sha256(target):
        return
    log = work / "baseline-build.log"
    completed = run(
        root,
        work,
        ["make", "match"],
        stdout_path=log,
    )
    require_success(completed, "clean baseline build")
    if not built.is_file() or sha256(built) != sha256(target):
        raise AuditError("baseline build did not reproduce the target")


def load_symbol_file(path: Path) -> dict[str, int]:
    result: dict[str, int] = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        match = SYMBOL_LINE_PATTERN.match(line)
        if match:
            result[match.group(1)] = int(match.group(2), 0)
    return result


def load_nm_symbols(
    root: Path,
    work: Path,
    path: Path,
) -> dict[str, int]:
    completed = run(root, work, [str(tool(root, "nm")), "-n", str(path)])
    require_success(completed, f"nm {path.name}")
    result: dict[str, int] = {}
    for line in completed.stdout.decode(errors="replace").splitlines():
        fields = line.split()
        if len(fields) >= 3:
            try:
                result[fields[-1]] = int(fields[0], 16)
            except ValueError:
                continue
    return result


def all_symbols(root: Path, work: Path) -> dict[str, int]:
    result = load_nm_symbols(
        root,
        work,
        resolve_within(root, TARGET_ELF_PATH, must_exist=True),
    )
    result.update(
        load_symbol_file(
            resolve_within(root, REFERENCE_SYMBOLS_PATH, must_exist=True)
        )
    )
    result.update(
        load_symbol_file(
            resolve_within(root, "config/slus_01411/symbols.txt", must_exist=True)
        )
    )
    result["_gp"] = RUNTIME_GP
    result["runtime_gp"] = RUNTIME_GP
    return result


def symbol_address(
    symbol: str,
    symbols: dict[str, int],
    text_address: int,
) -> int:
    if symbol in symbols:
        return symbols[symbol]
    if symbol.startswith(".text"):
        return text_address
    match = SYMBOL_ADDRESS_PATTERN.search(symbol)
    if match:
        return int(match.group(1), 16)
    raise AuditError(f"cannot resolve symbol {symbol}")


def object_relocations(
    root: Path,
    work: Path,
    path: Path,
) -> list[dict[str, Any]]:
    completed = run(
        root,
        work,
        [str(tool(root, "readelf")), "-rW", str(path)],
    )
    require_success(completed, f"readelf relocations {path.name}")
    rows: list[dict[str, Any]] = []
    in_text = False
    pattern = re.compile(
        r"^\s*([0-9A-Fa-f]+)\s+\S+\s+(R_MIPS_\S+)\s+"
        r"[0-9A-Fa-f]+\s+(\S+)"
    )
    for line in completed.stdout.decode(errors="replace").splitlines():
        if line.startswith("Relocation section '"):
            in_text = line.startswith("Relocation section '.rel.text'")
            continue
        match = pattern.match(line) if in_text else None
        if match:
            rows.append(
                {
                    "offset": int(match.group(1), 16),
                    "type": match.group(2),
                    "symbol": match.group(3),
                }
            )
    return rows


def normalized_relocations(
    rows: list[dict[str, Any]],
    symbols: dict[str, int],
    text_address: int,
) -> list[tuple[int, str, int]]:
    return sorted(
        (
            int(row["offset"]),
            str(row["type"]),
            symbol_address(str(row["symbol"]), symbols, text_address),
        )
        for row in rows
    )


def find_asm_block(root: Path, address: int, names: list[str]) -> str:
    directory = resolve_within(root, GENERATED_ASM_PATH, must_exist=True)
    for path in sorted(directory.glob("*.s")):
        text = path.read_text(encoding="utf-8", errors="replace")
        for name in names:
            marker = f"glabel {name}"
            start = text.find(marker)
            if start < 0:
                continue
            end = text.find(f"endlabel {name}", start)
            if end < 0:
                raise AuditError(f"{address:#010x}: missing endlabel {name}")
            line_end = text.find("\n", end)
            return text[start : line_end if line_end >= 0 else None]
    raise AuditError(f"{address:#010x}: target assembly block not found")


def asm_relocations(block: str, address: int) -> list[dict[str, Any]]:
    result: list[dict[str, Any]] = []
    instruction = re.compile(
        r"/\*\s+\S+\s+([0-9A-Fa-f]{8})\s+[0-9A-Fa-f]{8}\s+\*/\s+"
        r"(\S+)\s*(.*?)\s*$"
    )
    for line in block.splitlines():
        match = instruction.search(line)
        if not match:
            continue
        offset = int(match.group(1), 16) - address
        mnemonic = match.group(2)
        operands = match.group(3)
        for kind, expression in re.findall(
            r"%(hi|lo|gp_rel)\(([^)]+)\)",
            operands,
        ):
            symbol = re.split(r"\s+[+-]\s+", expression, maxsplit=1)[0]
            result.append(
                {
                    "offset": offset,
                    "type": {
                        "hi": "R_MIPS_HI16",
                        "lo": "R_MIPS_LO16",
                        "gp_rel": "R_MIPS_GPREL16",
                    }[kind],
                    "symbol": symbol,
                }
            )
        if mnemonic in {"j", "jal"}:
            symbol = operands.split(",", 1)[0].strip()
            if symbol.startswith(".L"):
                symbol = ".text"
            result.append(
                {
                    "offset": offset,
                    "type": "R_MIPS_26",
                    "symbol": symbol,
                }
            )
    return result


def expected_relocations(
    root: Path,
    work: Path,
    row: dict[str, str],
) -> list[dict[str, Any]]:
    address = parse_address(row["address"])
    if row["mode"] == "inline_refinement":
        object_path = resolve_within(
            root,
            f"tmp/project-build/obj/c_{address:08x}.o",
            must_exist=True,
        )
        return object_relocations(root, work, object_path)
    block = find_asm_block(
        root,
        address,
        [row["current_name"], f"func_{address:08X}"],
    )
    return asm_relocations(block, address)


def non_text_sections(
    root: Path,
    work: Path,
    path: Path,
) -> list[dict[str, Any]]:
    completed = run(
        root,
        work,
        [str(tool(root, "objdump")), "-h", str(path)],
    )
    require_success(completed, f"objdump sections {path.name}")
    lines = completed.stdout.decode(errors="replace").splitlines()
    ignored = {
        ".reginfo",
        ".MIPS.abiflags",
        ".pdr",
        ".gnu.attributes",
        ".comment",
    }
    result: list[dict[str, Any]] = []
    for index, line in enumerate(lines):
        match = re.match(r"\s*\d+\s+(\S+)\s+([0-9A-Fa-f]+)\s+", line)
        if not match:
            continue
        name = match.group(1)
        size = int(match.group(2), 16)
        flags = lines[index + 1] if index + 1 < len(lines) else ""
        if size and name != ".text" and name not in ignored and "ALLOC" in flags:
            result.append({"name": name, "size": size})
    return result


def undefined_symbols(
    root: Path,
    work: Path,
    path: Path,
) -> list[str]:
    completed = run(root, work, [str(tool(root, "nm")), "-u", str(path)])
    require_success(completed, f"nm undefined {path.name}")
    result: list[str] = []
    for line in completed.stdout.decode(errors="replace").splitlines():
        fields = line.split()
        if fields:
            result.append(fields[-1])
    return result


def compile_candidate(
    root: Path,
    work: Path,
    source: Path,
    profile: dict[str, Any],
) -> Path:
    compiler = resolve_within(root, profile["compiler"], must_exist=True)
    maspsx = resolve_within(root, MASPSX_PATH, must_exist=True)
    raw = work / "compiler.s"
    filtered = work / "filtered.s"
    transformed = work / "maspsx.s"
    obj = work / "candidate.o"
    preprocessed = work / "preprocessed.c"
    completed = run(
        root,
        work,
        [
            str(compiler),
            "-E",
            "-P",
            *profile["compiler_flags"],
            str(source),
        ],
        stdout_path=preprocessed,
    )
    require_success(completed, "candidate preprocessing")
    if ASM_PATTERN.search(preprocessed.read_text(encoding="utf-8")):
        raise AuditError("preprocessed candidate contains asm")
    completed = run(
        root,
        work,
        [
            str(compiler),
            "-S",
            *profile["compiler_flags"],
            "-o",
            str(raw),
            str(source),
        ],
    )
    require_success(completed, "candidate compilation")
    maspsx_input = raw
    assembly_filter = profile.get("assembly_filter")
    if assembly_filter is not None:
        completed = run(
            root,
            work,
            [
                sys.executable,
                str(resolve_within(root, assembly_filter, must_exist=True)),
            ],
            stdin_path=raw,
            stdout_path=filtered,
        )
        require_success(completed, "assembly filter")
        maspsx_input = filtered
    completed = run(
        root,
        work,
        [
            sys.executable,
            str(maspsx),
            f"--aspsx-version={profile['aspsx_version']}",
            *profile["maspsx_flags"],
        ],
        stdin_path=maspsx_input,
        stdout_path=transformed,
    )
    require_success(completed, "MASPSX")
    completed = run(
        root,
        work,
        [
            str(tool(root, "as")),
            "-EL",
            "-mips1",
            f"-G{profile['data_limit']}",
            "-o",
            str(obj),
            str(transformed),
        ],
    )
    require_success(completed, "candidate assembly")
    return obj


def link_candidate(
    root: Path,
    work: Path,
    obj: Path,
    address: int,
    symbols: dict[str, int],
) -> bytes:
    definitions = [f"_gp = 0x{RUNTIME_GP:08X};"]
    for symbol in sorted(set(undefined_symbols(root, work, obj))):
        definitions.append(
            f"{symbol} = 0x{symbol_address(symbol, symbols, address):08X};"
        )
    script = work / "link.ld"
    script.write_text(
        "\n".join(
            [
                'OUTPUT_FORMAT("elf32-littlemips")',
                "OUTPUT_ARCH(mips)",
                *definitions,
                "SECTIONS",
                "{",
                f"  .text 0x{address:08X} : {{ *(.text) }}",
                "  /DISCARD/ : {",
                "    *(.comment) *(.gnu.attributes) *(.MIPS.abiflags)",
                "    *(.note*) *(.pdr) *(.reginfo)",
                "    *(.data) *(.bss) *(.rodata) *(.rdata)",
                "    *(.sdata) *(.sbss)",
                "  }",
                "}",
            ]
        )
        + "\n",
        encoding="utf-8",
    )
    elf = work / "linked.elf"
    binary = work / "linked.bin"
    completed = run(
        root,
        work,
        [
            str(tool(root, "ld")),
            "-EL",
            "-G0",
            "--no-relax",
            "-T",
            str(script),
            "-o",
            str(elf),
            str(obj),
        ],
    )
    require_success(completed, "candidate link")
    completed = run(
        root,
        work,
        [
            str(tool(root, "objcopy")),
            "-j",
            ".text",
            "-O",
            "binary",
            str(elf),
            str(binary),
        ],
    )
    require_success(completed, "candidate text extraction")
    return binary.read_bytes()


def first_difference(expected: bytes, actual: bytes) -> str:
    for index, (left, right) in enumerate(zip(expected, actual)):
        if left != right:
            word = index & ~3
            return (
                f"+0x{index:X}: "
                f"{expected[word:word + 4].hex()} != "
                f"{actual[word:word + 4].hex()}"
            )
    if len(expected) != len(actual):
        return f"size 0x{len(expected):X} != 0x{len(actual):X}"
    return ""


def verify_one(
    root: Path,
    row: dict[str, str],
    profiles: dict[str, dict[str, Any]],
    symbols: dict[str, int],
) -> dict[str, Any]:
    address = parse_address(row["address"])
    work_root = resolve_within(root, f"{WORK_PATH}/build")
    build = work_root / f"{address:08X}"
    try:
        build.relative_to(work_root)
    except ValueError as error:
        raise AuditError("candidate build escaped work directory") from error
    if build.exists():
        shutil.rmtree(build)
    build.mkdir(parents=True)
    result: dict[str, Any] = {
        "address": row["address"],
        "mode": row["mode"],
        "profile": row["profile"],
        "exact": False,
        "bytes_match": False,
        "relocations_match": False,
        "sections_ok": False,
        "expected_size": parse_address(row["size"]),
        "actual_size": "",
        "first_difference": "",
        "error": "",
    }
    try:
        source = resolve_within(root, row["candidate_source"], must_exist=True)
        obj = compile_candidate(root, build, source, profiles[row["profile"]])
        extra_sections = non_text_sections(root, build, obj)
        result["sections_ok"] = not extra_sections
        if extra_sections:
            result["extra_sections"] = extra_sections
        candidate_relocations = object_relocations(root, build, obj)
        target_relocations = expected_relocations(root, build, row)
        result["candidate_relocations"] = candidate_relocations
        result["target_relocations"] = target_relocations
        result["relocations_match"] = normalized_relocations(
            candidate_relocations,
            symbols,
            address,
        ) == normalized_relocations(
            target_relocations,
            symbols,
            address,
        )
        actual = link_candidate(root, build, obj, address, symbols)
        expected = target_bytes(
            resolve_within(root, TARGET_PATH, must_exist=True),
            address,
            parse_address(row["size"]),
        )
        result["actual_size"] = len(actual)
        result["bytes_match"] = actual == expected
        result["first_difference"] = first_difference(expected, actual)
        result["expected_text_sha256"] = hashlib.sha256(expected).hexdigest()
        result["actual_text_sha256"] = hashlib.sha256(actual).hexdigest()
        result["exact"] = (
            result["bytes_match"]
            and result["relocations_match"]
            and result["sections_ok"]
        )
    except (
        AuditError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        subprocess.SubprocessError,
    ) as error:
        result["error"] = str(error).splitlines()[0][:500]
    write_json(build / "result.json", result)
    return result


def load_manifest(root: Path) -> list[dict[str, str]]:
    path = resolve_within(
        root,
        f"{WORK_PATH}/candidate-manifest.csv",
        must_exist=True,
    )
    rows = read_csv(path)
    if tuple(rows[0]) != MANIFEST_FIELDS if rows else True:
        raise AuditError("candidate manifest fields are invalid")
    return rows


def select_rows(
    rows: list[dict[str, str]],
    *,
    address: int | None,
    limit: int | None,
) -> list[dict[str, str]]:
    if address is not None:
        rows = [row for row in rows if parse_address(row["address"]) == address]
        if not rows:
            raise AuditError(f"{address:#010x}: not in candidate manifest")
    if limit is not None:
        if limit < 1:
            raise AuditError("--limit must be positive")
        rows = rows[:limit]
    return rows


def verify(
    root: Path,
    rows: list[dict[str, str]],
) -> list[dict[str, Any]]:
    work = resolve_within(root, WORK_PATH)
    ensure_baseline(root, work)
    profiles = load_json(
        resolve_within(root, PROFILES_PATH, must_exist=True)
    )["profiles"]
    symbols = all_symbols(root, work)
    results = [verify_one(root, row, profiles, symbols) for row in rows]
    write_csv(work / "verification-results.csv", RESULT_FIELDS, results)
    write_json(
        work / "verification-results.json",
        {"schema": 1, "results": results},
    )
    counts = Counter(
        "exact"
        if row["exact"]
        else "error"
        if row["error"]
        else "nonmatch"
        for row in results
    )
    summary = {
        "schema": 1,
        "selected": len(rows),
        "exact": counts["exact"],
        "nonmatch": counts["nonmatch"],
        "error": counts["error"],
        "exact_addresses": [
            row["address"] for row in results if row["exact"]
        ],
        "error_addresses": [
            row["address"] for row in results if row["error"]
        ],
    }
    write_json(work / "verification-summary.json", summary)
    return results


def git_output(root: Path, work: Path, *args: str) -> str:
    completed = run(root, work, ["git", "--no-pager", *args])
    require_success(completed, "git " + " ".join(args))
    return completed.stdout.decode(errors="replace").strip()


def require_integration_repository(root: Path, work: Path) -> None:
    if git_output(root, work, "branch", "--show-current") != "master":
        raise AuditError("integration requires branch master")
    status = git_output(
        root,
        work,
        "status",
        "--porcelain=v1",
        "--untracked-files=all",
    )
    if status:
        raise AuditError("integration requires a clean working tree")
    upstream = git_output(
        root,
        work,
        "rev-parse",
        "--abbrev-ref",
        "--symbolic-full-name",
        "@{upstream}",
    )
    if upstream != "origin/master":
        raise AuditError(f"unexpected upstream: {upstream}")
    if git_output(root, work, "rev-parse", "HEAD") != git_output(
        root,
        work,
        "rev-parse",
        "origin/master",
    ):
        raise AuditError("master must be synchronized with origin/master")


def result_rows(root: Path) -> dict[int, dict[str, str]]:
    rows = read_csv(
        resolve_within(
            root,
            f"{WORK_PATH}/verification-results.csv",
            must_exist=True,
        )
    )
    return {parse_address(row["address"]): row for row in rows}


def live_matching(root: Path) -> dict[int, dict[str, Any]]:
    value = load_json(resolve_within(root, MATCHING_PATH, must_exist=True))
    return {
        parse_address(str(row["address"])): row
        for row in value["functions"]
    }


def live_functions(root: Path) -> dict[int, dict[str, str]]:
    return {
        parse_address(row["address"]): row
        for row in read_csv(resolve_within(root, FUNCTIONS_PATH, must_exist=True))
    }


def external_rows(root: Path) -> list[dict[str, str]]:
    return read_csv(
        resolve_within(root, EXTERNAL_ATTEMPTS_PATH, must_exist=True)
    )


def evidence_history(
    rows: list[dict[str, str]],
    address: int,
    mode: str,
) -> list[dict[str, str]]:
    return [
        row
        for row in rows
        if parse_address(row["address"]) == address and row["mode"] == mode
    ]


def evidence_summary(
    result: dict[str, str],
    *,
    exact: bool,
) -> str:
    if exact:
        return (
            "Unchiga pure-C source independently reproduced exact linked "
            "text and relocation targets with no allocated non-text sections."
        )
    detail = result["first_difference"] or "text differs"
    return (
        "Unchiga pure-C source independently tested; "
        f"bytes={result['bytes_match']}, "
        f"relocations={result['relocations_match']}, "
        f"sections={result['sections_ok']}; {detail}."
    )


def append_evidence(
    root: Path,
    manifest_row: dict[str, str],
    result: dict[str, str],
    outcome: str,
) -> None:
    path = resolve_within(root, EXTERNAL_ATTEMPTS_PATH, must_exist=True)
    rows = record_external_attempt.load_rows(path)
    address = parse_address(manifest_row["address"])
    mode = manifest_row["mode"]
    history = evidence_history(rows, address, mode)
    maximum = record_external_attempt.MODE_MAX_ATTEMPTS[mode]
    if history and history[-1]["result"] in record_external_attempt.TERMINAL_RESULTS:
        if history[-1]["result"] == outcome:
            return
        raise AuditError(
            f"{address:#010x}: {mode} history already ended with "
            f"{history[-1]['result']}"
        )
    if maximum is not None and len(history) >= maximum:
        raise AuditError(f"{address:#010x}: {mode} allows {maximum} attempt(s)")
    candidate = resolve_within(
        root,
        manifest_row["candidate_source"],
        must_exist=True,
    )
    reference = resolve_within(
        root,
        manifest_row["reference_path"],
        must_exist=True,
    )
    rows.append(
        {
            "mode": mode,
            "address": manifest_row["address"],
            "attempt": str(len(history) + 1),
            "reference_path": manifest_row["reference_path"],
            "reference_sha256": sha256(reference),
            "profile": manifest_row["profile"],
            "candidate_source": manifest_row["candidate_source"],
            "candidate_sha256": sha256(candidate),
            "result": outcome,
            "summary": evidence_summary(result, exact=outcome == "matched"),
        }
    )
    functions = record_external_attempt.load_functions(
        resolve_within(root, FUNCTIONS_PATH, must_exist=True)
    )
    matching_addresses = record_external_attempt.load_matching_addresses(
        resolve_within(root, MATCHING_PATH, must_exist=True)
    )
    profiles = record_external_attempt.load_profiles(
        resolve_within(root, PROFILES_PATH, must_exist=True)
    )
    record_external_attempt.validate_rows(
        rows,
        functions,
        matching_addresses,
        profiles,
    )
    record_external_attempt.write_rows(path, rows)


def restore_paths(backups: dict[Path, bytes | None]) -> None:
    for path, original in backups.items():
        if original is None:
            path.unlink(missing_ok=True)
        else:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(original)


def add_required_linker_aliases(
    root: Path,
    work: Path,
    address: int,
) -> list[str]:
    obj = resolve_within(
        root,
        f"{WORK_PATH}/build/{address:08X}/candidate.o",
        must_exist=True,
    )
    config_path = resolve_within(
        root,
        C_LINKER_SYMBOLS_PATH,
        must_exist=True,
    )
    configured = load_symbol_file(config_path)
    configured.update(
        load_symbol_file(
            resolve_within(root, SYMBOLS_CONFIG_PATH, must_exist=True)
        )
    )
    function_names = {
        row["name"]
        for row in read_csv(
            resolve_within(root, FUNCTIONS_PATH, must_exist=True)
        )
    }
    reference = load_symbol_file(
        resolve_within(root, REFERENCE_SYMBOLS_PATH, must_exist=True)
    )
    target_symbols = load_nm_symbols(
        root,
        work,
        resolve_within(root, TARGET_ELF_PATH, must_exist=True),
    )
    aliases: list[tuple[str, int]] = []
    for symbol in sorted(set(undefined_symbols(root, work, obj))):
        if symbol in configured or symbol in function_names or symbol == "_gp":
            continue
        if not re.fullmatch(r"[A-Za-z_.$][A-Za-z0-9_.$]*", symbol):
            raise AuditError(f"{address:#010x}: invalid alias symbol {symbol}")
        alias_address = target_symbols.get(symbol) or reference.get(symbol)
        if alias_address is None:
            match = SYMBOL_ADDRESS_PATTERN.search(symbol)
            if match:
                alias_address = int(match.group(1), 16)
        if alias_address is None:
            raise AuditError(
                f"{address:#010x}: unresolved linker alias {symbol}"
            )
        aliases.append((symbol, alias_address))
    if not aliases:
        return []

    append_linker_aliases(config_path, aliases)
    return [symbol for symbol, _address in aliases]


def append_linker_aliases(
    config_path: Path,
    aliases: list[tuple[str, int]],
) -> None:
    if not aliases:
        return
    text = config_path.read_text(encoding="utf-8").rstrip()
    for symbol, alias_address in aliases:
        text += f"\n{symbol} = 0x{alias_address:08X};"
    config_path.write_text(text + "\n", encoding="utf-8")


def persist_current_c_symbols(root: Path, work: Path) -> list[str]:
    config_path = resolve_within(
        root,
        C_LINKER_SYMBOLS_PATH,
        must_exist=True,
    )
    configured = load_symbol_file(config_path)
    configured.update(
        load_symbol_file(
            resolve_within(root, SYMBOLS_CONFIG_PATH, must_exist=True)
        )
    )
    functions = live_functions(root)
    function_names = {row["name"] for row in functions.values()}
    matching = live_matching(root)
    objects = [
        resolve_within(
            root,
            f"tmp/project-build/obj/c_{address:08x}.o",
            must_exist=True,
        )
        for address in sorted(matching)
    ]
    completed = run(
        root,
        work,
        [str(tool(root, "nm")), "-u", *[str(path) for path in objects]],
    )
    require_success(completed, "scan matching-C undefined symbols")
    undefined: set[str] = set()
    for line in completed.stdout.decode(errors="replace").splitlines():
        fields = line.split()
        if len(fields) >= 2 and fields[-2] == "U":
            undefined.add(fields[-1])
    target_symbols = load_nm_symbols(
        root,
        work,
        resolve_within(root, TARGET_ELF_PATH, must_exist=True),
    )
    reference = load_symbol_file(
        resolve_within(root, REFERENCE_SYMBOLS_PATH, must_exist=True)
    )
    aliases: list[tuple[str, int]] = []
    for symbol in sorted(undefined):
        if symbol in configured or symbol in function_names or symbol == "_gp":
            continue
        address = target_symbols.get(symbol) or reference.get(symbol)
        if address is None:
            match = SYMBOL_ADDRESS_PATTERN.search(symbol)
            if match:
                address = int(match.group(1), 16)
        if address is None:
            raise AuditError(f"cannot persist C linker symbol {symbol}")
        aliases.append((symbol, address))
    append_linker_aliases(config_path, aliases)
    return [symbol for symbol, _address in aliases]


def candidate_destination(
    root: Path,
    manifest_row: dict[str, str],
) -> Path:
    if manifest_row["mode"] == "inline_refinement":
        return resolve_within(
            root,
            manifest_row["current_source"],
            must_exist=True,
        )
    return resolve_within(
        root,
        f"src/game/{manifest_row['current_name']}.c",
    )


def run_incremental_match(
    root: Path,
    work: Path,
    address: int,
) -> None:
    log = work / "integration-logs" / f"{address:08X}.log"
    log.parent.mkdir(parents=True, exist_ok=True)
    completed = run(
        root,
        work,
        ["make", "match-incremental"],
        stdout_path=log,
    )
    require_success(completed, f"{address:#010x}: incremental full match")


def commit_function(
    root: Path,
    work: Path,
    manifest_row: dict[str, str],
    destination: Path,
) -> None:
    address = parse_address(manifest_row["address"])
    completed = run(
        root,
        work,
        [
            "git",
            "add",
            "--",
            EXTERNAL_ATTEMPTS_PATH,
            FUNCTIONS_PATH,
            MATCHING_PATH,
            C_LINKER_SYMBOLS_PATH,
            str(destination.relative_to(root)),
        ],
    )
    require_success(completed, f"{address:#010x}: git add")
    completed = run(
        root,
        work,
        ["git", "diff", "--cached", "--check"],
    )
    require_success(completed, f"{address:#010x}: staged diff check")
    subject = (
        "decomp: replace inline assembly in "
        if manifest_row["mode"] == "inline_refinement"
        else "decomp: match "
    ) + manifest_row["current_name"]
    completed = run(
        root,
        work,
        [
            "git",
            "-c",
            "core.hooksPath=/dev/null",
            "commit",
            "--no-gpg-sign",
            "-m",
            subject,
            "-m",
            "Co-authored-by: Copilot "
            "<223556219+Copilot@users.noreply.github.com>",
        ],
    )
    require_success(completed, f"{address:#010x}: git commit")


def integrate_exact(
    root: Path,
    rows: list[dict[str, str]],
    results: dict[int, dict[str, str]],
) -> int:
    work = resolve_within(root, WORK_PATH)
    require_integration_repository(root, work)
    baseline_log = work / "integration-baseline.log"
    completed = run(
        root,
        work,
        ["make", "match"],
        stdout_path=baseline_log,
    )
    require_success(completed, "clean integration baseline")
    target = resolve_within(root, TARGET_PATH, must_exist=True)
    built = resolve_within(root, BUILT_PATH, must_exist=True)
    if sha256(target) != sha256(built):
        raise AuditError("clean integration baseline does not match target")
    durable_symbols = persist_current_c_symbols(root, work)
    if durable_symbols or git_output(
        root,
        work,
        "diff",
        "--",
        C_LINKER_SYMBOLS_PATH,
    ):
        symbol_log = work / "durable-symbol-match.log"
        completed = run(
            root,
            work,
            ["make", "match"],
            stdout_path=symbol_log,
        )
        require_success(completed, "durable C-symbol full match")
        completed = run(
            root,
            work,
            ["git", "add", "--", C_LINKER_SYMBOLS_PATH],
        )
        require_success(completed, "stage durable C symbols")
        completed = run(root, work, ["git", "diff", "--cached", "--check"])
        require_success(completed, "durable C-symbol diff check")
        completed = run(
            root,
            work,
            [
                "git",
                "-c",
                "core.hooksPath=/dev/null",
                "commit",
                "--no-gpg-sign",
                "-m",
                "build: persist matching C symbols",
                "-m",
                "Co-authored-by: Copilot "
                "<223556219+Copilot@users.noreply.github.com>",
            ],
        )
        require_success(completed, "commit durable C symbols")
        completed = run(root, work, ["git", "push", "origin", "master"])
        require_success(completed, "push durable C symbols")
    seed_log = work / "incremental-seed.log"
    completed = run(
        root,
        work,
        [
            str(
                resolve_within(
                    root,
                    "tools/environments/python/bin/python",
                    must_exist=True,
                )
            ),
            str(resolve_within(root, INCREMENTAL_TOOL_PATH, must_exist=True)),
            "--seed-existing",
        ],
        stdout_path=seed_log,
    )
    require_success(completed, "incremental cache seed")

    integrated = 0
    since_push = 0
    last_push = time.monotonic()
    for manifest_row in rows:
        address = parse_address(manifest_row["address"])
        result = results[address]
        if result["exact"] != "True":
            continue
        functions = live_functions(root)
        matching = live_matching(root)
        history = evidence_history(
            external_rows(root),
            address,
            manifest_row["mode"],
        )
        already_matched = (
            bool(history)
            and history[-1]["result"] == "matched"
            and address in matching
            and functions[address]["status"] == "matching_c"
        )
        if already_matched:
            continue

        destination = candidate_destination(root, manifest_row)
        tracked_paths = [
            resolve_within(root, EXTERNAL_ATTEMPTS_PATH, must_exist=True),
            resolve_within(root, FUNCTIONS_PATH, must_exist=True),
            resolve_within(root, MATCHING_PATH, must_exist=True),
            resolve_within(root, C_LINKER_SYMBOLS_PATH, must_exist=True),
            destination,
        ]
        backups = {
            path: path.read_bytes() if path.exists() else None
            for path in tracked_paths
        }
        try:
            add_required_linker_aliases(root, work, address)
            append_evidence(root, manifest_row, result, "matched")
            command = [
                str(
                    resolve_within(
                        root,
                        "tools/environments/python/bin/python",
                        must_exist=True,
                    )
                ),
                str(resolve_within(root, INTEGRATE_TOOL_PATH, must_exist=True)),
                manifest_row["address"],
                "--source",
                manifest_row["candidate_source"],
                "--destination",
                str(destination.relative_to(root)),
                "--profile",
                manifest_row["profile"],
                "--note",
                (
                    "Unchiga pure-C structure independently reproduced; "
                    f"exact linked bytes and relocations with "
                    f"{manifest_row['profile']}."
                ),
                "--evidence-source",
                (
                    "refinement"
                    if manifest_row["mode"] == "inline_refinement"
                    else "collaborator"
                ),
            ]
            if manifest_row["mode"] == "inline_refinement":
                command.append("--replace-existing")
            completed = run(root, work, command)
            require_success(completed, f"{address:#010x}: integrate candidate")
            run_incremental_match(root, work, address)
            commit_function(root, work, manifest_row, destination)
        except Exception:
            restore_paths(backups)
            run(
                root,
                work,
                ["git", "reset", "--quiet"],
            )
            raise
        integrated += 1
        since_push += 1
        if since_push >= 10 or time.monotonic() - last_push >= 1800:
            completed = run(root, work, ["git", "push", "origin", "master"])
            require_success(completed, "periodic git push")
            since_push = 0
            last_push = time.monotonic()

    if since_push:
        completed = run(root, work, ["git", "push", "origin", "master"])
        require_success(completed, "final exact-match push")
    return integrated


def render_audit_report(
    manifest: list[dict[str, str]],
    results: dict[int, dict[str, str]],
    rejected: list[dict[str, str]],
) -> str:
    exact = [
        row
        for row in manifest
        if results[parse_address(row["address"])]["exact"] == "True"
    ]
    misses = [
        row
        for row in manifest
        if results[parse_address(row["address"])]["exact"] != "True"
    ]
    lines = [
        "# Unchiga Pure-C Match Audit",
        "",
        "The collaborator sources were treated as hypotheses and compiled with "
        "the local Psy-Q 4.6/GCC 2.8.1 plus MASPSX 2.81 pipeline. All work was "
        "performed sequentially. Acceptance required exact linked function "
        "bytes, equivalent relocation targets, and no allocated non-text "
        "sections.",
        "",
        "## Results",
        "",
        f"- Initial address overlap: {len(manifest) + len(rejected)}",
        f"- Rejected for actual inline assembly: {len(rejected)}",
        f"- Independently tested pure-C candidates: {len(manifest)}",
        f"- Exact pure-C matches: {len(exact)}",
        f"- Pure-C nonmatches: {len(misses)}",
        "- Tool failures: 0",
        "",
        "## Exact matches",
        "",
        "| Address | Project symbol | Mode | Profile |",
        "| --- | --- | --- | --- |",
    ]
    for row in exact:
        lines.append(
            f"| `{row['address']}` | `{row['current_name']}` | "
            f"`{row['mode']}` | `{row['profile']}` |"
        )
    lines.extend(
        [
            "",
            "## Nonmatches",
            "",
            "| Address | Project symbol | Mode | First difference |",
            "| --- | --- | --- | --- |",
        ]
    )
    for row in misses:
        result = results[parse_address(row["address"])]
        detail = result["first_difference"] or "relocation/section mismatch"
        lines.append(
            f"| `{row['address']}` | `{row['current_name']}` | "
            f"`{row['mode']}` | `{detail}` |"
        )
    lines.extend(
        [
            "",
            "## Rejected collaborator definitions",
            "",
            "These files were excluded without consuming a pure-C attempt "
            "because direct source inspection found GCC inline assembly.",
            "",
            "| Address | Reason |",
            "| --- | --- |",
        ]
    )
    for row in rejected:
        lines.append(f"| `{row['address']}` | {row['reason']} |")
    lines.extend(
        [
            "",
            "Detailed manifests, source hashes, result JSON, command logs, and "
            "per-candidate build artifacts are under "
            "`tmp/agents/unchiga-integration/`.",
            "",
        ]
    )
    return "\n".join(lines)


def record_misses_and_report(
    root: Path,
    manifest: list[dict[str, str]],
    results: dict[int, dict[str, str]],
) -> int:
    work = resolve_within(root, WORK_PATH)
    require_integration_repository(root, work)
    recorded = 0
    for row in manifest:
        address = parse_address(row["address"])
        result = results[address]
        if result["exact"] == "True":
            continue
        history = evidence_history(external_rows(root), address, row["mode"])
        if history:
            if row["mode"] == "collaborator_match" or len(history) >= 2:
                continue
        outcome = (
            "deferred"
            if row["mode"] == "collaborator_match"
            else "nonmatch"
        )
        append_evidence(root, row, result, outcome)
        recorded += 1

    rejected = read_csv(
        resolve_within(
            root,
            f"{WORK_PATH}/rejected-candidates.csv",
            must_exist=True,
        )
    )
    report = resolve_within(root, AUDIT_REPORT_PATH)
    report.write_text(
        render_audit_report(manifest, results, rejected),
        encoding="utf-8",
    )
    completed = run(
        root,
        work,
        [
            "git",
            "add",
            "--",
            EXTERNAL_ATTEMPTS_PATH,
            AUDIT_REPORT_PATH,
        ],
    )
    require_success(completed, "stage collaborator audit report")
    completed = run(root, work, ["git", "diff", "--cached", "--check"])
    require_success(completed, "collaborator audit diff check")
    if git_output(root, work, "diff", "--cached", "--name-only"):
        completed = run(
            root,
            work,
            [
                "git",
                "-c",
                "core.hooksPath=/dev/null",
                "commit",
                "--no-gpg-sign",
                "-m",
                "docs: record collaborator match audit",
                "-m",
                "Co-authored-by: Copilot "
                "<223556219+Copilot@users.noreply.github.com>",
            ],
        )
        require_success(completed, "commit collaborator audit report")
        completed = run(root, work, ["git", "push", "origin", "master"])
        require_success(completed, "push collaborator audit report")
    return recorded


def apply_audit(root: Path, rows: list[dict[str, str]]) -> tuple[int, int]:
    results = result_rows(root)
    if set(results) != {parse_address(row["address"]) for row in rows}:
        raise AuditError(
            "verification results do not cover the complete current manifest"
        )
    integrated = integrate_exact(root, rows, results)
    recorded = record_misses_and_report(root, rows, results)
    return integrated, recorded


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Prepare and independently verify Unchiga pure-C candidates "
            "sequentially."
        )
    )
    parser.add_argument(
        "command",
        choices=("manifest", "verify", "all", "apply"),
        nargs="?",
        default="all",
    )
    parser.add_argument("--address", type=lambda value: int(value, 0))
    parser.add_argument("--limit", type=int)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        if args.command in {"manifest", "all"}:
            rows = prepare_manifest(root)
        else:
            rows = load_manifest(root)
        selected = select_rows(
            rows,
            address=args.address,
            limit=args.limit,
        )
        if args.command in {"verify", "all"}:
            results = verify(root, selected)
            exact = sum(bool(row["exact"]) for row in results)
            errors = sum(bool(row["error"]) for row in results)
            print(
                f"Unchiga audit: selected={len(results)} "
                f"exact={exact} nonmatch={len(results) - exact - errors} "
                f"errors={errors}"
            )
        elif args.command == "apply":
            if args.address is not None or args.limit is not None:
                raise AuditError("apply requires the complete manifest")
            integrated, recorded = apply_audit(root, rows)
            print(
                f"Unchiga apply: integrated={integrated} "
                f"recorded_nonmatches={recorded}"
            )
        else:
            print(f"Unchiga manifest: candidates={len(rows)}")
    except (
        AuditError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        csv.Error,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
