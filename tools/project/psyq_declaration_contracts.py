#!/usr/bin/env python3
from __future__ import annotations

import csv
from pathlib import Path
import sys

from unmatched_contracts import declarations


ROOT = Path(__file__).resolve().parents[2]
FUNCTIONS = Path("config/slus_01411/functions.csv")
PSYQ_ROOT = Path("src/psyq")
CONSUMER_ROOTS = (
    Path("src/game"),
    Path("src/candidates"),
    Path("src/overlays"),
)


def sdk_functions(root: Path) -> set[str]:
    with (root / FUNCTIONS).open(newline="", encoding="utf-8") as handle:
        return {
            row["name"]
            for row in csv.DictReader(handle)
            if row["status"] == "sdk_asm"
        }


def source_files(root: Path, relative: Path):
    base = root / relative
    if not base.is_dir():
        return
    for path in sorted(base.rglob("*")):
        if path.suffix in {".c", ".h"}:
            yield path


def validate(root: Path = ROOT) -> tuple[list[str], dict[str, int]]:
    sdk = sdk_functions(root)
    errors: list[str] = []
    foreign_sites = 0
    header_symbols: set[str] = set()

    for path in source_files(root, PSYQ_ROOT):
        if path.suffix != ".h":
            continue
        for name, _ in declarations(
            path.read_text(encoding="utf-8", errors="surrogateescape")
        ):
            if name in sdk:
                header_symbols.add(name)

    for relative in CONSUMER_ROOTS:
        for path in source_files(root, relative):
            for name, statement in declarations(
                path.read_text(encoding="utf-8", errors="surrogateescape")
            ):
                if name not in sdk:
                    continue
                foreign_sites += 1
                errors.append(
                    f"{path.relative_to(root)}: Psy-Q function {name} must be "
                    f"declared by a header under {PSYQ_ROOT}: {statement}"
                )

    return errors, {
        "sdk_functions": len(sdk),
        "header_symbols": len(header_symbols),
        "foreign_sites": foreign_sites,
    }


def main() -> int:
    errors, stats = validate()
    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1
    print(
        "Psy-Q declarations: OK "
        f"({stats['sdk_functions']} SDK functions, "
        f"{stats['header_symbols']} declared through Psy-Q headers, "
        f"{stats['foreign_sites']} foreign sites)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
