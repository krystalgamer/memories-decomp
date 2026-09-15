#!/usr/bin/env python3
from __future__ import annotations

from collections import Counter, defaultdict
import csv
import json
from pathlib import Path
import re
import sys

import candidate_builds


ROOT = Path(__file__).resolve().parents[2]
FUNCTIONS = Path("config/slus_01411/functions.csv")
MATCHING_C = Path("config/slus_01411/matching_c.json")
UNMATCHED_HEADER = Path("src/unmatched.h")
EXCEPTIONS = Path("config/slus_01411/unmatched_contract_exceptions.json")
DATA_EXCEPTIONS = Path(
    "config/slus_01411/unmatched_data_contract_exceptions.json"
)
LINKER_SYMBOLS = Path("config/slus_01411/c_symbols.ld")
CANDIDATES = Path("config/slus_01411/candidates.json")
COMPILER_PROFILES = Path("config/slus_01411/compiler_profiles.json")
# Headers a build-integrated candidate may not take its declaration from:
# the candidate trees themselves, and the overlays resident code cannot see.
HOME_HEADER_EXCLUDED = {"candidates", "candidates_target", "overlays"}
SHARED_OWNER_HEADERS = {
    "func_80042188": "src/game/display_object_packet_submit.h",
    "func_8004CB0C": "src/game/model_slot_setup.h",
}
IDENTIFIER = re.compile(r"\b[A-Za-z_]\w*\b")
FUNCTION_DECLARATION = re.compile(r"\b(?P<name>[A-Za-z_]\w*)\s*\(")
DECLARATION_KEYWORDS = {"__attribute__", "asm"}
LINKER_ASSIGNMENT = re.compile(
    r"^(?P<name>[A-Za-z_]\w*)\s*=\s*0x[0-9A-Fa-f]+\s*;",
    re.MULTILINE,
)


class ContractError(RuntimeError):
    pass


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="surrogateescape")


def inventory(root: Path) -> dict[str, str]:
    path = root / FUNCTIONS
    with path.open(newline="", encoding="utf-8") as handle:
        rows = list(csv.DictReader(handle))
    return {
        row["name"]: row["status"]
        for row in rows
        if row["module"] == "game"
    }


def candidate_names(root: Path) -> set[str]:
    """Inventory names of the build-integrated resident candidates."""
    path = root / CANDIDATES
    if not path.is_file():
        return set()
    data = json.loads(read_text(path))
    addresses = {
        int(item["address"], 16) for item in data.get("candidates", [])
    }
    with (root / FUNCTIONS).open(newline="", encoding="utf-8") as handle:
        return {
            row["name"]
            for row in csv.DictReader(handle)
            if int(row["address"], 16) in addresses
        }


def home_header_declarations(
    root: Path,
    names: set[str],
) -> dict[str, set[str]]:
    """Resident headers other than unmatched.h that declare each name."""
    result: dict[str, set[str]] = defaultdict(set)
    for path in sorted((root / "src").rglob("*.h")):
        relative = path.relative_to(root)
        if relative == UNMATCHED_HEADER:
            continue
        parts = relative.relative_to("src").parts
        if parts and parts[0] in HOME_HEADER_EXCLUDED:
            continue
        for name, _ in declarations(read_text(path)):
            if name in names:
                result[name].add(relative.as_posix())
    return result


def matching_sources(root: Path) -> list[Path]:
    data = json.loads(read_text(root / MATCHING_C))
    entries = data["functions"] if isinstance(data, dict) else data
    return sorted({root / entry["source"] for entry in entries})


def source_profiles(root: Path) -> dict[str, set[str]]:
    result: dict[str, set[str]] = defaultdict(set)
    matching = json.loads(read_text(root / MATCHING_C))
    entries = matching["functions"] if isinstance(matching, dict) else matching
    for entry in entries:
        result[entry["source"]].add(entry.get("profile", "default"))
    path = root / CANDIDATES
    if path.is_file():
        for entry in json.loads(read_text(path)).get("candidates", []):
            source = entry.get("source")
            if source:
                result[source].add(entry.get("profile", "default"))
    return result


