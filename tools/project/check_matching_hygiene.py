#!/usr/bin/env python3
"""Reject matching C that only matches through register pins or inline asm.

A matching_c entry stands as a decompilation only if its function is plain C
built by one consistent profile. #3859 moved every function that failed that
to a candidate; this keeps the manifests from regressing. For each entry in
config/slus_01411/matching_c.json and the overlay *_matching_c.json manifests
it rejects:

- a profile whose compiler -G differs from its MASPSX -G;
- a register variable pinned to a hard register (`register T x asm("$N")`);
- an asm statement in the function body, including one reached through a
  project macro or a `static inline` helper that expands to asm;
- a function the source provides only as top-level asm, not as a C
  definition.

A pin or asm statement that turns out to be load-bearing means the function
belongs in src/candidates/ (see notes/candidates/rules.md); one that does
not can simply be deleted.
"""

from __future__ import annotations

import csv
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONFIG = Path("config/slus_01411")
PROFILES = CONFIG / "compiler_profiles.json"

ASM_KEYWORD = r"(?:__asm__|__asm|asm)"
ASM_STATEMENT = re.compile(
    rf"\b{ASM_KEYWORD}\b\s*(?:volatile|__volatile__|__volatile)?\s*\("
)
REGISTER_PIN = re.compile(
    rf"\bregister\b[^;]*?\b{ASM_KEYWORD}\s*\(\s*\"[^\"]*\"\s*\)"
)
DEFINE = re.compile(r"^[ \t]*#[ \t]*define[ \t]+([A-Za-z_]\w*)(.*)$", re.M)
IDENTIFIER = re.compile(r"\b[A-Za-z_]\w*\b")
G_FLAG = re.compile(r"-G(\d+)")


@dataclass(frozen=True)
class Finding:
    manifest: str
    address: str
    name: str
    source: str
    problem: str

    def __str__(self) -> str:
        return (
            f"{self.source}: {self.name} ({self.address}, {self.manifest}): "
            f"{self.problem}"
        )


def mask(text: str, keep_strings: bool = False) -> str:
    """Blank comments (and strings unless kept), preserving offsets."""
    out = list(text)
    index = 0
    length = len(text)
    while index < length:
        char = text[index]
        follow = text[index + 1] if index + 1 < length else ""
        if char == "/" and follow == "*":
            end = text.find("*/", index + 2)
            end = length if end < 0 else end + 2
            for position in range(index, end):
                if out[position] != "\n":
                    out[position] = " "
            index = end
            continue
        if char == "/" and follow == "/":
            end = text.find("\n", index)
            end = length if end < 0 else end
            for position in range(index, end):
                out[position] = " "
            index = end
            continue
        if char in "\"'":
            end = index + 1
            while end < length and text[end] != char:
                end += 2 if text[end] == "\\" else 1
            if not keep_strings:
                for position in range(index + 1, min(end, length)):
                    if out[position] != "\n":
                        out[position] = " "
            index = end + 1
            continue
        index += 1
    return "".join(out)


def blank_directives(text: str) -> str:
    """Blank preprocessor lines (with continuations), preserving offsets."""
    lines = text.split("\n")
    out: list[str] = []
    continuing = False
    for line in lines:
        if continuing or line.lstrip().startswith("#"):
            continuing = line.rstrip().endswith("\\")
            out.append(" " * len(line))
        else:
            out.append(line)
    return "\n".join(out)


def function_bodies(text: str) -> dict[str, tuple[str, str]]:
    """Top-level function definitions: name -> (header, body)."""
    code = blank_directives(mask(text))
    result: dict[str, tuple[str, str]] = {}
    depth = 0
    statement_start = 0
    body_start = 0
    for index, char in enumerate(code):
        if char == "{":
            if depth == 0:
                body_start = index
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                header = code[statement_start:body_start].rstrip()
                if header.endswith(")") and "=" not in header.split("(")[0]:
                    match = re.search(
                        r"([A-Za-z_]\w*)\s*\((?:[^()]|\((?:[^()]|\([^()]*\))*\))*\)\s*$",
                        header,
                    )
                    if match:
                        result[match.group(1)] = (
                            text[statement_start:body_start],
                            text[body_start:index + 1],
                        )
                statement_start = index + 1
        elif char == ";" and depth == 0:
            statement_start = index + 1
    return result


def asm_macros(texts: list[str]) -> set[str]:
    """Macros whose replacement text contains asm, directly or via another."""
    bodies: dict[str, str] = {}
    for text in texts:
        joined = re.sub(r"\\\n", " ", mask(text, keep_strings=True))
        for match in DEFINE.finditer(joined):
            bodies[match.group(1)] = match.group(2)
    tainted = {
        name for name, body in bodies.items() if ASM_STATEMENT.search(body)
    }
    changed = True
    while changed:
        changed = False
        for name, body in bodies.items():
            if name not in tainted and tainted & set(IDENTIFIER.findall(body)):
                tainted.add(name)
                changed = True
    return tainted


