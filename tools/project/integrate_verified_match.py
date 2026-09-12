#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any

from function_inventory import FIELDS, load_inventory
from record_external_attempt import ExternalAttemptError, latest_successes
from workspace import WorkspaceError, require_workspace_root, resolve_within


class IntegrationError(RuntimeError):
    pass


ASM_PATTERN = re.compile(r"\b(?:asm|__asm|__asm__)\b")
REGISTER_PIN_CODE_PATTERN = re.compile(
    r"\bregister\b[^;=]*?\b(?:asm|__asm|__asm__)\s*\(\s*\)"
)
REGISTER_AGGREGATE_PIN_CODE_PATTERN = re.compile(
    r"\bregister\s+(?:struct|union)\s*"
    r"\{(?:[^{}]|\{[^{}]*\})*\}\s*[A-Za-z_][A-Za-z0-9_]*\s*"
    r"(?:asm|__asm|__asm__)\s*\(\s*\)"
)
SYMBOL_ALIAS_PATTERN = re.compile(
    r"(?P<declaration>\bextern\b[^;]*?)\b(?:asm|__asm|__asm__)"
    r"\s*\(\s*\"(?P<symbol>(?:\\.|[^\"\\])*)\"\s*\)"
    r"(?:\s*__attribute__\s*\(\([^;]*?\)\))*\s*;"
)
SYMBOL_DEFINITION_PATTERN = re.compile(
    r"^\s*([A-Za-z_.$][A-Za-z0-9_.$]*)\s*=", re.MULTILINE
)
TRACKED_SYMBOL_PATHS = (
    "config/slus_01411/symbols.txt",
    "config/slus_01411/c_symbols.ld",
    "config/slus_01411/link_symbols.ld",
)
EXTERNAL_FIELDS = (
    "mode",
    "address",
    "attempt",
    "reference_path",
    "reference_sha256",
    "profile",
    "candidate_source",
    "candidate_sha256",
    "result",
    "summary",
)
G_FLAG = re.compile(r"^-G(?P<value>\d+)$")


def splice_c_lines(source: str) -> str:
    return re.sub(r"\\\r?\n", "", source)


def strip_c_comments(source: str) -> str:
    source = splice_c_lines(source)
    result: list[str] = []
    index = 0
    state = "code"
    while index < len(source):
        char = source[index]
        following = source[index + 1] if index + 1 < len(source) else ""
        if state == "code":
            if char == "/" and following == "/":
                result.extend((" ", " "))
                index += 2
                state = "line_comment"
                continue
            if char == "/" and following == "*":
                result.extend((" ", " "))
                index += 2
                state = "block_comment"
                continue
            result.append(char)
            if char == '"':
                state = "string"
            elif char == "'":
                state = "character"
        elif state == "line_comment":
            if char == "\n":
                result.append(char)
                state = "code"
            else:
                result.append(" ")
        elif state == "block_comment":
            if char == "*" and following == "/":
                result.extend((" ", " "))
                index += 2
                state = "code"
                continue
            result.append("\n" if char == "\n" else " ")
        else:
            result.append(char)
            if char == "\\" and following:
                result.append(following)
                index += 2
                continue
            if (state == "string" and char == '"') or (
                state == "character" and char == "'"
            ):
                state = "code"
        index += 1
    return "".join(result)


def mask_c_literals(source: str) -> str:
    result: list[str] = []
    index = 0
    state = "code"
    while index < len(source):
        char = source[index]
        if state == "code":
            if char == '"':
                result.append(" ")
                state = "string"
            elif char == "'":
                result.append(" ")
                state = "character"
            else:
                result.append(char)
        else:
            if char == "\\" and index + 1 < len(source):
                result.extend((" ", " "))
                index += 2
                continue
            result.append("\n" if char == "\n" else " ")
            if (state == "string" and char == '"') or (
                state == "character" and char == "'"
            ):
                state = "code"
        index += 1
    return "".join(result)


def register_pin_spans(source: str) -> list[tuple[int, int]]:
    code = mask_c_literals(source)
    spans = [
        match.span()
        for pattern in (
            REGISTER_AGGREGATE_PIN_CODE_PATTERN,
            REGISTER_PIN_CODE_PATTERN,
        )
        for match in pattern.finditer(code)
    ]
    spans.sort()
    return [
        span
        for index, span in enumerate(spans)
        if index == 0 or span[0] >= spans[index - 1][1]
    ]