def profile_defines(root: Path) -> dict[str, dict[str, str | None]]:
    path = root / COMPILER_PROFILES
    result: dict[str, dict[str, str | None]] = {"default": {}}
    if not path.is_file():
        return result
    profiles = json.loads(read_text(path)).get("profiles", {})
    for name, profile in profiles.items():
        result[name] = {}
        for flag in profile.get("compiler_flags", []):
            if not flag.startswith("-D") or len(flag) <= 2:
                continue
            macro, separator, value = flag[2:].partition("=")
            result[name][macro] = value if separator else "1"
    return result


INCLUDE = re.compile(r'^\s*#\s*include\s+"(?P<path>[^"]+)"')
CONDITIONAL = re.compile(
    r"^\s*#\s*(?P<kind>if|ifdef|ifndef|elif|else|endif)\b"
    r"(?P<argument>.*)$"
)
DEFINE = re.compile(
    r"^\s*#\s*define\s+(?P<name>[A-Za-z_]\w*)"
    r"(?P<parameters>\([^)]*\))?(?P<value>.*)$"
)
UNDEF = re.compile(r"^\s*#\s*undef\s+(?P<name>[A-Za-z_]\w*)")
CONDITION_TOKEN = re.compile(
    r"\s*(?:"
    r"(?P<number>0[xX][0-9A-Fa-f]+|\d+)[uUlL]*|"
    r"(?P<identifier>[A-Za-z_]\w*)|"
    r"(?P<operator>\|\||&&|==|!=|<=|>=|<<|>>|[()!~+\-*/%<>&^|])"
    r")"
)


MacroDefinitions = set[str] | dict[str, str | None]


