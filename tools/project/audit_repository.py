#!/usr/bin/env python3

from __future__ import annotations

import csv
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path
from pathlib import PurePosixPath

from workspace import WorkspaceError, require_workspace_root


class AuditError(RuntimeError):
    pass


EXPECTED_NAME = "Copilot"
EXPECTED_EMAIL = "223556219+Copilot@users.noreply.github.com"
FORBIDDEN_TRACKED_PREFIXES = (
    "game/",
    "tmp/",
    "tools/downloads/",
    "tools/environments/",
    "tools/toolchains/",
    "tools/vendor/",
)
ALLOWED_MARKDOWN_PATHS = {
    ".github/copilot-instructions.md",
}
ALLOWED_MARKDOWN_NAME = "README.md"
ATTEMPT_FIELDS = ("address", "attempt", "compiler", "flags", "result", "summary")
ATTEMPT_RESULTS = {"matched", "nonmatch", "deferred"}
MAX_FUNCTION_ATTEMPTS = 6
EXTERNAL_ATTEMPT_FIELDS = (
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
EXTERNAL_MODES = {
    "reference_match",
    "inline_refinement",
    "collaborator_match",
    "post_terminal_resolution",
}
EXTERNAL_MODE_LIMITS = {
    "reference_match": MAX_FUNCTION_ATTEMPTS,
    "inline_refinement": MAX_FUNCTION_ATTEMPTS,
    "collaborator_match": 1,
    "post_terminal_resolution": 1,
}
SHA256_PATTERN = re.compile(r"^[0-9a-f]{64}$")
ASM_PATTERN = re.compile(r"\b(?:asm|__asm|__asm__)\b")
COMMENT_PATTERN = re.compile(r"/\*.*?\*/|//[^\n]*", re.DOTALL)


def git(root: Path, *arguments: str) -> str:
    try:
        result = subprocess.run(
            ["git", *arguments],
            cwd=root,
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
    except subprocess.CalledProcessError as error:
        details = error.stderr.strip()
        message = f"git {' '.join(arguments)} failed"
        if details:
            message = f"{message}: {details}"
        raise AuditError(message) from error
    return result.stdout


def is_copilot_attributed(name: str, email: str) -> bool:
    return (
        name == EXPECTED_NAME
        or email.casefold() == EXPECTED_EMAIL.casefold()
    )


def audit_identity(root: Path) -> None:
    name = git(root, "config", "--local", "user.name").strip()
    email = git(root, "config", "--local", "user.email").strip()
    if (name, email) != (EXPECTED_NAME, EXPECTED_EMAIL):
        raise AuditError(
            f"local Git identity is {name} <{email}>, expected "
            f"{EXPECTED_NAME} <{EXPECTED_EMAIL}>"
        )

    commits = [
        line for line in git(root, "rev-list", "--reverse", "HEAD").splitlines()
        if line
    ]
    if not commits:
        raise AuditError("repository has no commits")
    first_parent = {
        line
        for line in git(root, "rev-list", "--first-parent", "HEAD").splitlines()
        if line
    }

    for commit in commits:
        output = git(
            root,
            "show",
            "-s",
            "--format=%P%n%an%n%ae%n%cn%n%ce%n%B",
            commit,
        )
        lines = output.splitlines()
        if len(lines) < 5:
            raise AuditError(f"{commit}: malformed commit metadata")
        if commit not in first_parent:
            continue
        parents = lines[0].split()
        author_name, author_email, committer_name, committer_email = lines[1:5]
        message = "\n".join(lines[5:])
        if (
            len(parents) >= 2
            and (committer_name, committer_email)
            == ("GitHub", "noreply@github.com")
            and re.match(r"^Merge pull request #\d+ from ", message)
        ):
            continue
        author_is_copilot = is_copilot_attributed(author_name, author_email)
        committer_is_copilot = is_copilot_attributed(
            committer_name, committer_email
        )
        if author_is_copilot and (author_name, author_email) != (
            EXPECTED_NAME,
            EXPECTED_EMAIL,
        ):
            raise AuditError(
                f"{commit}: Copilot-attributed author is "
                f"{author_name} <{author_email}>, expected "
                f"{EXPECTED_NAME} <{EXPECTED_EMAIL}>"
            )
        if committer_is_copilot and (committer_name, committer_email) != (
            EXPECTED_NAME,
            EXPECTED_EMAIL,
        ):
            raise AuditError(
                f"{commit}: Copilot-attributed committer is "
                f"{committer_name} <{committer_email}>, expected "
                f"{EXPECTED_NAME} <{EXPECTED_EMAIL}>"
            )
        if not (author_is_copilot or committer_is_copilot):
            continue
        trailers = re.findall(
            r"^Co-authored-by:\s*(.+?)\s*$",
            message,
            flags=re.IGNORECASE | re.MULTILINE,
        )
        expected_trailer = f"{EXPECTED_NAME} <{EXPECTED_EMAIL}>"
        unexpected = [
            trailer for trailer in trailers if trailer != expected_trailer
        ]
        if unexpected:
            raise AuditError(
                f"{commit}: unexpected Co-authored-by trailer "
                f"{unexpected[0]}"
            )


def audit_tracked_paths(root: Path) -> None:
    tracked = [
        line for line in git(root, "ls-files").splitlines()
        if line
    ]
    for path in tracked:
        if path.startswith(FORBIDDEN_TRACKED_PREFIXES):
            raise AuditError(f"forbidden generated or supplied path tracked: {path}")
        if (
            path.lower().endswith(".md")
            and not path.startswith("notes/")
            and PurePosixPath(path).name != ALLOWED_MARKDOWN_NAME
            and path not in ALLOWED_MARKDOWN_PATHS
        ):
            raise AuditError(
                f"documentation is outside notes/, a directory "
                f"{ALLOWED_MARKDOWN_NAME}, or the allowed project "
                f"Markdown paths: {path}"
            )

    ignored = git(
        root,
        "check-ignore",
        "game/SLUS_014.11",
        "tmp/project-build/SLUS_014.11",
        "tools/environments/python/bin/python",
    ).splitlines()
    if len(ignored) != 3:
        raise AuditError("required supplied/generated paths are not all ignored")


def audit_worktree(root: Path) -> None:
    status = git(root, "status", "--porcelain").strip()
    if status:
        raise AuditError(f"worktree is not clean:\n{status}")


def parse_integer(value: str, description: str) -> int:
    try:
        return int(value, 0)
    except ValueError as error:
        raise AuditError(f"{description} is not an integer: {value}") from error


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def require_tmp_path(value: str, description: str) -> None:
    path = Path(value)
    if (
        path.is_absolute()
        or not path.parts
        or path.parts[0] != "tmp"
        or ".." in path.parts
    ):
        raise AuditError(f"{description} must normalize beneath tmp/")


def audit_attempts(root: Path) -> None:
    functions_path = root / "config/slus_01411/functions.csv"
    attempts_path = root / "config/slus_01411/attempts.csv"
    external_path = root / "config/slus_01411/external_attempts.csv"
    profiles_path = root / "config/slus_01411/compiler_profiles.json"

    with functions_path.open("r", encoding="utf-8", newline="") as handle:
        functions_reader = csv.DictReader(handle)
        functions = list(functions_reader)
    function_addresses = {
        parse_integer(row["address"], "function address") for row in functions
    }
    function_names = {
        parse_integer(row["address"], "function address"): row["name"]
        for row in functions
    }
    matching_path = root / "config/slus_01411/matching_c.json"
    with matching_path.open("r", encoding="utf-8") as handle:
        matching_value = json.load(handle)
    matching_rows = matching_value.get("functions")
    if not isinstance(matching_rows, list):
        raise AuditError(f"{matching_path}: missing functions list")
    matching_entries = {
        parse_integer(str(row["address"]), "matching manifest address"): row
        for row in matching_rows
        if isinstance(row, dict)
    }
    matching_addresses = {
        parse_integer(row["address"], "matching function address")
        for row in functions
        if row["status"] == "matching_c"
    }
    function_modules = {
        parse_integer(row["address"], "function address"): row["module"]
        for row in functions
    }
    with profiles_path.open("r", encoding="utf-8") as handle:
        profiles_value = json.load(handle)
    profiles = profiles_value.get("profiles")
    if not isinstance(profiles, dict):
        raise AuditError(f"{profiles_path}: missing profiles object")

    with attempts_path.open("r", encoding="utf-8", newline="") as handle:
        attempts_reader = csv.DictReader(handle)
        if tuple(attempts_reader.fieldnames or ()) != ATTEMPT_FIELDS:
            raise AuditError(f"{attempts_path}: unexpected CSV fields")
        attempts = list(attempts_reader)

    by_address: dict[int, list[dict[str, str]]] = {}
    for row in attempts:
        address = parse_integer(row["address"], "attempt address")
        if address not in function_addresses:
            raise AuditError(
                f"attempt references unknown function {address:#010x}"
            )
        if row["result"] not in ATTEMPT_RESULTS:
            raise AuditError(
                f"{address:#010x}: unsupported attempt result {row['result']}"
            )
        if not row["compiler"] or not row["flags"] or not row["summary"]:
            raise AuditError(f"{address:#010x}: incomplete attempt record")
        if function_modules[address] != "game":
            raise AuditError(
                f"{address:#010x}: canonical attempts are game-only"
            )
        by_address.setdefault(address, []).append(row)

    matched_attempts: set[int] = set()
    for address, rows in by_address.items():
        if len(rows) > MAX_FUNCTION_ATTEMPTS:
            raise AuditError(
                f"{address:#010x}: exceeds six-attempt budget"
            )
        ended = False
        for expected, row in enumerate(rows, start=1):
            attempt = parse_integer(row["attempt"], "attempt number")
            if attempt != expected:
                raise AuditError(
                    f"{address:#010x}: expected attempt {expected}, "
                    f"found {attempt}"
                )
            if ended:
                raise AuditError(
                    f"{address:#010x}: attempt recorded after terminal result"
                )
            if row["result"] == "matched":
                matched_attempts.add(address)
                ended = True
            elif row["result"] == "deferred":
                ended = True

    with external_path.open("r", encoding="utf-8", newline="") as handle:
        external_reader = csv.DictReader(handle)
        if tuple(external_reader.fieldnames or ()) != EXTERNAL_ATTEMPT_FIELDS:
            raise AuditError(f"{external_path}: unexpected CSV fields")
        external_attempts = list(external_reader)

    external_by_key: dict[tuple[str, int], list[dict[str, str]]] = {}
    external_matches: set[int] = set()
    inline_latest: dict[int, str] = {}
    for row in external_attempts:
        if row["mode"] == "inline_refinement":
            inline_latest[
                parse_integer(row["address"], "external attempt address")
            ] = row["result"]
    resolution_addresses = {
        address
        for address, rows in by_address.items()
        if rows[-1]["result"] == "deferred"
    } | {
        address
        for address, result in inline_latest.items()
        if result == "deferred"
    }
    for row in external_attempts:
        mode = row["mode"]
        address = parse_integer(row["address"], "external attempt address")
        if mode not in EXTERNAL_MODES:
            raise AuditError(
                f"{address:#010x}: unsupported external mode {mode}"
            )
        if address not in function_addresses:
            raise AuditError(
                f"external attempt references unknown function {address:#010x}"
            )
        if function_modules[address] != "game":
            raise AuditError(
                f"{address:#010x}: external attempts are game-only"
            )
        if row["profile"] not in profiles:
            raise AuditError(
                f"{address:#010x}: unknown external profile {row['profile']}"
            )
        if row["result"] not in ATTEMPT_RESULTS:
            raise AuditError(
                f"{address:#010x}: unsupported external result {row['result']}"
            )
        require_tmp_path(
            row["candidate_source"],
            f"{address:#010x}: external candidate",
        )
        if not SHA256_PATTERN.fullmatch(row["candidate_sha256"]):
            raise AuditError(
                f"{address:#010x}: invalid candidate SHA-256"
            )
        if not row["summary"]:
            raise AuditError(f"{address:#010x}: empty external summary")
        has_reference = bool(row["reference_path"] or row["reference_sha256"])
        if bool(row["reference_path"]) != bool(row["reference_sha256"]):
            raise AuditError(
                f"{address:#010x}: incomplete reference path/hash pair"
            )
        if mode == "reference_match" and not has_reference:
            raise AuditError(
                f"{address:#010x}: reference_match lacks reference source"
            )
        if has_reference:
            reference = PurePosixPath(row["reference_path"])
            original = (
                "tmp/references/ygofm-decomp/src/"
                f"func_{address:08X}.c"
            )
            collaborator = (
                not reference.is_absolute()
                and ".." not in reference.parts
                and reference.parts[:4]
                == ("tmp", "references", "ygofm-decomp-unchiga", "src")
                and reference.suffix == ".c"
            )
            valid_reference = (
                row["reference_path"] == original
                if mode == "reference_match"
                else collaborator
                if mode == "collaborator_match"
                else row["reference_path"] == original or collaborator
            )
            if not valid_reference:
                raise AuditError(
                    f"{address:#010x}: unexpected reference path"
                )
            if not SHA256_PATTERN.fullmatch(row["reference_sha256"]):
                raise AuditError(
                    f"{address:#010x}: invalid reference SHA-256"
                )
        if mode == "inline_refinement" and address not in matching_addresses:
            raise AuditError(
                f"{address:#010x}: inline refinement is not matching C"
            )
        if mode == "post_terminal_resolution":
            if address not in resolution_addresses:
                raise AuditError(
                    f"{address:#010x}: post-terminal resolution lacks a "
                    "deferred canonical or inline-refinement history"
                )
            if row["result"] != "matched":
                raise AuditError(
                    f"{address:#010x}: post-terminal resolution must be matched"
                )
        external_by_key.setdefault((mode, address), []).append(row)

    for (mode, address), rows in external_by_key.items():
        maximum = EXTERNAL_MODE_LIMITS[mode]
        if len(rows) > maximum:
            raise AuditError(
                f"{address:#010x}: exceeds {maximum} {mode} attempts"
            )
        ended = False
        for expected, row in enumerate(rows, start=1):
            attempt = parse_integer(row["attempt"], "external attempt number")
            if attempt != expected:
                raise AuditError(
                    f"{address:#010x}: expected external attempt {expected}, "
                    f"found {attempt}"
                )
            if ended:
                raise AuditError(
                    f"{address:#010x}: external row follows terminal result"
                )
            if row["result"] == "matched":
                external_matches.add(address)
                ended = True
            elif row["result"] == "deferred":
                ended = True
            elif expected == maximum:
                raise AuditError(
                    f"{address:#010x}: final external attempt is not deferred"
                )

    latest_success_by_address: dict[int, dict[str, str]] = {}
    for row in external_attempts:
        if row["result"] == "matched":
            address = parse_integer(
                row["address"], "external matched address"
            )
            latest_success_by_address[address] = row

    for address, row in latest_success_by_address.items():
        mode = row["mode"]
        if address not in matching_addresses:
            raise AuditError(
                f"{address:#010x}: matched {mode} evidence is not integrated"
            )
        entry = matching_entries.get(address)
        if not isinstance(entry, dict):
            raise AuditError(
                f"{address:#010x}: missing matching manifest entry"
            )
        if entry.get("profile") != row["profile"]:
            raise AuditError(
                f"{address:#010x}: matched external profile differs from manifest"
            )
        source_value = entry.get("source")
        if not isinstance(source_value, str):
            raise AuditError(
                f"{address:#010x}: matching manifest source is invalid"
            )
        source = root / source_value
        if not source.is_file():
            raise AuditError(
                f"{address:#010x}: matching source does not exist"
            )
        source_text = source.read_text(encoding="utf-8")
        definition = re.compile(
            rf"\b{re.escape(function_names[address])}\s*\("
        )
        if definition.search(source_text) is None:
            raise AuditError(
                f"{address:#010x}: current source does not define "
                f"{function_names[address]}"
            )
        if ASM_PATTERN.search(COMMENT_PATTERN.sub("", source_text)):
            raise AuditError(
                f"{address:#010x}: successful external source still uses GCC asm"
            )

    missing = sorted(
        matching_addresses - matched_attempts - external_matches
    )
    if missing:
        formatted = ", ".join(f"{address:#010x}" for address in missing)
        raise AuditError(f"matching C functions lack successful attempts: {formatted}")


def main() -> int:
    try:
        root = require_workspace_root()
        audit_identity(root)
        audit_tracked_paths(root)
        audit_attempts(root)
        audit_worktree(root)
    except (
        AuditError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        json.JSONDecodeError,
        csv.Error,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print("repository audit: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
