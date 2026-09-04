#!/usr/bin/env python3

from __future__ import annotations

import argparse
import shutil
import sys
from pathlib import Path

from workspace import WorkspaceError, require_workspace_root, resolve_within


class CleanError(RuntimeError):
    pass


CLEAN_TARGETS = {
    "extract": "tmp/extract",
    "generated": "tmp/generated",
    "overlays": "tmp/overlays",
    "splat": "tmp/splat",
    "project-build": "tmp/project-build",
    "reports": "tmp/reports",
}


def remove_target(root: Path, name: str) -> None:
    relative_path = CLEAN_TARGETS[name]
    resolve_within(root, relative_path)
    path = root / relative_path
    if not path.exists() and not path.is_symlink():
        print(f"clean: absent {relative_path}")
        return
    if path.is_symlink() or path.is_file():
        path.unlink()
    elif path.is_dir():
        shutil.rmtree(path)
    else:
        raise CleanError(f"unsupported generated path type: {relative_path}")
    print(f"clean: removed {relative_path}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Remove only explicitly known generated project paths."
    )
    parser.add_argument(
        "targets",
        nargs="+",
        choices=sorted(CLEAN_TARGETS),
        help="generated path groups to remove",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        for target in dict.fromkeys(args.targets):
            remove_target(root, target)
    except (CleanError, WorkspaceError, OSError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