def _condition_value(
    argument: str,
    defines: MacroDefinitions,
    expanding: frozenset[str] = frozenset(),
) -> int:
    tokens: list[str] = []
    position = 0
    while position < len(argument):
        match = CONDITION_TOKEN.match(argument, position)
        if match is None:
            if argument[position:].strip() == "":
                break
            raise ContractError(
                f"unsupported preprocessor condition: {argument.strip()}"
            )
        tokens.append(
            match.group("number")
            or match.group("identifier")
            or match.group("operator")
        )
        position = match.end()

    class Parser:
        def __init__(self) -> None:
            self.position = 0

        def take(self, token: str) -> bool:
            if self.position < len(tokens) and tokens[self.position] == token:
                self.position += 1
                return True
            return False

        def expression(self) -> int:
            value = self.logical_or()
            if self.position != len(tokens):
                raise ContractError(
                    f"unsupported preprocessor condition: {argument.strip()}"
                )
            return value

        def logical_or(self) -> int:
            value = self.logical_and()
            while self.take("||"):
                right = self.logical_and()
                value = int(bool(value) or bool(right))
            return value

        def logical_and(self) -> int:
            value = self.bitwise_or()
            while self.take("&&"):
                right = self.bitwise_or()
                value = int(bool(value) and bool(right))
            return value

        def bitwise_or(self) -> int:
            value = self.bitwise_xor()
            while self.take("|"):
                value |= self.bitwise_xor()
            return value

        def bitwise_xor(self) -> int:
            value = self.bitwise_and()
            while self.take("^"):
                value ^= self.bitwise_and()
            return value

        def bitwise_and(self) -> int:
            value = self.equality()
            while self.take("&"):
                value &= self.equality()
            return value

        def equality(self) -> int:
            value = self.relational()
            while self.position < len(tokens) and tokens[self.position] in {
                "==", "!=",
            }:
                operator = tokens[self.position]
                self.position += 1
                right = self.relational()
                value = int(value == right) if operator == "==" else int(value != right)
            return value

        def relational(self) -> int:
            value = self.shift()
            while self.position < len(tokens) and tokens[self.position] in {
                "<", "<=", ">", ">=",
            }:
                operator = tokens[self.position]
                self.position += 1
                right = self.shift()
                value = int({
                    "<": value < right,
                    "<=": value <= right,
                    ">": value > right,
                    ">=": value >= right,
                }[operator])
            return value

        def shift(self) -> int:
            value = self.additive()
            while self.position < len(tokens) and tokens[self.position] in {
                "<<", ">>",
            }:
                operator = tokens[self.position]
                self.position += 1
                right = self.additive()
                value = value << right if operator == "<<" else value >> right
            return value

        def additive(self) -> int:
            value = self.multiply()
            while self.position < len(tokens) and tokens[self.position] in {
                "+", "-",
            }:
                operator = tokens[self.position]
                self.position += 1
                right = self.multiply()
                value = value + right if operator == "+" else value - right
            return value

        def multiply(self) -> int:
            value = self.unary()
            while self.position < len(tokens) and tokens[self.position] in {
                "*", "/", "%",
            }:
                operator = tokens[self.position]
                self.position += 1
                right = self.unary()
                if right == 0 and operator in {"/", "%"}:
                    raise ContractError(
                        f"invalid preprocessor condition: {argument.strip()}"
                    )
                if operator == "*":
                    value *= right
                else:
                    quotient = abs(value) // abs(right)
                    if (value < 0) != (right < 0):
                        quotient = -quotient
                    if operator == "/":
                        value = quotient
                    else:
                        value -= quotient * right
            return value

        def unary(self) -> int:
            if self.take("!"):
                return int(not self.unary())
            if self.take("~"):
                return ~self.unary()
            if self.take("+"):
                return self.unary()
            if self.take("-"):
                return -self.unary()
            if self.take("defined"):
                parenthesized = self.take("(")
                if (
                    self.position >= len(tokens)
                    or not IDENTIFIER.fullmatch(tokens[self.position])
                ):
                    raise ContractError(
                        f"invalid defined expression: {argument.strip()}"
                    )
                name = tokens[self.position]
                self.position += 1
                if parenthesized and not self.take(")"):
                    raise ContractError(
                        f"invalid defined expression: {argument.strip()}"
                    )
                return int(name in defines)
            return self.primary()

        def primary(self) -> int:
            if self.take("("):
                value = self.logical_or()
                if not self.take(")"):
                    raise ContractError(
                        f"unbalanced preprocessor condition: {argument.strip()}"
                    )
                return value
            if self.position >= len(tokens):
                raise ContractError(
                    f"incomplete preprocessor condition: {argument.strip()}"
                )
            token = tokens[self.position]
            self.position += 1
            if re.fullmatch(r"0[xX][0-9A-Fa-f]+|\d+", token):
                return int(token, 0)
            if IDENTIFIER.fullmatch(token):
                if token not in defines:
                    return 0
                if isinstance(defines, set):
                    return 1
                value = defines[token]
                if value is None:
                    return 0
                if token in expanding:
                    raise ContractError(
                        f"recursive macro in preprocessor condition: {token}"
                    )
                return _condition_value(
                    value, defines, expanding | frozenset({token})
                )
            raise ContractError(
                f"unsupported preprocessor condition: {argument.strip()}"
            )

    if not tokens:
        return 0
    return Parser().expression()


def condition_enabled(argument: str, defines: MacroDefinitions) -> bool:
    return bool(_condition_value(argument, defines))