def contains_register_pin(source: str) -> bool:
    return bool(register_pin_spans(strip_c_comments(source)))


def mask_spans(source: str, spans: list[tuple[int, int]]) -> str:
    output = list(source)
    for start, end in spans:
        for index in range(start, end):
            if output[index] != "\n":
                output[index] = " "
    return "".join(output)


def profile_g_value(flags: Any, description: str) -> int:
    if not isinstance(flags, list) or not all(isinstance(flag, str) for flag in flags):
        raise IntegrationError(f"{description}: expected a string list")
    values: list[int] = []
    for index, flag in enumerate(flags):
        match = G_FLAG.fullmatch(flag)
        if match is not None:
            values.append(int(match.group("value")))
        elif flag == "-G":
            if index + 1 >= len(flags) or not flags[index + 1].isdigit():
                raise IntegrationError(
                    f"{description}: -G must be followed by a non-negative integer"
                )
            values.append(int(flags[index + 1]))
    if len(values) != 1:
        raise IntegrationError(
            f"{description}: expected exactly one -G value, found {len(values)}"
        )
    return values[0]


def validate_effective_profile(profile: Any, profile_name: str) -> None:
    if not isinstance(profile, dict):
        raise IntegrationError(f"invalid compiler profile: {profile_name}")
    compiler_g = profile_g_value(
        profile.get("compiler_flags"), f"{profile_name} compiler_flags"
    )
    maspsx_g = profile_g_value(
        profile.get("maspsx_flags"), f"{profile_name} maspsx_flags"
    )
    if compiler_g != maspsx_g:
        raise IntegrationError(
            f"profile {profile_name} mixes compiler -G{compiler_g} "
            f"with MASPSX -G{maspsx_g}"
        )


