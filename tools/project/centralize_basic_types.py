#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import re
import sys
from pathlib import Path

from workspace import WorkspaceError, require_workspace_root, resolve_within

# `--check` reads only tracked sources under src/, so it resolves the root from
# this file. `require_workspace_root` insists on the ignored retail image, which
# would keep the check out of the metadata workflow -- the one job that runs
# without a toolchain or retail input. `--apply` rewrites files in place and
# keeps the guard, because there the cost of running from the wrong directory is
# real.
ROOT = Path(__file__).resolve().parents[2]


HEADER_PATH = "src/types.h"
SOURCE_ROOT = "src"
ALIASES = {
    "s8": {"signed char"},
    "u8": {"unsigned char"},
    "s16": {"short", "signed short"},
    "u16": {"unsigned short"},
    "s32": {"int", "signed int"},
    "u32": {"unsigned int"},
    "s64": {"signed long long"},
    "u64": {"unsigned long long"},
}
TYPEDEF_PATTERN = re.compile(
    r"\btypedef\s+"
    r"(?P<base>(?:signed\s+|unsigned\s+)?"
    r"(?:char|short|int|long\s+long)(?:\s+int)?)"
    r"\s+(?P<name>s8|u8|s16|u16|s32|u32|s64|u64)\s*;"
)
INCLUDE_PATTERN = re.compile(
    r'^\s*#\s*include\s+"(?P<path>[^"]+)"\s*$',
    re.MULTILINE,
)


class TypeHeaderError(RuntimeError):
    pass


def normalized_base(value: str) -> str:
    return " ".join(value.split())


def source_files(root: Path) -> list[Path]:
    source_root = resolve_within(root, SOURCE_ROOT, must_exist=True)
    return sorted(path for path in source_root.rglob("*.c") if path.is_file())


def expected_include(root: Path, source: Path) -> str:
    header = resolve_within(root, HEADER_PATH, must_exist=True)
    return Path(os.path.relpath(header, source.parent)).as_posix()


def remove_primitive_typedefs(text: str, source: Path) -> str:
    def replace(match: re.Match[str]) -> str:
        name = match.group("name")
        base = normalized_base(match.group("base"))
        if base not in ALIASES[name]:
            raise TypeHeaderError(
                f"{source}: unexpected definition for {name}: {base}"
            )
        return ""

    return TYPEDEF_PATTERN.sub(replace, text)


def update_source(root: Path, source: Path, text: str) -> str:
    include = expected_include(root, source)
    text = remove_primitive_typedefs(text, source)
    includes = [
        match.group("path")
        for match in INCLUDE_PATTERN.finditer(text)
        if Path(match.group("path")).name == "types.h"
    ]
    if includes and includes != [include]:
        raise TypeHeaderError(
            f"{source}: unexpected primitive type includes: {includes}"
        )
    if not includes:
        text = f'#include "{include}"\n\n' + text.lstrip("\n")
    text = re.sub(r"[ \t]+(?=\n|$)", "", text)
    text = re.sub(r"\n{3,}", "\n\n", text)
    if not text.endswith("\n"):
        text += "\n"
    return text


def validate_header(path: Path) -> None:
    text = path.read_text(encoding="utf-8")
    found: dict[str, str] = {}
    for match in TYPEDEF_PATTERN.finditer(text):
        found[match.group("name")] = normalized_base(match.group("base"))
    if set(found) != set(ALIASES):
        missing = sorted(set(ALIASES) - set(found))
        extra = sorted(set(found) - set(ALIASES))
        raise TypeHeaderError(
            f"{path}: primitive aliases differ; missing={missing}, extra={extra}"
        )
    for name, base in found.items():
        if base not in ALIASES[name]:
            raise TypeHeaderError(
                f"{path}: unexpected definition for {name}: {base}"
            )


def planned_updates(root: Path) -> dict[Path, str]:
    header = resolve_within(root, HEADER_PATH, must_exist=True)
    validate_header(header)
    updates: dict[Path, str] = {}
    for source in source_files(root):
        current = source.read_text(encoding="utf-8")
        updated = update_source(root, source, current)
        if updated != current:
            updates[source] = updated
    return updates


def write_updates(updates: dict[Path, str]) -> None:
    temporary_paths: list[tuple[Path, Path]] = []
    try:
        for source, text in updates.items():
            temporary = source.with_name(f"{source.name}.types.tmp")
            temporary.write_text(text, encoding="utf-8")
            temporary_paths.append((temporary, source))
        for temporary, source in temporary_paths:
            temporary.replace(source)
    finally:
        for temporary, _source in temporary_paths:
            temporary.unlink(missing_ok=True)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Use src/types.h for primitive integer aliases in every C source."
        )
    )
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--apply", action="store_true")
    action.add_argument("--check", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = ROOT if args.check else require_workspace_root()
        updates = planned_updates(root)
        if args.check:
            if updates:
                print(
                    f"basic types require updates in {len(updates)} source files",
                    file=sys.stderr,
                )
                return 1
            print(f"basic types: OK ({len(source_files(root))} C sources)")
            return 0
        write_updates(updates)
        print(f"centralized basic types in {len(updates)} source files")
    except (
        TypeHeaderError,
        WorkspaceError,
        OSError,
        UnicodeError,
        ValueError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