def active_header_source(
    root: Path,
    path: Path,
    defines: dict[str, str | None],
    seen: set[Path] | None = None,
) -> str:
    if seen is None:
        seen = set()
    path = path.resolve()
    if path in seen:
        return ""
    seen.add(path)
    active = True
    conditions: list[tuple[bool, bool]] = []
    output: list[str] = []
    for line in read_text(path).splitlines(keepends=True):
        conditional = CONDITIONAL.match(line)
        if conditional is not None:
            kind = conditional.group("kind")
            argument = conditional.group("argument").strip()
            if kind in {"if", "ifdef", "ifndef"}:
                parent = active
                if kind == "ifdef":
                    branch = argument in defines
                elif kind == "ifndef":
                    branch = argument not in defines
                else:
                    branch = condition_enabled(argument, defines)
                conditions.append((parent, branch))
                active = parent and branch
            elif kind in {"elif", "else"} and conditions:
                parent, branch_taken = conditions[-1]
                branch = (
                    not branch_taken
                    if kind == "else"
                    else not branch_taken and condition_enabled(argument, defines)
                )
                conditions[-1] = (parent, branch_taken or branch)
                active = parent and branch
            elif kind == "endif" and conditions:
                parent, _ = conditions.pop()
                active = parent
            continue
        define = DEFINE.match(line)
        if define is not None:
            if active:
                defines[define.group("name")] = (
                    None
                    if define.group("parameters") is not None
                    else define.group("value").strip() or "1"
                )
            continue
        undef = UNDEF.match(line)
        if undef is not None:
            if active:
                defines.pop(undef.group("name"), None)
            continue
        include = INCLUDE.match(line)
        if include is not None and active:
            included = (path.parent / include.group("path")).resolve()
            if (
                included.is_relative_to((root / "src").resolve())
                and included.is_file()
                and included.suffix == ".h"
            ):
                output.append(active_header_source(root, included, defines, seen))
            continue
        if active and path.suffix == ".h":
            output.append(line)
    return "".join(output)


def reachable_data_owners(
    root: Path,
    source: str,
    profile: str,
    linker: set[str],
    defines_by_profile: dict[str, dict[str, str | None]],
) -> set[str]:
    defines = dict(defines_by_profile.get(profile, {}))
    text = active_header_source(root, root / source, defines)
    return {name for name, _ in extern_object_declarations(text, linker)}


def linker_symbols(root: Path) -> set[str]:
    return {
        match.group("name")
        for match in LINKER_ASSIGNMENT.finditer(read_text(root / LINKER_SYMBOLS))
    }


def split_declarators(text: str) -> list[str]:
    parts: list[str] = []
    start = 0
    parentheses = 0
    brackets = 0
    for index, char in enumerate(text):
        if char == "(":
            parentheses += 1
        elif char == ")" and parentheses:
            parentheses -= 1
        elif char == "[":
            brackets += 1
        elif char == "]" and brackets:
            brackets -= 1
        elif char == "," and parentheses == 0 and brackets == 0:
            parts.append(text[start:index].strip())
            start = index + 1
    parts.append(text[start:].strip())
    return [part for part in parts if part]


def remove_parenthesized_annotation(text: str, name: str) -> str:
    pattern = re.compile(rf"\b{re.escape(name)}\s*\(")
    while True:
        match = pattern.search(text)
        if match is None:
            return text
        opening = text.find("(", match.start())
        depth = 0
        end = None
        quote: str | None = None
        escaped = False
        for index in range(opening, len(text)):
            char = text[index]
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
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
                if depth == 0:
                    end = index + 1
                    break
        if end is None:
            return text
        text = text[:match.start()] + text[end:]


def extern_object_symbols(statement: str) -> list[str]:
    if not statement.startswith("extern "):
        return []
    alias = candidate_builds.ASM_ALIAS.search(statement)
    if alias is not None:
        return [alias.group("name")]
    declaration = remove_parenthesized_annotation(
        statement.rstrip(";"),
        "__attribute__",
    )
    result: list[str] = []
    for declarator in split_declarators(declaration):
        pointer = re.search(
            r"\(\s*\*\s*(?P<name>[A-Za-z_]\w*)\s*\)",
            declarator,
        )
        if pointer is not None:
            result.append(pointer.group("name"))
            continue
        ordinary_function = re.search(
            r"\b(?P<name>[A-Za-z_]\w*)\s*\(",
            declarator,
        )
        if ordinary_function is not None:
            continue
        declarator = re.sub(r"(?:\[[^\]]*\]\s*)+$", "", declarator)
        name = re.search(r"(?P<name>[A-Za-z_]\w*)\s*$", declarator)
        if name is not None:
            result.append(name.group("name"))
    return result


