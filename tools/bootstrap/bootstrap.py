#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any

PROJECT_TOOLS = Path(__file__).resolve().parents[1] / "project"
sys.path.insert(0, str(PROJECT_TOOLS))

from workspace import (  # noqa: E402
    WorkspaceError,
    local_environment,
    require_workspace_root,
    resolve_within,
)


class BootstrapError(RuntimeError):
    pass


def run(
    root: Path,
    command: list[str],
    *,
    capture_output: bool = False,
) -> str:
    environment = os.environ.copy()
    environment.update(local_environment(root))
    try:
        result = subprocess.run(
            command,
            cwd=root,
            env=environment,
            check=True,
            text=True,
            stdout=subprocess.PIPE if capture_output else None,
            stderr=subprocess.PIPE if capture_output else None,
        )
    except subprocess.CalledProcessError as error:
        details = error.stderr.strip() if error.stderr else ""
        message = f"command failed with exit code {error.returncode}: {command[0]}"
        if details:
            message = f"{message}: {details}"
        raise BootstrapError(message) from error
    return result.stdout.strip() if capture_output else ""


def load_lock(root: Path) -> dict[str, Any]:
    path = resolve_within(root, "tools/bootstrap/tools.json", must_exist=True)
    with path.open("r", encoding="utf-8") as handle:
        lock = json.load(handle)
    if lock.get("schema") != 1:
        raise BootstrapError(f"{path}: unsupported lock schema")
    return lock


def ensure_directories(root: Path) -> None:
    for relative_path in (
        "tmp/bootstrap",
        "tmp/cache",
        "tmp/home",
        "tools/downloads/python/bootstrap",
        "tools/downloads/python/tools",
        "tools/environments",
        "tools/vendor",
    ):
        resolve_within(root, relative_path).mkdir(parents=True, exist_ok=True)


def local_python(root: Path, config: dict[str, Any]) -> Path:
    environment_path = str(config["environment"])
    return resolve_within(root, f"{environment_path}/bin/python")


def ensure_python_environment(
    root: Path, config: dict[str, Any], *, check_only: bool
) -> Path:
    expected_version = tuple(config["major_minor"])
    if (
        len(expected_version) != 2
        or not all(isinstance(part, int) for part in expected_version)
    ):
        raise BootstrapError("python major_minor must contain two integers")

    python = local_python(root, config)
    if not python.exists():
        if check_only:
            raise BootstrapError(f"missing local Python environment: {python}")
        if sys.version_info[:2] != expected_version:
            expected = ".".join(str(part) for part in expected_version)
            actual = f"{sys.version_info.major}.{sys.version_info.minor}"
            raise BootstrapError(
                f"bootstrap Python is {actual}; this lock requires {expected}"
            )
        environment = resolve_within(root, str(config["environment"]))
        run(root, [sys.executable, "-m", "venv", "--copies", str(environment)])

    python = resolve_within(
        root, f"{config['environment']}/bin/python", must_exist=True
    )
    actual_version = run(
        root,
        [
            str(python),
            "-c",
            "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')",
        ],
        capture_output=True,
    )
    expected = ".".join(str(part) for part in expected_version)
    if actual_version != expected:
        raise BootstrapError(
            f"local Python is {actual_version}; expected version {expected}"
        )
    return python


def install_requirements(
    root: Path,
    python: Path,
    requirements: str,
    download_directory: str,
) -> None:
    requirement_path = resolve_within(root, requirements, must_exist=True)
    downloads = resolve_within(root, download_directory)
    downloads.mkdir(parents=True, exist_ok=True)

    run(
        root,
        [
            str(python),
            "-m",
            "pip",
            "download",
            "--disable-pip-version-check",
            "--no-deps",
            "--require-hashes",
            "--dest",
            str(downloads),
            "--requirement",
            str(requirement_path),
        ],
    )
    run(
        root,
        [
            str(python),
            "-m",
            "pip",
            "install",
            "--disable-pip-version-check",
            "--no-input",
            "--no-deps",
            "--no-build-isolation",
            "--no-index",
            "--find-links",
            str(downloads),
            "--require-hashes",
            "--requirement",
            str(requirement_path),
        ],
    )


