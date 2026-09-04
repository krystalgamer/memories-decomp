#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import sys
from pathlib import Path
from typing import Any

from function_inventory import FIELDS, load_inventory
from workspace import WorkspaceError, require_workspace_root, resolve_within


class IntegrationError(RuntimeError):
    pass


ASM_PATTERN = re.compile(r"\b(?:asm|__asm|__asm__)\b")
COMMENT_PATTERN = re.compile(r"/\*.*?\*/|//[^\n]*", re.DOTALL)
REGISTER_PIN_PATTERN = re.compile(
    r"\bregister\b[^;]*?\b(?:asm|__asm|__asm__)\s*\(\s*\"[^\"]*\"\s*\)"
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


def uses_asm_extension(source: str, *, allow_register_pins: bool = False) -> bool:
    """Report use of a GCC asm extension.

    By default any asm extension is rejected. When ``allow_register_pins`` is
    set, `register` variables pinned to a hard register are permitted per
    issue #5, which accepts that narrow form for functions that are otherwise
    unmatchable. Statement-level inline assembly is rejected in both modes.
    """
    text = COMMENT_PATTERN.sub("", source)
    if allow_register_pins:
        text = REGISTER_PIN_PATTERN.sub("register", text)
    return ASM_PATTERN.search(text) is not None


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
        rows = [
            row
            for row in reader
            if parse_address(row["address"]) == address
            and (mode is None or row.get("mode") == mode)
        ]
    if not rows or rows[-1]["result"] != "matched":
        raise IntegrationError(
            f"{address:#010x}: latest recorded attempt is not matched"
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
        ),
        default="canonical",
        help="ledger containing the terminal matched result",
    )
    parser.add_argument(
        "--replace-existing",
        action="store_true",
        help="replace an existing matching source after inline refinement",
    )
    parser.add_argument(
        "--allow-register-pins",
        action="store_true",
        help=(
            "accept `register` variables pinned to a hard register; "
            "statement-level inline assembly is still rejected"
        ),
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
                            else "post_terminal_resolution"
                        )
                    )
                ),
            )
        profiles = load_json(profiles_path).get("profiles")
        if not isinstance(profiles, dict) or args.profile not in profiles:
            raise IntegrationError(f"unknown compiler profile: {args.profile}")

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
            if uses_asm_extension(
                source_text, allow_register_pins=args.allow_register_pins
            ):
                raise IntegrationError(
                    f"{address:#010x}: reference match still contains GCC asm "
                    "(pass --allow-register-pins to accept register pinning)"
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