def extern_object_declarations(
    text: str,
    known_symbols: set[str] | None = None,
) -> list[tuple[str, str]]:
    result: list[tuple[str, str]] = []
    for statement in candidate_builds.top_level_statements(text):
        symbols = extern_object_symbols(statement)
        if known_symbols is not None:
            symbols = [
                symbol for symbol in symbols if symbol in known_symbols
            ]
        result.extend((symbol, statement) for symbol in sorted(symbols))
    return result


def declaration_name(statement: str) -> str | None:
    if re.search(r"\(\s*\*\s*[A-Za-z_]\w*\s*\)", statement):
        return None
    match = FUNCTION_DECLARATION.search(statement)
    if match is None or match.group("name") in DECLARATION_KEYWORDS:
        return None
    return match.group("name")


def declarations(text: str) -> list[tuple[str, str]]:
    result = []
    for statement in candidate_builds.top_level_statements(text):
        name = declaration_name(statement)
        if name is not None:
            result.append((name, statement))
    return result


def strip_literals_and_directives(text: str) -> str:
    text = candidate_builds.strip_c_comments(text)
    output: list[str] = []
    quote: str | None = None
    escaped = False
    for char in text:
        if quote is not None:
            output.append("\n" if char == "\n" else " ")
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            continue
        if char in ('"', "'"):
            quote = char
            output.append(" ")
            continue
        output.append(char)
    return "\n".join(
        "" if line.lstrip().startswith("#") else line
        for line in "".join(output).splitlines()
    )


def executable_references(
    text: str,
    unmatched: set[str],
    local_declarations: list[tuple[str, str]],
) -> set[str]:
    counts = Counter(
        token
        for token in IDENTIFIER.findall(strip_literals_and_directives(text))
        if token in unmatched
    )
    for name, _ in local_declarations:
        if name in unmatched:
            counts[name] -= 1
    return {name for name, count in counts.items() if count > 0}


def load_exceptions(
    root: Path,
    path: Path = EXCEPTIONS,
) -> list[dict[str, str]]:
    data = json.loads(read_text(root / path))
    if data.get("schema") != 1 or not isinstance(data.get("exceptions"), list):
        raise ContractError(f"{path}: expected schema 1 exception list")
    required = {"symbol", "source", "declaration", "reason"}
    exceptions = data["exceptions"]
    for index, item in enumerate(exceptions):
        if not isinstance(item, dict) or set(item) != required:
            raise ContractError(
                f"{path}: exception {index} must contain "
                f"{sorted(required)}"
            )
        if not all(isinstance(item[key], str) and item[key] for key in required):
            raise ContractError(
                f"{path}: exception {index} values must be non-empty strings"
            )
    keys = [
        (item["source"], item["symbol"], item["declaration"])
        for item in exceptions
    ]
    if len(keys) != len(set(keys)):
        raise ContractError(f"{path}: duplicate exception")
    return exceptions


# Statuses whose declarations may live in the central header. Both kinds lack a
# defining C translation unit, which is the whole reason a shared declaration
# has nowhere else to go; only unmatched_asm is *required* to be centralized,
# while handwritten_asm is admitted a group at a time as it is moved.
CENTRALIZABLE_STATUSES = frozenset({"unmatched_asm", "handwritten_asm"})
CENTRAL_VARIANT_COUNTS = {
    "func_80013C28": 2,
}
CENTRAL_VARIANT_BLOCKS = {
    "func_80013C28": (
        "#ifdef FUNC_80013C28_CALLBACK_VIEW",
        "void func_80013C28(u8, u8 *, u32 *);",
        "#else",
        "void func_80013C28(s32);",
        "#endif",
    ),
}