def check_distributions(
    root: Path, python: Path, expected: dict[str, str]
) -> None:
    script = (
        "import importlib.metadata, json, sys\n"
        "expected = json.loads(sys.argv[1])\n"
        "actual = {}\n"
        "for name in expected:\n"
        "    try:\n"
        "        actual[name] = importlib.metadata.version(name)\n"
        "    except importlib.metadata.PackageNotFoundError:\n"
        "        actual[name] = None\n"
        "print(json.dumps(actual, sort_keys=True))\n"
    )
    output = run(
        root,
        [str(python), "-c", script, json.dumps(expected, sort_keys=True)],
        capture_output=True,
    )
    actual = json.loads(output)
    errors = [
        f"{name}: {actual.get(name) or 'missing'} != {version}"
        for name, version in expected.items()
        if actual.get(name) != version
    ]
    if errors:
        raise BootstrapError("Python distribution mismatch: " + "; ".join(errors))


def check_required_paths(root: Path, paths: list[str]) -> None:
    for relative_path in paths:
        path = resolve_within(root, relative_path, must_exist=True)
        if not path.is_file():
            raise BootstrapError(f"required tool path is not a file: {relative_path}")


def git_output(root: Path, destination: Path, *arguments: str) -> str:
    return run(
        root,
        ["git", "-C", str(destination), *arguments],
        capture_output=True,
    )


def check_git_tool(root: Path, tool: dict[str, Any]) -> None:
    destination = resolve_within(root, str(tool["destination"]), must_exist=True)
    if not (destination / ".git").is_dir():
        raise BootstrapError(f"{tool['name']}: destination is not a Git checkout")

    actual_commit = git_output(root, destination, "rev-parse", "HEAD")
    if actual_commit != tool["commit"]:
        raise BootstrapError(
            f"{tool['name']}: checkout is {actual_commit}, expected {tool['commit']}"
        )

    actual_url = git_output(root, destination, "remote", "get-url", "origin")
    if actual_url != tool["url"]:
        raise BootstrapError(
            f"{tool['name']}: origin is {actual_url}, expected {tool['url']}"
        )

    status = git_output(root, destination, "status", "--porcelain")
    if status:
        raise BootstrapError(f"{tool['name']}: checkout has local changes")

    for relative_path in tool["required_paths"]:
        path = resolve_within(
            root,
            f"{tool['destination']}/{relative_path}",
            must_exist=True,
        )
        if not path.is_file():
            raise BootstrapError(
                f"{tool['name']}: missing required path {relative_path}"
            )


def install_git_tool(root: Path, tool: dict[str, Any]) -> None:
    destination = resolve_within(root, str(tool["destination"]))
    if destination.exists():
        check_git_tool(root, tool)
        return

    staging = resolve_within(root, f"tmp/bootstrap/{tool['name']}")
    if staging.exists():
        raise BootstrapError(
            f"stale bootstrap directory exists; remove it before retrying: {staging}"
        )

    staging.parent.mkdir(parents=True, exist_ok=True)
    run(root, ["git", "init", "--quiet", str(staging)])
    run(
        root,
        ["git", "-C", str(staging), "remote", "add", "origin", str(tool["url"])],
    )
    run(
        root,
        [
            "git",
            "-C",
            str(staging),
            "fetch",
            "--quiet",
            "--depth",
            "1",
            "origin",
            str(tool["commit"]),
        ],
    )
    run(
        root,
        ["git", "-C", str(staging), "checkout", "--quiet", "--detach", "FETCH_HEAD"],
    )
    destination.parent.mkdir(parents=True, exist_ok=True)
    staging.replace(destination)
    check_git_tool(root, tool)


def bootstrap(root: Path, lock: dict[str, Any], *, check_only: bool) -> None:
    if not check_only:
        ensure_directories(root)
    python_config = lock["python"]
    python = ensure_python_environment(root, python_config, check_only=check_only)

    if not check_only:
        try:
            check_distributions(root, python, python_config["distributions"])
        except BootstrapError:
            install_requirements(
                root,
                python,
                str(python_config["bootstrap_requirements"]),
                "tools/downloads/python/bootstrap",
            )
            install_requirements(
                root,
                python,
                str(python_config["requirements"]),
                "tools/downloads/python/tools",
            )

    check_distributions(root, python, python_config["distributions"])
    check_required_paths(root, python_config["required_paths"])

    for tool in lock["git_tools"]:
        if check_only:
            check_git_tool(root, tool)
        else:
            install_git_tool(root, tool)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Install or verify pinned local decompilation tools."
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="verify the local tool installation without changing it",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        lock = load_lock(root)
        bootstrap(root, lock, check_only=args.check)
    except (
        BootstrapError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print("local tools: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
