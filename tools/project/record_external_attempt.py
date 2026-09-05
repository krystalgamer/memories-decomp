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
from pathlib import PurePosixPath
from typing import Any

from workspace import WorkspaceError, require_workspace_root, resolve_within


class ExternalAttemptError(RuntimeError):
    pass


FIELDS = (
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
MODES = {
    "reference_match",
    "inline_refinement",
    "collaborator_match",
    "post_terminal_resolution",
}
RESULTS = {"matched", "nonmatch", "deferred"}
TERMINAL_RESULTS = {"matched", "deferred"}
MAX_ATTEMPTS = 6
MODE_MAX_ATTEMPTS = {
    "reference_match": MAX_ATTEMPTS,
    "inline_refinement": MAX_ATTEMPTS,
    "collaborator_match": 1,
    "post_terminal_resolution": 1,
}
SHA256_PATTERN = re.compile(r"^[0-9a-f]{64}$")
ASM_PATTERN = re.compile(r"\b(?:asm|__asm|__asm__)\b")
COMMENT_PATTERN = re.compile(r"/\*.*?\*/|//[^\n]*", re.DOTALL)
REGISTER_PIN_PATTERN = re.compile(
    r"\bregister\b[^;]*?\b(?:asm|__asm|__asm__)\s*\(\s*\"[^\"]*\"\s*\)"
)


def parse_address(value: str) -> int:
    try:
        address = int(value, 0)
    except ValueError as error:
        raise ExternalAttemptError(f"invalid function address: {value}") from error
    if address < 0:
        raise ExternalAttemptError(f"negative function address: {value}")
    return address


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def uses_asm_extension(source: str, *, allow_register_pins: bool = False) -> bool:
    """Report use of a GCC asm extension.

    By default any asm extension is rejected, keeping these ledgers pure C.
    When ``allow_register_pins`` is set, `register` variables pinned to a hard
    register are permitted per issue #5, which accepts that narrow form for
    functions that are otherwise unmatchable. Statement-level inline assembly
    is still rejected in both modes.
    """
    text = COMMENT_PATTERN.sub("", source)
    if allow_register_pins:
        text = REGISTER_PIN_PATTERN.sub("register", text)
    return ASM_PATTERN.search(text) is not None


def load_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def load_functions(path: Path) -> dict[int, dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        return {parse_address(row["address"]): row for row in reader}


def load_matching_addresses(path: Path) -> set[int]:
    value = load_json(path)
    functions = value.get("functions") if isinstance(value, dict) else None
    if not isinstance(functions, list):
        raise ExternalAttemptError(f"{path}: missing functions list")
    return {
        parse_address(str(entry["address"]))
        for entry in functions
        if isinstance(entry, dict)
    }


def load_terminal_deferred_addresses(path: Path) -> set[int]:
    latest: dict[int, str] = {}
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            latest[parse_address(row["address"])] = row["result"]
    return {
        address for address, result in latest.items() if result == "deferred"
    }


def terminal_resolution_addresses(
    rows: list[dict[str, str]],
    canonical_deferred_addresses: set[int],
) -> set[int]:
    inline_latest: dict[int, str] = {}
    for row in rows:
        if row["mode"] == "inline_refinement":
            inline_latest[parse_address(row["address"])] = row["result"]
    return canonical_deferred_addresses | {
        address
        for address, result in inline_latest.items()
        if result == "deferred"
    }


def load_profiles(path: Path) -> dict[str, dict[str, Any]]:
    value = load_json(path)
    profiles = value.get("profiles") if isinstance(value, dict) else None
    if not isinstance(profiles, dict):
        raise ExternalAttemptError(f"{path}: missing profiles object")
    if not all(
        isinstance(name, str) and isinstance(profile, dict)
        for name, profile in profiles.items()
    ):
        raise ExternalAttemptError(f"{path}: malformed profiles object")
    return profiles


def require_tmp_path(value: str, context: str) -> None:
    path = PurePosixPath(value)
    if (
        path.is_absolute()
        or not path.parts
        or path.parts[0] != "tmp"
        or ".." in path.parts
    ):
        raise ExternalAttemptError(f"{context}: path must normalize beneath tmp/")


def expected_reference_path(
    mode: str,
    address: int,
    value: str,
) -> bool:
    if mode == "reference_match":
        return value == (
            "tmp/references/ygofm-decomp/src/"
            f"func_{address:08X}.c"
        )
    if mode == "collaborator_match":
        path = PurePosixPath(value)
        return (
            not path.is_absolute()
            and ".." not in path.parts
            and path.parts[:4]
            == ("tmp", "references", "ygofm-decomp-unchiga", "src")
            and path.suffix == ".c"
        )
    return True


def preprocess_candidate(
    root: Path,
    candidate: Path,
    profile: dict[str, Any],
) -> str:
    compiler_value = profile.get("compiler")
    flags = profile.get("compiler_flags")
    if (
        not isinstance(compiler_value, str)
        or not isinstance(flags, list)
        or not all(isinstance(flag, str) for flag in flags)
    ):
        raise ExternalAttemptError("invalid compiler profile")
    compiler = resolve_within(root, compiler_value, must_exist=True)
    result = subprocess.run(
        [str(compiler), "-E", "-P", *flags, str(candidate)],
        cwd=root,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if result.returncode:
        details = " ".join(result.stderr.split())
        raise ExternalAttemptError(
            f"candidate preprocessing failed: {details[:500]}"
        )
    return result.stdout


def load_rows(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != FIELDS:
            raise ExternalAttemptError(f"{path}: unexpected CSV fields")
        return list(reader)


def validate_rows(
    rows: list[dict[str, str]],
    functions: dict[int, dict[str, str]],
    matching_addresses: set[int],
    profiles: dict[str, dict[str, Any]],
    terminal_deferred_addresses: set[int],
) -> None:
    resolution_addresses = terminal_resolution_addresses(
        rows,
        terminal_deferred_addresses,
    )
    grouped: dict[tuple[str, int], list[dict[str, str]]] = {}
    for row in rows:
        mode = row["mode"]
        address = parse_address(row["address"])
        if mode not in MODES:
            raise ExternalAttemptError(
                f"{address:#010x}: unsupported external mode {mode}"
            )
        function = functions.get(address)
        if function is None:
            raise ExternalAttemptError(
                f"{address:#010x}: external attempt references unknown function"
            )
        if function["module"] != "game":
            raise ExternalAttemptError(
                f"{address:#010x}: external attempts are game-only"
            )
        if row["profile"] not in profiles:
            raise ExternalAttemptError(
                f"{address:#010x}: unknown compiler profile {row['profile']}"
            )
        if row["result"] not in RESULTS:
            raise ExternalAttemptError(
                f"{address:#010x}: unsupported result {row['result']}"
            )
        if not row["summary"]:
            raise ExternalAttemptError(f"{address:#010x}: empty summary")
        require_tmp_path(
            row["candidate_source"],
            f"{address:#010x}: candidate source",
        )
        if not SHA256_PATTERN.fullmatch(row["candidate_sha256"]):
            raise ExternalAttemptError(
                f"{address:#010x}: invalid candidate SHA-256"
            )
        has_reference = bool(row["reference_path"] or row["reference_sha256"])
        if bool(row["reference_path"]) != bool(row["reference_sha256"]):
            raise ExternalAttemptError(
                f"{address:#010x}: incomplete reference path/hash pair"
            )
        if mode in {"reference_match", "collaborator_match"} and not has_reference:
            raise ExternalAttemptError(
                f"{address:#010x}: {mode} requires a reference source"
            )
        if has_reference:
            if not expected_reference_path(
                mode,
                address,
                row["reference_path"],
            ):
                raise ExternalAttemptError(
                    f"{address:#010x}: invalid {mode} reference path"
                )
            if not SHA256_PATTERN.fullmatch(row["reference_sha256"]):
                raise ExternalAttemptError(
                    f"{address:#010x}: invalid reference SHA-256"
                )
        if mode == "inline_refinement" and address not in matching_addresses:
            raise ExternalAttemptError(
                f"{address:#010x}: inline refinement requires matching C"
            )
        if mode == "post_terminal_resolution":
            if address not in resolution_addresses:
                raise ExternalAttemptError(
                    f"{address:#010x}: post-terminal resolution lacks a "
                    "deferred canonical or inline-refinement history"
                )
            if row["result"] != "matched":
                raise ExternalAttemptError(
                    f"{address:#010x}: post-terminal resolution must be matched"
                )
        grouped.setdefault((mode, address), []).append(row)

    for (mode, address), history in grouped.items():
        maximum = MODE_MAX_ATTEMPTS[mode]
        if len(history) > maximum:
            raise ExternalAttemptError(
                f"{address:#010x}: exceeds {maximum} {mode} attempt(s)"
            )
        ended = False
        for expected, row in enumerate(history, start=1):
            try:
                attempt = int(row["attempt"], 10)
            except ValueError as error:
                raise ExternalAttemptError(
                    f"{address:#010x}: invalid attempt number {row['attempt']}"
                ) from error
            if attempt != expected:
                raise ExternalAttemptError(
                    f"{address:#010x}: expected attempt {expected}, "
                    f"found {attempt}"
                )
            if ended:
                raise ExternalAttemptError(
                    f"{address:#010x}: row follows terminal external result"
                )
            if row["result"] in TERMINAL_RESULTS:
                ended = True
            if (
                maximum == MAX_ATTEMPTS
                and expected == MAX_ATTEMPTS
                and row["result"] == "nonmatch"
            ):
                raise ExternalAttemptError(
                    f"{address:#010x}: sixth external attempt must be deferred"
                )


def sort_key(row: dict[str, str]) -> tuple[int, str, int]:
    return (
        parse_address(row["address"]),
        row["mode"],
        int(row["attempt"], 10),
    )


def sort_rows(rows: list[dict[str, str]]) -> list[dict[str, str]]:
    """Order the ledger by address, then mode, then attempt.

    The file is keyed by address and the attempt column carries sequence within
    a function, so chronological order across unrelated functions carries no
    information. Sorting by address means two concurrent matches touch
    different parts of the file instead of both appending to the tail.

    This ordering is safe for the existing consumers because a `matched` row is
    unique per address: `audit_repository.py` selects the latest success by
    address alone, without filtering on mode, and only a filter that keeps at
    most one row per key can be insensitive to order. Recording a second
    `matched` row for one address, under any mode, would break that and would
    silently change which row `make audit` validates.

    Note also that "last row for an address" no longer means "most recent
    event". Under the previous chronological order it did, and a new
    mode-agnostic consumer reaching for that meaning would now read different
    data.
    """
    return sorted(rows, key=sort_key)


def check_sorted(rows: list[dict[str, str]]) -> None:
    expected = sort_rows(rows)
    if [row["address"] for row in rows] != [row["address"] for row in expected]:
        raise ExternalAttemptError(
            "ledger is not ordered by address; rewrite it with "
            "record_external_attempt.py"
        )


def write_rows(path: Path, rows: list[dict[str, str]]) -> None:
    rows = sort_rows(rows)
    temporary = path.with_name(f"{path.name}.tmp")
    try:
        with temporary.open("w", encoding="utf-8", newline="") as handle:
            writer = csv.DictWriter(handle, fieldnames=FIELDS, lineterminator="\n")
            writer.writeheader()
            writer.writerows(rows)
        temporary.replace(path)
    except OSError:
        temporary.unlink(missing_ok=True)
        raise


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Record a pure-C external-reference or refinement attempt."
    )
    parser.add_argument("address", nargs="?")
    parser.add_argument("--mode", choices=sorted(MODES))
    parser.add_argument("--reference")
    parser.add_argument("--profile")
    parser.add_argument("--candidate")
    parser.add_argument("--result", choices=sorted(RESULTS))
    parser.add_argument("--summary")
    parser.add_argument(
        "--ledger",
        default="config/slus_01411/external_attempts.csv",
        help="ledger path relative to the repository root",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="validate the selected ledger without writing",
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
    try:
        root = require_workspace_root()
        functions_path = resolve_within(
            root, "config/slus_01411/functions.csv", must_exist=True
        )
        matching_path = resolve_within(
            root, "config/slus_01411/matching_c.json", must_exist=True
        )
        profiles_path = resolve_within(
            root, "config/slus_01411/compiler_profiles.json", must_exist=True
        )
        attempts_path = resolve_within(
            root, "config/slus_01411/attempts.csv", must_exist=True
        )
        ledger_path = resolve_within(root, args.ledger, must_exist=True)
        functions = load_functions(functions_path)
        matching_addresses = load_matching_addresses(matching_path)
        terminal_deferred_addresses = load_terminal_deferred_addresses(
            attempts_path
        )
        profiles = load_profiles(profiles_path)
        rows = load_rows(ledger_path)
        validate_rows(
            rows,
            functions,
            matching_addresses,
            profiles,
            terminal_deferred_addresses,
        )
        if args.check:
            check_sorted(rows)
            print(f"external attempts: OK ({len(rows)} rows)")
            return 0

        required = {
            "address": args.address,
            "mode": args.mode,
            "profile": args.profile,
            "candidate": args.candidate,
            "result": args.result,
            "summary": args.summary,
        }
        missing = [key for key, value in required.items() if not value]
        if missing:
            raise ExternalAttemptError(
                "missing arguments: " + ", ".join(missing)
            )

        address = parse_address(args.address)
        function = functions.get(address)
        if function is None:
            raise ExternalAttemptError(
                f"{address:#010x}: function is not in the inventory"
            )
        if function["module"] != "game":
            raise ExternalAttemptError(
                f"{address:#010x}: external attempts are game-only"
            )
        if args.mode in {"reference_match", "collaborator_match"}:
            if function["status"] == "matching_c":
                raise ExternalAttemptError(
                    f"{address:#010x}: use inline_refinement for matching C"
                )
            if not args.reference:
                raise ExternalAttemptError("--reference is required")
        elif args.mode == "inline_refinement":
            if address not in matching_addresses:
                raise ExternalAttemptError(
                    f"{address:#010x}: inline refinement requires matching C"
                )
        else:
            resolution_addresses = terminal_resolution_addresses(
                rows,
                terminal_deferred_addresses,
            )
            if address not in resolution_addresses:
                raise ExternalAttemptError(
                    f"{address:#010x}: post-terminal resolution requires a "
                    "deferred canonical or inline-refinement history"
                )

        candidate = resolve_within(root, args.candidate, must_exist=True)
        temporary_root = resolve_within(root, "tmp", must_exist=True)
        try:
            candidate.relative_to(temporary_root)
        except ValueError as error:
            raise ExternalAttemptError(
                "candidate source must be beneath tmp/"
            ) from error
        source_text = candidate.read_text(encoding="utf-8")
        preprocessed_text = preprocess_candidate(
            root, candidate, profiles[args.profile]
        )
        if (
            uses_asm_extension(
                source_text, allow_register_pins=args.allow_register_pins
            )
            or uses_asm_extension(
                preprocessed_text, allow_register_pins=args.allow_register_pins
            )
        ):
            raise ExternalAttemptError(
                f"{address:#010x}: candidate still uses a GCC asm extension "
                "(pass --allow-register-pins to accept register pinning)"
            )
        name = function["name"]
        if name not in source_text:
            raise ExternalAttemptError(
                f"{address:#010x}: candidate does not define {name}"
            )

        reference_path = ""
        reference_hash = ""
        if args.reference:
            reference = resolve_within(root, args.reference, must_exist=True)
            reference_value = str(reference.relative_to(root))
            if not expected_reference_path(
                args.mode,
                address,
                reference_value,
            ):
                raise ExternalAttemptError(
                    f"invalid {args.mode} reference path"
                )
            reference_path = reference_value
            reference_hash = sha256(reference)

        history = [
            row
            for row in rows
            if row["mode"] == args.mode
            and parse_address(row["address"]) == address
        ]
        if history and history[-1]["result"] in TERMINAL_RESULTS:
            raise ExternalAttemptError(
                f"{address:#010x}: external history already ended with "
                f"{history[-1]['result']}"
            )
        maximum = MODE_MAX_ATTEMPTS[args.mode]
        if len(history) >= maximum:
            raise ExternalAttemptError(
                f"{address:#010x}: {args.mode} attempt budget is exhausted"
            )
        attempt = len(history) + 1
        if (
            maximum == MAX_ATTEMPTS
            and attempt == MAX_ATTEMPTS
            and args.result == "nonmatch"
        ):
            raise ExternalAttemptError(
                "record the sixth unsuccessful attempt as deferred"
            )

        rows.append(
            {
                "mode": args.mode,
                "address": f"0x{address:08X}",
                "attempt": str(attempt),
                "reference_path": reference_path,
                "reference_sha256": reference_hash,
                "profile": args.profile,
                "candidate_source": str(candidate.relative_to(root)),
                "candidate_sha256": sha256(candidate),
                "result": args.result,
                "summary": args.summary,
            }
        )
        validate_rows(
            rows,
            functions,
            matching_addresses,
            profiles,
            terminal_deferred_addresses,
        )
        write_rows(ledger_path, rows)
    except (
        ExternalAttemptError,
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

    print(
        f"external attempt {attempt}/{maximum}: "
        f"{address:#010x} {args.mode} {args.result}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