def canonical_source_lines(source: str) -> list[str]:
    result: list[str] = []
    statement: list[str] = []
    source = re.sub(
        r"\\\r?\n",
        "",
        candidate_builds.strip_c_comments(source),
    )
    for line in source.splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        if stripped.startswith("#"):
            if statement:
                result.append(" ".join(" ".join(statement).split()))
                statement = []
            result.append(" ".join(stripped.split()))
            continue
        statement.append(stripped)
        if ";" in stripped:
            result.append(" ".join(" ".join(statement).split()))
            statement = []
    if statement:
        result.append(" ".join(" ".join(statement).split()))
    return result


DIRECTIVE = re.compile(
    r"^#\s*(?P<kind>if|ifdef|ifndef|elif|else|endif)\b(?P<argument>.*)$"
)


def enclosing_conditionals(
    lines: list[str], stop: int
) -> list[tuple[str, str, str]]:
    conditionals: list[tuple[str, str, str]] = []
    for line in lines[:stop]:
        match = DIRECTIVE.match(line)
        if match is None:
            continue
        kind = match.group("kind")
        argument = match.group("argument").strip()
        if kind in {"if", "ifdef", "ifndef"}:
            conditionals.append((kind, argument, "initial"))
        elif kind == "elif" and conditionals:
            opening_kind, opening_argument, _ = conditionals[-1]
            conditionals[-1] = (opening_kind, opening_argument, "elif")
        elif kind == "else" and conditionals:
            opening_kind, opening_argument, _ = conditionals[-1]
            conditionals[-1] = (opening_kind, opening_argument, "else")
        elif kind == "endif" and conditionals:
            conditionals.pop()
    return conditionals


def validate_variant_block(
    source: str,
    name: str,
    expected: tuple[str, ...],
) -> str | None:
    lines = canonical_source_lines(source)
    starts = [
        index
        for index in range(len(lines) - len(expected) + 1)
        if tuple(lines[index:index + len(expected)]) == expected
    ]
    if len(starts) != 1:
        return (
            f"{UNMATCHED_HEADER}: conditional declaration {name} must use "
            f"the approved selector and ABI arms"
        )
    conditionals = enclosing_conditionals(lines, starts[0])
    if conditionals not in (
        [],
        [("ifndef", "MEMORIES_DECOMP_UNMATCHED_H", "initial")],
    ):
        return (
            f"{UNMATCHED_HEADER}: conditional declaration {name} must not be "
            "inside an extra enclosing preprocessor arm"
        )
    return None