def asm_inlines(texts: list[str], macros: set[str]) -> set[str]:
    """static inline functions whose body is, or reaches, asm."""
    bodies: dict[str, str] = {}
    for text in texts:
        for name, (header, body) in function_bodies(text).items():
            if re.search(r"\binline\b|\b__inline__\b|\b__inline\b", header):
                bodies[name] = mask(body, keep_strings=True)
    tainted = {
        name
        for name, body in bodies.items()
        if ASM_STATEMENT.search(body) or macros & set(IDENTIFIER.findall(body))
    }
    changed = True
    while changed:
        changed = False
        for name, body in bodies.items():
            if name not in tainted and tainted & set(IDENTIFIER.findall(body)):
                tainted.add(name)
                changed = True
    return tainted


def g_value(flags: list[str]) -> int | None:
    values = [int(match.group(1)) for flag in flags if (match := G_FLAG.fullmatch(flag))]
    return values[-1] if values else None


def mixed_g(profile: dict[str, object]) -> tuple[int | None, int | None] | None:
    compiler = g_value(list(profile.get("compiler_flags", [])))
    maspsx = g_value(list(profile.get("maspsx_flags", [])))
    return (compiler, maspsx) if compiler != maspsx else None


def body_problems(
    body: str,
    macros: set[str],
    inlines: set[str],
) -> list[str]:
    code = mask(body, keep_strings=True)
    problems: list[str] = []
    pins = REGISTER_PIN.findall(code)
    for pin in pins:
        problems.append("register pin `" + " ".join(pin.split()) + "`")
    remaining = REGISTER_PIN.sub(" ", code)
    statements = len(ASM_STATEMENT.findall(remaining))
    if statements:
        problems.append(
            f"{statements} inline asm statement{'s' if statements != 1 else ''}"
        )
    identifiers = set(IDENTIFIER.findall(mask(remaining)))
    for name in sorted(identifiers & macros):
        problems.append(f"asm through macro `{name}`")
    for name in sorted(identifiers & inlines):
        problems.append(f"asm through inline helper `{name}`")
    return problems


def manifests(root: Path) -> list[tuple[str, Path, Path]]:
    config = root / CONFIG
    result = [("resident", config / "matching_c.json", config / "functions.csv")]
    for path in sorted((config / "overlays").glob("*_matching_c.json")):
        module = path.name[: -len("_matching_c.json")]
        result.append(
            (module, path, config / "overlays" / f"{module}_functions.csv")
        )
    return result


def load_names(path: Path) -> dict[int, str]:
    if not path.is_file():
        return {}
    with path.open(encoding="utf-8", newline="") as handle:
        return {int(row["address"], 16): row["name"] for row in csv.DictReader(handle)}


def validate(root: Path = ROOT) -> tuple[list[Finding], int]:
    profiles = json.loads((root / PROFILES).read_text(encoding="utf-8"))["profiles"]
    headers = [
        path.read_text(encoding="utf-8", errors="surrogateescape")
        for path in sorted((root / "src").rglob("*.h"))
        if "candidates" not in path.relative_to(root / "src").parts
    ]
    header_macros = asm_macros(headers)
    header_inlines = asm_inlines(headers, header_macros)
    findings: list[Finding] = []
    checked = 0
    texts: dict[str, str] = {}
    for manifest, path, inventory in manifests(root):
        names = load_names(root / inventory)
        entries = json.loads(path.read_text(encoding="utf-8"))["functions"]
        for entry in entries:
            checked += 1
            address = int(entry["address"], 16)
            name = names.get(address, f"func_{address:08X}")
            source = entry["source"]
            problems: list[str] = []
            profile = profiles.get(entry["profile"])
            if profile is not None and (g := mixed_g(profile)) is not None:
                problems.append(
                    f"mixed -G profile {entry['profile']} "
                    f"(compiler -G{g[0]}, MASPSX -G{g[1]})"
                )
            if source not in texts:
                texts[source] = (root / source).read_text(
                    encoding="utf-8", errors="surrogateescape"
                )
            text = texts[source]
            macros = header_macros | asm_macros([text])
            inlines = header_inlines | asm_inlines([text], macros)
            definition = function_bodies(text).get(name)
            if definition is None:
                problems.append("no C definition (top-level asm or missing)")
            else:
                header, body = definition
                problems.extend(body_problems(header + body, macros, inlines))
            for problem in problems:
                findings.append(
                    Finding(manifest, f"0x{address:08X}", name, source, problem)
                )
    return findings, checked


def main() -> int:
    if Path.cwd().resolve() != ROOT:
        print(f"error: run from repository root {ROOT}", file=sys.stderr)
        return 1
    findings, checked = validate()
    for finding in findings:
        print(f"error: {finding}", file=sys.stderr)
    if findings:
        print(
            "error: matching C must not depend on register pins, inline asm or "
            "a mixed -G profile; remove the device if it is inert, otherwise "
            "move the function to src/candidates/ (#3859)",
            file=sys.stderr,
        )
        return 1
    print(f"matching hygiene: OK ({checked} functions)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
