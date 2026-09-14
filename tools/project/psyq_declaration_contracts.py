#!/usr/bin/env python3
from __future__ import annotations

import csv
from pathlib import Path
import re
import sys

import candidate_builds
from unmatched_contracts import declaration_name


ROOT = Path(__file__).resolve().parents[2]
FUNCTIONS = Path("config/slus_01411/functions.csv")
PSYQ_ROOT = Path("src/psyq")
CONSUMER_ROOTS = (
    Path("src/game"),
    Path("src/candidates"),
    Path("src/overlays"),
)
LOCAL_INCLUDE = re.compile(r'^\s*#\s*include\s+"(?P<path>[^"]+)"', re.MULTILINE)
DEFINE = re.compile(
    r"^\s*#\s*define\s+(?P<name>[A-Za-z_]\w*)"
    r"(?:\((?P<parameters>[^)]*)\))?\s*(?P<replacement>.*)$"
)
UNDEF = re.compile(r"^\s*#\s*undef\s+(?P<name>[A-Za-z_]\w*)")
CONDITIONAL = re.compile(
    r"^\s*#\s*(?P<directive>if|ifdef|ifndef|elif|else|endif)\b"
    r"(?P<argument>.*)$"
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


def expand_outside_literals(text: str, transform) -> str:
    output: list[str] = []
    start = 0
    index = 0
    while index < len(text):
        char = text[index]
        if char in ('"', "'"):
            output.append(transform(text[start:index]))
            literal_start = index
            quote = char
            index += 1
            while index < len(text):
                char = text[index]
                if char == "\\" and index + 1 < len(text):
                    index += 2
                    continue
                index += 1
                if char == quote:
                    break
            output.append(text[literal_start:index])
            start = index
            continue
        index += 1
    output.append(transform(text[start:]))
    return "".join(output)


def active_source(text: str) -> str:
    text = re.sub(r"\\\r?\n", "", text)
    text = candidate_builds.strip_c_comments(text)
    object_macros: dict[str, str] = {}
    function_macros: dict[str, tuple[list[str], str]] = {}
    active = True
    conditions: list[tuple[bool, bool]] = []
    output: list[str] = []

    for line in text.splitlines(keepends=True):
        conditional = CONDITIONAL.match(line)
        if conditional is not None:
            directive = conditional.group("directive")
            argument = conditional.group("argument").strip()
            if directive in {"if", "ifdef", "ifndef"}:
                parent = active
                if directive == "ifdef":
                    branch = argument in object_macros or argument in function_macros
                elif directive == "ifndef":
                    branch = argument not in object_macros and argument not in function_macros
                else:
                    expression = re.sub(
                        r"defined\s*\(\s*([A-Za-z_]\w*)\s*\)",
                        lambda match: "1"
                        if match.group(1) in object_macros
                        or match.group(1) in function_macros
                        else "0",
                        argument,
                    )
                    expression = object_macros.get(expression, expression)
                    branch = expression.strip() not in {"", "0"}
                conditions.append((parent, branch))
                active = parent and branch
            elif directive in {"elif", "else"} and conditions:
                parent, branch_taken = conditions[-1]
                if directive == "else":
                    branch = not branch_taken
                else:
                    expression = object_macros.get(argument, argument)
                    branch = not branch_taken and expression.strip() not in {"", "0"}
                active = parent and branch
                conditions[-1] = (parent, branch_taken or branch)
            elif directive == "endif" and conditions:
                parent, _ = conditions.pop()
                active = parent
            output.append("\n" if line.endswith("\n") else "")
            continue

        undef = UNDEF.match(line)
        if undef is not None:
            if active:
                name = undef.group("name")
                object_macros.pop(name, None)
                function_macros.pop(name, None)
            output.append("\n" if line.endswith("\n") else "")
            continue

        macro = DEFINE.match(line)
        if macro is not None:
            if active:
                name = macro.group("name")
                replacement = macro.group("replacement")
                parameters = macro.group("parameters")
                if parameters is None:
                    object_macros[name] = replacement
                else:
                    function_macros[name] = (
                        [
                            parameter.strip()
                            for parameter in parameters.split(",")
                            if parameter.strip()
                        ],
                        replacement,
                    )
            output.append("\n" if line.endswith("\n") else "")
            continue

        if not active or line.lstrip().startswith("#"):
            output.append("\n" if line.endswith("\n") else "")
            continue

        expanded = line
        for _ in range(10):
            previous = expanded
            for name, (parameters, replacement) in function_macros.items():
                pattern = re.compile(rf"\b{re.escape(name)}\s*\((?P<args>[^()]*)\)")

                def expand_function(match: re.Match[str]) -> str:
                    arguments = [
                        argument.strip()
                        for argument in match.group("args").split(",")
                    ]
                    if len(arguments) != len(parameters):
                        return match.group(0)
                    result = replacement
                    for parameter, argument in zip(parameters, arguments):
                        result = re.sub(
                            rf"\b{re.escape(parameter)}\b",
                            argument,
                            result,
                        )
                    return result

                expanded = expand_outside_literals(
                    expanded,
                    lambda segment, pattern=pattern: pattern.sub(
                        expand_function, segment
                    ),
                )
            for name, replacement in object_macros.items():
                pattern = re.compile(rf"\b{re.escape(name)}\b")
                expanded = expand_outside_literals(
                    expanded,
                    lambda segment, pattern=pattern, value=replacement: pattern.sub(
                        lambda _match: value, segment
                    ),
                )
            if expanded == previous:
                break
        output.append(expanded)
    return "".join(output)


def local_declaration_statements(text: str) -> list[str]:
    statements: list[str] = []
    start = 0
    quote: str | None = None
    escaped = False
    for index, char in enumerate(text):
        if quote is not None:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            continue
        if char in ('"', "'"):
            quote = char
            continue
        if char in "{}":
            start = index + 1
            continue
        if char != ";":
            continue
        statement = candidate_builds.normalized_statement(text[start : index + 1])
        is_control_statement = statement.startswith(
            ("return ", "if ", "while ", "for ", "switch ")
        )
        if not is_control_statement and re.match(
            r"^(?:extern\s+)?"
            r"(?:(?:static|const|volatile|signed|unsigned|short|long)\s+|"
            r"(?:struct|union|enum)\s+[A-Za-z_]\w*\s+|"
            r"[A-Za-z_]\w*\s+)+"
            r"\**\s*[A-Za-z_]\w*\s*\(",
            statement,
        ):
            statements.append(statement)
        start = index + 1
    return statements


def declarations(text: str) -> list[tuple[str, str]]:
    text = active_source(text)
    statements = candidate_builds.top_level_statements(text)
    statements.extend(local_declaration_statements(text))
    result: list[tuple[str, str]] = []
    seen: set[tuple[str, str]] = set()
    for statement in statements:
        name = declaration_name(statement)
        if name is None:
            continue
        alias = candidate_builds.ASM_ALIAS.search(statement)
        identity = alias.group("name") if alias is not None else name
        record = (identity, statement)
        if record not in seen:
            result.append(record)
            seen.add(record)
    return result


def consumer_files(root: Path) -> list[Path]:
    pending = [
        path
        for relative in CONSUMER_ROOTS
        for path in source_files(root, relative)
    ]
    result: set[Path] = set()
    while pending:
        path = pending.pop()
        if path in result:
            continue
        result.add(path)
        text = re.sub(
            r"\\\r?\n",
            "",
            path.read_text(encoding="utf-8", errors="surrogateescape"),
        )
        for match in LOCAL_INCLUDE.finditer(text):
            included = (path.parent / match.group("path")).resolve()
            if (
                included.is_relative_to((root / "src").resolve())
                and included.is_file()
                and included.suffix == ".h"
            ):
                pending.append(included)
    return sorted(
        path
        for path in result
        if not path.is_relative_to(root / PSYQ_ROOT)
    )


def included_macro_directives(
    root: Path, path: Path, seen: set[Path] | None = None
) -> str:
    if seen is None:
        seen = set()
    path = path.resolve()
    if path in seen:
        return ""
    seen.add(path)
    text = re.sub(
        r"\\\r?\n",
        "",
        path.read_text(encoding="utf-8", errors="surrogateescape"),
    )
    output: list[str] = []
    for line in text.splitlines(keepends=True):
        include = LOCAL_INCLUDE.match(line)
        if include is not None:
            included = (path.parent / include.group("path")).resolve()
            if (
                included.is_relative_to((root / "src").resolve())
                and included.is_file()
                and included.suffix == ".h"
            ):
                output.append(included_macro_directives(root, included, seen))
            continue
        if DEFINE.match(line) or UNDEF.match(line) or CONDITIONAL.match(line):
            output.append(line)
    return "".join(output)


def translation_unit_text(root: Path, path: Path) -> str:
    text = path.read_text(encoding="utf-8", errors="surrogateescape")
    wanted = set(re.findall(r"\b[A-Za-z_]\w*\b", text))
    output: list[str] = []
    start = 0
    for match in LOCAL_INCLUDE.finditer(text):
        output.append(text[start : match.start()])
        included = (path.parent / match.group("path")).resolve()
        if (
            included.is_relative_to((root / "src").resolve())
            and included.is_file()
            and included.suffix == ".h"
        ):
            directives = included_macro_directives(root, included)
            entries: list[tuple[str, str, str | None]] = []
            for line in directives.splitlines(keepends=True):
                define = DEFINE.match(line)
                undef = UNDEF.match(line)
                if define is not None:
                    entries.append(
                        (define.group("name"), line, define.group("replacement"))
                    )
                elif undef is not None:
                    entries.append((undef.group("name"), line, None))
            replacements: dict[str, list[str]] = {}
            for name, _, replacement in entries:
                if replacement is not None:
                    replacements.setdefault(name, []).append(replacement)
            pending = list(wanted)
            needed: set[str] = set()
            while pending:
                name = pending.pop()
                if name in needed or name not in replacements:
                    continue
                needed.add(name)
                for replacement in replacements[name]:
                    pending.extend(re.findall(r"\b[A-Za-z_]\w*\b", replacement))
            output.extend(line for name, line, _ in entries if name in needed)
        start = match.end()
    output.append(text[start:])
    return "".join(output)


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

    for path in consumer_files(root):
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

    reported = {(error.split(":", 1)[0], error) for error in errors}
    for relative in CONSUMER_ROOTS:
        for path in source_files(root, relative):
            if path.suffix != ".c":
                continue
            for name, statement in declarations(translation_unit_text(root, path)):
                if name not in sdk:
                    continue
                error = (
                    f"{path.relative_to(root)}: Psy-Q function {name} must be "
                    f"declared by a header under {PSYQ_ROOT}: {statement}"
                )
                key = (str(path.relative_to(root)), error)
                if key in reported:
                    continue
                foreign_sites += 1
                errors.append(error)
                reported.add(key)

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