def validate(root: Path = ROOT) -> tuple[list[str], dict[str, int]]:
    statuses = inventory(root)
    unmatched = {
        name for name, status in statuses.items() if status == "unmatched_asm"
    }
    errors: list[str] = []

    central_source = read_text(root / UNMATCHED_HEADER)
    central_pairs = declarations(central_source)
    central = defaultdict(list)
    for name, statement in central_pairs:
        central[name].append(statement)
        status = statuses.get(name)
        if status not in CENTRALIZABLE_STATUSES:
            errors.append(
                f"{UNMATCHED_HEADER}: stale declaration {name}: "
                f"functions.csv status is {status or 'absent'}"
            )
    for name, statements in central.items():
        if len(statements) > 1 and name not in CENTRAL_VARIANT_COUNTS:
            errors.append(
                f"{UNMATCHED_HEADER}: duplicate declarations for {name}: "
                f"{statements}"
            )
        expected_variants = CENTRAL_VARIANT_COUNTS.get(name)
        if expected_variants is not None and len(statements) != expected_variants:
            errors.append(
                f"{UNMATCHED_HEADER}: conditional declaration {name} must "
                f"have exactly {expected_variants} arms: {statements}"
            )
        expected_block = CENTRAL_VARIANT_BLOCKS.get(name)
        if expected_block is not None:
            error = validate_variant_block(central_source, name, expected_block)
            if error is not None:
                errors.append(error)

    # Candidate interfaces may retain their former resident header. A shared
    # ABI dispatcher may also own an unmatched callee when its caller variants
    # are the interface itself rather than unrelated local declarations.
    candidates = candidate_names(root) & unmatched
    shared_owners = set(SHARED_OWNER_HEADERS) & unmatched
    homes = home_header_declarations(root, candidates | shared_owners)
    for name, headers in sorted(homes.items()):
        if name in central:
            errors.append(
                f"{UNMATCHED_HEADER}: {name} is also declared by "
                f"resident headers {sorted(headers)}"
            )
        if name in SHARED_OWNER_HEADERS:
            expected = SHARED_OWNER_HEADERS[name]
            if headers != {expected}:
                errors.append(
                    f"{name} must be declared only by {expected}: "
                    f"{sorted(headers)}"
                )
        elif len(headers) > 1:
            errors.append(
                f"candidate {name} is declared by several resident headers: "
                f"{sorted(headers)}"
            )

    configured = load_exceptions(root)
    if configured:
        errors.append(
            f"{EXCEPTIONS}: local unmatched-function exceptions are no longer "
            "supported; use guarded declarations in src/unmatched.h"
        )
    local_sites: list[tuple[str, str, str]] = []
    referenced_sites: list[tuple[str, str]] = []
    linker = linker_symbols(root)
    profiles_by_source = source_profiles(root)
    defines_by_profile = profile_defines(root)
    central_data_pairs = extern_object_declarations(
        read_text(root / UNMATCHED_HEADER),
    )
    central_data = defaultdict(list)
    for name, statement in central_data_pairs:
        central_data[name].append(statement)
    data_configured = load_exceptions(root, DATA_EXCEPTIONS)
    data_approved = {
        (item["source"], item["symbol"], item["declaration"])
        for item in data_configured
    }
    data_found_approved: set[tuple[str, str, str]] = set()
    local_data = defaultdict(list)

    for source in matching_sources(root):
        relative = source.relative_to(root).as_posix()
        text = read_text(source)
        source_declarations = [
            pair for pair in declarations(text) if pair[0] in unmatched
        ]
        for name, statement in source_declarations:
            key = (relative, name, statement)
            local_sites.append(key)
            errors.append(
                f"{relative}: local declaration of unmatched function "
                f"{name} is forbidden; use a guarded declaration in "
                f"{UNMATCHED_HEADER}: {statement}"
            )
        for name in sorted(
            executable_references(text, unmatched, source_declarations)
        ):
            referenced_sites.append((relative, name))
            if (
                name not in central
                and name not in homes
            ):
                errors.append(
                    f"{relative}: unmatched function {name} is referenced "
                    f"without a declaration in {UNMATCHED_HEADER}"
                )
        for name, statement in extern_object_declarations(text, linker):
            key = (relative, name, statement)
            local_data[name].append((relative, statement))
            if key in data_approved:
                data_found_approved.add(key)
                continue
            if name not in central_data:
                continue
            errors.append(
                f"{relative}: local declaration of central unmatched data "
                f"{name} is not approved: {statement}"
            )

    candidate_data_sites = 0
    for source in sorted((root / "src/candidates").rglob("*.c")):
        relative = source.relative_to(root).as_posix()
        for name, statement in extern_object_declarations(
            read_text(source), linker
        ):
            candidate_data_sites += 1
            local_data[name].append((relative, statement))
            key = (relative, name, statement)
            if key in data_approved:
                data_found_approved.add(key)
                continue
            if name in central_data:
                errors.append(
                    f"{relative}: local declaration of central unmatched data "
                    f"{name} is not approved: {statement}"
                )

    all_data_symbols = set(local_data) | set(central_data)
    data_header_index = candidate_builds.canonical_declaration_index(
        all_data_symbols,
        root / "src",
    )
    for name, statements in central_data.items():
        duplicates = [
            statement
            for statement, count in Counter(statements).items()
            if count > 1
        ]
        if duplicates:
            errors.append(
                f"{UNMATCHED_HEADER}: duplicate data declarations for "
                f"{name}: {duplicates}"
            )
        other_headers = sorted(
            {
                path
                for path, _ in data_header_index.get(name, [])
                if path != UNMATCHED_HEADER.relative_to("src").as_posix()
            }
        )
        if other_headers:
            errors.append(
                f"{UNMATCHED_HEADER}: stale data declaration {name}: "
                f"also declared by resident headers {other_headers}"
            )
        if name not in linker:
            errors.append(
                f"{UNMATCHED_HEADER}: stale data declaration {name}: "
                f"absent from {LINKER_SYMBOLS}"
            )

    for source, name, statement in sorted(
        data_approved - data_found_approved
    ):
        errors.append(
            f"{DATA_EXCEPTIONS}: configured exception not found exactly: "
            f"{source}: {statement}"
        )
    for item in data_configured:
        if item["symbol"] not in linker:
            errors.append(
                f"{DATA_EXCEPTIONS}: stale exception {item['symbol']}: "
                f"absent from {LINKER_SYMBOLS}"
            )
        if not data_header_index.get(item["symbol"]):
            errors.append(
                f"{DATA_EXCEPTIONS}: exception {item['symbol']} has no "
                "declaration in a resident owner header"
            )

    missing_data_owners: dict[str, list[tuple[str, str, str]]] = defaultdict(list)
    for name in sorted(local_data):
        for source, statement in sorted(local_data[name]):
            if (source, name, statement) in data_approved:
                continue
            profiles = profiles_by_source.get(source, {"default"})
            for profile in sorted(profiles):
                if name not in reachable_data_owners(
                    root,
                    source,
                    profile,
                    linker,
                    defines_by_profile,
                ):
                    missing_data_owners[name].append((source, statement, profile))
                    if profile == "default":
                        errors.append(
                            f"{source}: local declaration of unmatched data {name} "
                            f"has no owner header: {statement}"
                        )
                    else:
                        errors.append(
                            f"{source}: local declaration of unmatched data {name} "
                            f"has no reachable owner header under profile {profile}: "
                            f"{statement}"
                        )
    headerless_data = set(missing_data_owners)

    stats = {
        "unmatched": len(unmatched),
        "central": len(central),
        "candidate_homes": len(homes),
        "exception_names": 0,
        "exception_sites": 0,
        "referenced_names": len({name for _, name in referenced_sites}),
        "referenced_sites": len(referenced_sites),
        "local_sites": len(local_sites),
        "central_data": len(central_data),
        "local_data_names": len(local_data),
        "local_data_sites": sum(len(sites) for sites in local_data.values()),
        "candidate_data_sites": candidate_data_sites,
        "headerless_data": len(headerless_data),
        "headerless_data_sites": sum(
            len(missing_data_owners[name]) for name in headerless_data
        ),
        "data_exception_names": len(
            {item["symbol"] for item in data_configured}
        ),
        "data_exception_sites": len(data_configured),
    }
    return sorted(set(errors)), stats


def main() -> int:
    if Path.cwd().resolve() != ROOT:
        print(f"error: run from repository root {ROOT}", file=sys.stderr)
        return 1
    try:
        errors, stats = validate()
    except (ContractError, KeyError, OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1
    print(
        "unmatched contracts: OK "
        f"({stats['unmatched']} unmatched, {stats['central']} central, "
        f"{stats['candidate_homes']} candidate home headers, "
        f"{stats['exception_names']} exception names/"
        f"{stats['exception_sites']} sites, "
        f"{stats['referenced_names']} referenced names; "
        f"{stats['central_data']} central data, "
        f"{stats['headerless_data']} headerless data/"
        f"{stats['headerless_data_sites']} sites, "
        f"{stats['data_exception_names']} data exception names/"
        f"{stats['data_exception_sites']} sites, "
        f"{stats['candidate_data_sites']} candidate sites)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
