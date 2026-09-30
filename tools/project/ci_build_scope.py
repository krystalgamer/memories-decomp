#!/usr/bin/env python3
"""Preserve the build workflows' existing documentation/trace path filters."""

from __future__ import annotations

import os
from pathlib import Path
import re
import subprocess
import sys

from workspace import WorkspaceError, require_workspace_root


class ScopeError(Exception):
    pass


def requires_code_builds(paths: list[str]) -> bool:
    return any(path != "README.md" and not path.startswith(("notes/", "tools/trace/"))
               for path in paths)


def code_builds(root: Path, event: str, base: str, head: str) -> bool:
    if event == "workflow_dispatch":
        return True
    if event not in ("push", "pull_request"):
        raise ScopeError(f"unsupported build event: {event}")
    if not re.fullmatch(r"[0-9a-fA-F]{40}", head):
        raise ScopeError("invalid build head SHA")
    if not base or base == "0" * 40:
        print("::notice::No previous commit; running all build gates.", file=sys.stderr)
        return True
    if not re.fullmatch(r"[0-9a-fA-F]{40}", base):
        raise ScopeError("invalid build base SHA")
    probe = subprocess.run(["git", "cat-file", "-e", base + "^{commit}"],
                           cwd=root, capture_output=True, check=False)
    if probe.returncode:
        print("::warning::Change base is unavailable; running all build gates.", file=sys.stderr)
        return True
    revision = base + ("..." if event == "pull_request" else "..") + head
    result = subprocess.run(["git", "diff", "--name-only", "-z", revision],
                            cwd=root, capture_output=True, check=False)
    if result.returncode:
        raise ScopeError(f"cannot inspect changed files (git diff exit {result.returncode})")
    paths = [os.fsdecode(path) for path in result.stdout.split(b"\0") if path]
    return requires_code_builds(paths)


def main() -> int:
    try:
        root = require_workspace_root()
        event = os.environ["BUILD_EVENT"]
        base = os.environ.get("PR_BASE_SHA" if event == "pull_request" else "PUSH_BASE_SHA", "")
        run = code_builds(root, event, base, os.environ.get("BUILD_HEAD_SHA", ""))
        print(f"code-builds={str(run).lower()}")
    except (OSError, KeyError, ScopeError, WorkspaceError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