def preprocess_source(root: Path, source: Path, profile: Any) -> str:
    if not isinstance(profile, dict):
        raise IntegrationError("invalid compiler profile")
    compiler_value = profile.get("compiler")
    flags = profile.get("compiler_flags")
    if (
        not isinstance(compiler_value, str)
        or not isinstance(flags, list)
        or not all(isinstance(flag, str) for flag in flags)
    ):
        raise IntegrationError("invalid compiler profile")
    compiler = resolve_within(root, compiler_value, must_exist=True)
    result = subprocess.run(
        [str(compiler), "-E", "-P", *flags, str(source)],
        cwd=root,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if result.returncode:
        details = " ".join(result.stderr.split())
        raise IntegrationError(f"source preprocessing failed: {details[:500]}")
    return result.stdout


def parse_address(value: str) -> int:
    try:
        return int(value, 0)
    except ValueError as error:
        raise IntegrationError(f"invalid function address: {value}") from error


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def declared_symbol(declaration: str) -> str | None:
    declaration = re.sub(
        r"\b(?:asm|__asm|__asm__)\s*\(\s*\"(?:\\.|[^\"\\])*\"\s*\)",
        "",
        declaration,
        flags=re.DOTALL,
    )
    declaration = re.sub(
        r"\b__attribute__\s*\(\((?:[^()]|\([^()]*\))*\)\)",
        "",
        declaration,
        flags=re.DOTALL,
    )
    pointer = re.search(r"\(\s*\*+\s*([A-Za-z_]\w*)\s*\)", declaration)
    if pointer is not None:
        return pointer.group(1)
    function = re.search(r"\b([A-Za-z_]\w*)\s*\(", declaration)
    if function is not None:
        return function.group(1)
    declaration = re.sub(r"(?:\[[^\]]*\]\s*)+$", "", declaration.strip())
    variable = re.search(r"([A-Za-z_]\w*)\s*$", declaration)
    return variable.group(1) if variable is not None else None


def load_tracked_symbol_names(root: Path) -> set[str]:
    names: set[str] = set()
    for relative in TRACKED_SYMBOL_PATHS:
        path = resolve_within(root, relative, must_exist=True)
        names.update(SYMBOL_DEFINITION_PATTERN.findall(path.read_text()))
    inventory_paths = [
        resolve_within(root, "config/slus_01411/functions.csv", must_exist=True),
        *sorted(
            resolve_within(root, "config/slus_01411/overlays", must_exist=True).glob(
                "*_functions.csv"
            )
        ),
    ]
    for path in inventory_paths:
        with path.open(encoding="utf-8", newline="") as handle:
            for row in csv.DictReader(handle):
                name = row.get("name")
                if name:
                    names.add(name)
    for path in sorted(resolve_within(root, "src", must_exist=True).rglob("*.h")):
        try:
            source = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        text = strip_c_comments(source)
        for declaration in re.findall(r"\bextern\b([^;]+);", text, re.DOTALL):
            name = declared_symbol(declaration)
            if name is not None:
                names.add(name)
    return names


def uses_asm_extension(
    source: str,
    *,
    allow_register_pins: bool = False,
    allow_symbol_aliases: bool = False,
    tracked_symbol_names: set[str] | None = None,
) -> bool:
    """Report use of a GCC asm extension.

    Register pins and extern symbol aliases can be permitted independently.
    Statement-level inline assembly is always rejected.
    """
    text = strip_c_comments(source)
    code = mask_c_literals(text)
    if allow_register_pins:
        code = mask_spans(code, register_pin_spans(text))
    if allow_symbol_aliases:
        allowed = tracked_symbol_names or set()
        alias_spans = []
        for match in SYMBOL_ALIAS_PATTERN.finditer(text):
            declaration = code[
                match.start("declaration"):match.end("declaration")
            ]
            if (
                declared_symbol(declaration) is not None
                and match.group("symbol") in allowed
                and ASM_PATTERN.search(code[match.start():match.end()]) is not None
            ):
                alias_spans.append(match.span())
        code = mask_spans(code, alias_spans)
    return ASM_PATTERN.search(code) is not None


def load_json(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as handle:
        value = json.load(handle)
    if not isinstance(value, dict) or value.get("schema") != 1:
        raise IntegrationError(f"{path}: unsupported schema")
    return value


def require_matched_attempt(
    path: Path, address: int, *, mode: str | None = None
) -> dict[str, str]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if mode is not None and tuple(reader.fieldnames or ()) != EXTERNAL_FIELDS:
            raise IntegrationError(f"{path}: unexpected external CSV fields")
        address_rows = [
            row
            for row in reader
            if parse_address(row["address"]) == address
        ]
        rows = [row for row in address_rows if mode is None or row.get("mode") == mode]
    if not rows or rows[-1]["result"] != "matched":
        raise IntegrationError(
            f"{address:#010x}: latest recorded attempt is not matched"
        )
    if mode is not None and any(
        row["mode"] == "reclassification_match" for row in address_rows
    ):
        try:
            selected = latest_successes(address_rows).get(address)
        except ExternalAttemptError as error:
            raise IntegrationError(str(error)) from error
        if selected != rows[-1]:
            raise IntegrationError(
                f"{address:#010x}: requested evidence has been superseded"
            )
    return rows[-1]


def write_json(path: Path, value: dict[str, Any]) -> Path:
    temporary = path.with_name(f"{path.name}.tmp")
    temporary.write_text(
        json.dumps(value, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    return temporary


def write_inventory(path: Path, rows: list[dict[str, str]]) -> Path:
    temporary = path.with_name(f"{path.name}.tmp")
    with temporary.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=FIELDS, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
    return temporary


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Promote one logged and byte-verified function to matching C."
    )
    parser.add_argument("address", help="function address such as 0x80012345")
    parser.add_argument(
        "--source",
        required=True,
        help="verified temporary C source beneath tmp/",
    )
    parser.add_argument(
        "--destination",
        required=True,
        help="new tracked C source path beneath src/",
    )
    parser.add_argument(
        "--profile",
        required=True,
        help="compiler profile from compiler_profiles.json",
    )
    parser.add_argument(
        "--note",
        required=True,
        help="durable inventory note",
    )
    parser.add_argument(
        "--evidence-source",
        choices=(
            "canonical",
            "reference",
            "refinement",
            "collaborator",
            "post-terminal",
            "reclassification",
        ),
        default="canonical",
        help="ledger containing the terminal matched result",
    )
    parser.add_argument(
        "--replace-existing",
        action="store_true",
        help="replace an existing matching source after inline refinement",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    temporary_paths: list[Path] = []
    completed_replacements: list[tuple[Path, bytes | None]] = []
    try:
        root = require_workspace_root()
        if args.replace_existing != (args.evidence_source == "refinement"):
            raise IntegrationError(
                "--replace-existing requires --evidence-source refinement, "
                "and refinement evidence requires replacement"
            )
        address = parse_address(args.address)
        source = resolve_within(root, args.source, must_exist=True)
        destination = resolve_within(root, args.destination)
        source_root = resolve_within(root, "src", must_exist=True)
        temporary_root = resolve_within(root, "tmp", must_exist=True)
        try:
            source.relative_to(temporary_root)
        except ValueError as error:
            raise IntegrationError("verified source must be beneath tmp/") from error
        try:
            destination.relative_to(source_root)
        except ValueError as error:
            raise IntegrationError("destination must be beneath src/") from error
        if args.replace_existing:
            if not destination.is_file():
                raise IntegrationError(
                    f"replacement destination does not exist: {destination}"
                )
        elif destination.exists():
            raise IntegrationError(f"destination already exists: {destination}")

        functions_path = resolve_within(
            root, "config/slus_01411/functions.csv", must_exist=True
        )
        attempts_path = resolve_within(
            root, "config/slus_01411/attempts.csv", must_exist=True
        )
        external_attempts_path = resolve_within(
            root,
            "config/slus_01411/external_attempts.csv",
            must_exist=True,
        )
        matching_path = resolve_within(
            root, "config/slus_01411/matching_c.json", must_exist=True
        )
        profiles_path = resolve_within(
            root,
            "config/slus_01411/compiler_profiles.json",
            must_exist=True,
        )

        external_evidence: dict[str, str] | None = None
        if args.evidence_source == "canonical":
            require_matched_attempt(attempts_path, address)
        else:
            external_evidence = require_matched_attempt(
                external_attempts_path,
                address,
                mode=(
                    "reference_match"
                    if args.evidence_source == "reference"
                    else (
                        "inline_refinement"
                        if args.evidence_source == "refinement"
                        else (
                            "collaborator_match"
                            if args.evidence_source == "collaborator"
                            else (
                                "reclassification_match"
                                if args.evidence_source == "reclassification"
                                else "post_terminal_resolution"
                            )
                        )
                    )
                ),
            )
        profiles = load_json(profiles_path).get("profiles")
        if not isinstance(profiles, dict) or args.profile not in profiles:
            raise IntegrationError(f"unknown compiler profile: {args.profile}")
        profile = profiles[args.profile]
        validate_effective_profile(profile, args.profile)

        functions = load_inventory(functions_path)
        matches = [
            function for function in functions if function.address == address
        ]
        if len(matches) != 1:
            raise IntegrationError(
                f"{address:#010x}: expected one inventory function"
            )
        function = matches[0]
        if function.module != "game":
            raise IntegrationError(
                f"{address:#010x}: only game functions may be integrated"
            )
        if args.replace_existing:
            if function.status != "matching_c":
                raise IntegrationError(
                    f"{address:#010x}: refinement target is not matching C"
                )
        elif function.status == "matching_c":
            raise IntegrationError(f"{address:#010x}: already matching C")

        source_bytes = source.read_bytes()
        source_text = source_bytes.decode("utf-8")
        tracked_symbol_names = load_tracked_symbol_names(root)
        source_without_comments = strip_c_comments(source_text)
        if contains_register_pin(source_without_comments):
            raise IntegrationError(
                f"{address:#010x}: matching C cannot contain hard-register variables"
            )
        if uses_asm_extension(
            source_text,
            allow_register_pins=True,
            allow_symbol_aliases=True,
            tracked_symbol_names=tracked_symbol_names,
        ):
            raise IntegrationError(
                f"{address:#010x}: matching C contains statement-level GCC asm "
                "or an assembler alias that is not a tracked symbol"
            )
        preprocessed_text = preprocess_source(root, source, profile)
        preprocessed_without_comments = strip_c_comments(preprocessed_text)
        if contains_register_pin(preprocessed_without_comments):
            raise IntegrationError(
                f"{address:#010x}: expanded matching C cannot contain "
                "hard-register variables"
            )
        if uses_asm_extension(
            preprocessed_text,
            allow_register_pins=True,
            allow_symbol_aliases=True,
            tracked_symbol_names=tracked_symbol_names,
        ):
            raise IntegrationError(
                f"{address:#010x}: expanded matching C contains statement-level "
                "GCC asm or an assembler alias that is not a tracked symbol"
            )
        if external_evidence is not None:
            evidence_source = resolve_within(
                root,
                external_evidence["candidate_source"],
                must_exist=True,
            )
            if evidence_source != source:
                raise IntegrationError(
                    f"{address:#010x}: source differs from matched reference evidence"
                )
            if external_evidence["profile"] != args.profile:
                raise IntegrationError(
                    f"{address:#010x}: profile differs from matched reference evidence"
                )
            if sha256(source) != external_evidence["candidate_sha256"]:
                raise IntegrationError(
                    f"{address:#010x}: source hash differs from matched reference evidence"
                )
        definition_pattern = re.compile(
            rf"\b{re.escape(function.name)}\s*\("
        )
        if definition_pattern.search(source_text) is None:
            raise IntegrationError(
                f"{source}: does not define expected symbol {function.name}"
            )
        if external_evidence is None and not source_text.endswith("\n"):
            source_text += "\n"
            output_bytes = source_text.encode("utf-8")
        else:
            output_bytes = source_bytes

        matching = load_json(matching_path)
        entries = matching.get("functions")
        if not isinstance(entries, list):
            raise IntegrationError(f"{matching_path}: functions must be a list")
        existing_entries = [
            entry
            for entry in entries
            if isinstance(entry, dict)
            and parse_address(str(entry.get("address"))) == address
        ]
        destination_relative = str(destination.relative_to(root))
        if args.replace_existing:
            if len(existing_entries) != 1:
                raise IntegrationError(
                    f"{address:#010x}: expected one matching manifest entry"
                )
            entry = existing_entries[0]
            source_value = entry.get("source")
            source_owners = sum(
                1
                for candidate_entry in entries
                if isinstance(candidate_entry, dict)
                and candidate_entry.get("source") == source_value
            )
            if source_owners != 1:
                raise IntegrationError(
                    f"{address:#010x}: cannot replace one function inside "
                    "a grouped translation unit"
                )
            if entry.get("source") != destination_relative:
                raise IntegrationError(
                    f"{address:#010x}: destination differs from matching manifest"
                )
            entry["size"] = f"0x{function.size:X}"
            entry["profile"] = args.profile
        else:
            if existing_entries:
                raise IntegrationError(
                    f"{address:#010x}: already in matching manifest"
                )
            entries.append(
                {
                    "address": f"0x{address:08X}",
                    "size": f"0x{function.size:X}",
                    "source": destination_relative,
                    "profile": args.profile,
                }
            )
        entries.sort(key=lambda entry: parse_address(str(entry["address"])))

        inventory_rows: list[dict[str, str]] = []
        with functions_path.open("r", encoding="utf-8", newline="") as handle:
            reader = csv.DictReader(handle)
            for row in reader:
                if parse_address(row["address"]) == address:
                    row["status"] = "matching_c"
                    row["notes"] = args.note
                inventory_rows.append(row)

        destination.parent.mkdir(parents=True, exist_ok=True)
        source_temporary = destination.with_name(f"{destination.name}.tmp")
        source_temporary.write_bytes(output_bytes)
        temporary_paths.append(source_temporary)
        matching_temporary = write_json(matching_path, matching)
        temporary_paths.append(matching_temporary)
        inventory_temporary = write_inventory(functions_path, inventory_rows)
        temporary_paths.append(inventory_temporary)

        for temporary, target in (
            (source_temporary, destination),
            (matching_temporary, matching_path),
            (inventory_temporary, functions_path),
        ):
            original = target.read_bytes() if target.exists() else None
            temporary.replace(target)
            temporary_paths.remove(temporary)
            completed_replacements.append((target, original))
    except (
        IntegrationError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        csv.Error,
        json.JSONDecodeError,
    ) as error:
        for path in temporary_paths:
            path.unlink(missing_ok=True)
        for path, original in reversed(completed_replacements):
            if original is None:
                path.unlink(missing_ok=True)
            else:
                path.write_bytes(original)
        print(f"error: {error}", file=sys.stderr)
        return 1

    print(
        f"integrated: {address:#010x} -> "
        f"{destination.relative_to(root)}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
