#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path

from workspace import WorkspaceError, require_workspace_root, resolve_within


class AttemptError(RuntimeError):
    pass


FIELDS = ("address", "attempt", "compiler", "flags", "result", "summary")
RESULTS = {"matched", "nonmatch", "deferred"}


def parse_address(value: str) -> int:
    try:
        address = int(value, 0)
    except ValueError as error:
        raise AttemptError(f"invalid function address: {value}") from error
    if address < 0:
        raise AttemptError(f"function address must be non-negative: {value}")
    return address


def load_function_addresses(path: Path) -> set[int]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if "address" not in (reader.fieldnames or ()):
            raise AttemptError(f"{path}: missing address field")
        return {parse_address(row["address"]) for row in reader}


def load_attempts(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != FIELDS:
            raise AttemptError(f"{path}: unexpected CSV fields")
        return list(reader)


def validate_history(rows: list[dict[str, str]], address: int) -> list[dict[str, str]]:
    selected = [row for row in rows if parse_address(row["address"]) == address]
    for expected, row in enumerate(selected, start=1):
        try:
            attempt = int(row["attempt"], 10)
        except ValueError as error:
            raise AttemptError(
                f"{row['address']}: invalid attempt number {row['attempt']}"
            ) from error
        if attempt != expected:
            raise AttemptError(
                f"{row['address']}: expected attempt {expected}, found {attempt}"
            )
        if row["result"] not in RESULTS:
            raise AttemptError(
                f"{row['address']}: unsupported result {row['result']}"
            )
    return selected


def write_attempts(path: Path, rows: list[dict[str, str]]) -> None:
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
        description=(
            "Record one decompilation attempt. Attempts are unbounded; a "
            "history ends only when it is recorded as matched or deferred."
        )
    )
    parser.add_argument("address", help="function address such as 0x80012345")
    parser.add_argument("--compiler", required=True, help="compiler identifier")
    parser.add_argument("--flags", required=True, help="compiler/source variant")
    parser.add_argument(
        "--result",
        choices=sorted(RESULTS),
        required=True,
        help="attempt outcome",
    )
    parser.add_argument(
        "--summary",
        required=True,
        help="concise result or mismatch reason",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        address = parse_address(args.address)
        functions_path = resolve_within(
            root,
            "config/slus_01411/functions.csv",
            must_exist=True,
        )
        attempts_path = resolve_within(
            root,
            "config/slus_01411/attempts.csv",
            must_exist=True,
        )
        if address not in load_function_addresses(functions_path):
            raise AttemptError(f"{address:#010x}: function is not in the inventory")

        rows = load_attempts(attempts_path)
        history = validate_history(rows, address)
        if history and history[-1]["result"] in {"matched", "deferred"}:
            raise AttemptError(
                f"{address:#010x}: attempts already ended with "
                f"{history[-1]['result']}"
            )
        attempt = len(history) + 1
        rows.append(
            {
                "address": f"0x{address:08X}",
                "attempt": str(attempt),
                "compiler": args.compiler,
                "flags": args.flags,
                "result": args.result,
                "summary": args.summary,
            }
        )
        write_attempts(attempts_path, rows)
    except (
        AttemptError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        csv.Error,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"attempt {attempt}: {address:#010x} {args.result}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
