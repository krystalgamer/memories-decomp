#!/usr/bin/env python3

from __future__ import annotations

import os
import sys
from pathlib import Path


class WorkspaceError(RuntimeError):
    pass


WORKSPACE_MARKERS = (
    ".git",
    "Makefile",
    "config/slus_01411/target.yaml",
)


def resolve_within(
    root: Path, relative_path: str | os.PathLike[str], *, must_exist: bool = False
) -> Path:
    path = Path(relative_path)
    if path.is_absolute():
        raise WorkspaceError(f"absolute paths are not allowed: {path}")

    try:
        resolved = (root / path).resolve(strict=must_exist)
    except FileNotFoundError as error:
        raise WorkspaceError(f"path does not exist: {path}") from error

    try:
        resolved.relative_to(root)
    except ValueError as error:
        raise WorkspaceError(f"path leaves the workspace: {path}") from error
    return resolved


def require_workspace_root() -> Path:
    root = Path.cwd().resolve(strict=True)
    missing = [
        marker
        for marker in WORKSPACE_MARKERS
        if not resolve_within(root, marker).exists()
    ]
    if missing:
        raise WorkspaceError(
            "run this command from the repository root; missing "
            + ", ".join(missing)
        )

    script = Path(__file__).resolve(strict=True)
    try:
        script.relative_to(root)
    except ValueError as error:
        raise WorkspaceError("project tools must execute from this workspace") from error
    return root


def local_environment(root: Path) -> dict[str, str]:
    paths = {
        "HOME": "tmp/home",
        "TMPDIR": "tmp",
        "XDG_CACHE_HOME": "tmp/cache",
        "PIP_CACHE_DIR": "tmp/pip-cache",
        "PYTHONPYCACHEPREFIX": "tmp/pycache",
        "NPM_CONFIG_CACHE": "tmp/npm-cache",
        "CARGO_HOME": "tools/environments/cargo",
        "RUSTUP_HOME": "tools/environments/rustup",
        "GOPATH": "tools/environments/go",
        "GOMODCACHE": "tools/environments/go/pkg/mod",
    }
    return {
        name: str(resolve_within(root, relative_path))
        for name, relative_path in paths.items()
    }


def main() -> int:
    try:
        root = require_workspace_root()
        environment = local_environment(root)
    except (WorkspaceError, OSError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"workspace: {root}")
    for name in sorted(environment):
        print(f"{name}={environment[name]}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
