#!/usr/bin/env python3

from __future__ import annotations

import argparse
import ast
import csv
import io
import json
import re
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

from workspace import WorkspaceError, require_workspace_root, resolve_within


class GlobalUsageError(RuntimeError):
    pass


@dataclass(frozen=True)
class Function:
    address: int
    size: int
    name: str
    status: str
    module: str


@dataclass(frozen=True)
class Token:
    value: str
    start: int
    end: int
    line: int


@dataclass(frozen=True)
class CFunction:
    name: str
    tokens: tuple[Token, ...]


@dataclass
class Usage:
    global_address: int
    function: Function
    evidence_source: str
    evidence_path: str
    accesses: set[str] = field(default_factory=set)
    observed_names: set[str] = field(default_factory=set)
    contexts: set[str] = field(default_factory=set)
    widths: set[str] = field(default_factory=set)


ASSIGNMENT_OPERATORS = {
    "=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    "<<=",
    ">>=",
    "&=",
    "^=",
    "|=",
}
COMPOUND_ASSIGNMENT_OPERATORS = ASSIGNMENT_OPERATORS - {"="}
GENERATED_NAME_RE = re.compile(r"D_([0-9A-Fa-f]{8})(?:_[A-Za-z0-9_]+)?$")
CODEGEN_ALIAS_RE = re.compile(r"Base\d+_[0-9A-Fa-f]{8}$")
IDENTIFIER_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
SYMBOL_ASSIGNMENT_RE = re.compile(
    r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(0x[0-9A-Fa-f]+)\s*;"
)
TOKEN_RE = re.compile(
    r"[A-Za-z_][A-Za-z0-9_]*"
    r"|0[xX][0-9A-Fa-f]+"
    r"|(?:<<=|>>=|\+\+|--|->|==|!=|<=|>=|&&|\|\||"
    r"\+=|-=|\*=|/=|%=|&=|\^=|\|=|<<|>>)"
    r"|[{}()\[\];,.:?~!%^&*+\-/|<>=]"
)
RELOCATION_RE = re.compile(
    r"%(hi|lo|gp_rel)\(\s*([A-Za-z_][A-Za-z0-9_]*)"
    r"(?:\s*[+-]\s*(?:0[xX][0-9A-Fa-f]+|\d+))?\s*\)"
)
FUNCTION_LABEL_RE = re.compile(r"^\s*glabel\s+([A-Za-z_][A-Za-z0-9_]*)\s*$")

TYPE_WIDTHS = {
    "s8": "8",
    "u8": "8",
    "char": "8",
    "s16": "16",
    "u16": "16",
    "short": "16",
    "s32": "32",
    "u32": "32",
    "int": "32",
    "long": "32",
    "float": "32",
    "s64": "64",
    "u64": "64",
    "double": "64",
}
LOAD_WIDTHS = {
    "lb": "8",
    "lbu": "8",
    "lh": "16",
    "lhu": "16",
    "lw": "32",
    "lwl": "32",
    "lwr": "32",
    "lwc1": "32",
    "lwc2": "32",
    "ld": "64",
    "ldc1": "64",
    "ldc2": "64",
}
STORE_WIDTHS = {
    "sb": "8",
    "sh": "16",
    "sw": "32",
    "swl": "32",
    "swr": "32",
    "swc1": "32",
    "swc2": "32",
    "sd": "64",
    "sdc1": "64",
    "sdc2": "64",
}
MIPS_OPCODE_ACCESS = {
    0x20: ("read", "8"),
    0x21: ("read", "16"),
    0x22: ("read", "32"),
    0x23: ("read", "32"),
    0x24: ("read", "8"),
    0x25: ("read", "16"),
    0x26: ("read", "32"),
    0x28: ("write", "8"),
    0x29: ("write", "16"),
    0x2A: ("write", "32"),
    0x2B: ("write", "32"),
    0x2E: ("write", "32"),
    0x31: ("read", "32"),
    0x32: ("read", "32"),
    0x35: ("read", "64"),
    0x36: ("read", "64"),
    0x39: ("write", "32"),
    0x3A: ("write", "32"),
    0x3D: ("write", "64"),
    0x3E: ("write", "64"),
}


def load_inventory(path: Path) -> list[Function]:
    functions: list[Function] = []
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        expected = {"address", "size", "name", "status", "module"}
        if reader.fieldnames is None or not expected.issubset(reader.fieldnames):
            raise GlobalUsageError(f"{path} has unexpected columns")
        for row in reader:
            functions.append(
                Function(
                    address=int(row["address"], 0),
                    size=int(row["size"], 0),
                    name=row["name"],
                    status=row["status"],
                    module=row["module"],
                )
            )
    if len({function.address for function in functions}) != len(functions):
        raise GlobalUsageError(f"{path} contains duplicate function addresses")
    if len({function.name for function in functions}) != len(functions):
        raise GlobalUsageError(f"{path} contains duplicate function names")
    return functions


def load_matching_sources(path: Path) -> dict[int, str]:
    with path.open("r", encoding="utf-8") as handle:
        value = json.load(handle)
    entries = value.get("functions")
    if not isinstance(entries, list):
        raise GlobalUsageError(f"{path} has no functions list")
    result: dict[int, str] = {}
    for entry in entries:
        address = int(entry["address"], 0)
        source = entry["source"]
        if address in result:
            raise GlobalUsageError(
                f"{path} contains duplicate matching function {address:#010x}"
            )
        result[address] = source
    return result


def load_symbols(
    paths: list[Path],
) -> tuple[dict[str, int], dict[int, set[str]], set[str]]:
    by_name: dict[str, int] = {}
    by_address: dict[int, set[str]] = defaultdict(set)
    function_names: set[str] = set()
    for path in paths:
        with path.open("r", encoding="utf-8") as handle:
            for line_number, line in enumerate(handle, 1):
                match = SYMBOL_ASSIGNMENT_RE.match(line)
                if match is None:
                    continue
                name, address_text = match.groups()
                address = int(address_text, 0)
                previous = by_name.get(name)
                if previous is not None and previous != address:
                    raise GlobalUsageError(
                        f"{path}:{line_number}: {name} has conflicting addresses"
                    )
                by_name[name] = address
                by_address[address].add(name)
                if (
                    "type:func" in line
                    or name.startswith("func_")
                    or name == "entrypoint"
                ):
                    function_names.add(name)
    return by_name, by_address, function_names


def blank_non_code(text: str) -> str:
    output = list(text)
    index = 0
    state = "normal"
    line_start = True
    preprocessor = False
    escaped = False
    while index < len(text):
        character = text[index]
        following = text[index + 1] if index + 1 < len(text) else ""
        if preprocessor:
            if character == "\n":
                preprocessor = index > 0 and text[index - 1] == "\\"
                line_start = True
            else:
                output[index] = " "
            index += 1
            continue
        if state == "normal":
            if line_start and character in " \t\r":
                index += 1
                continue
            if line_start and character == "#":
                output[index] = " "
                preprocessor = True
                line_start = False
                index += 1
                continue
            line_start = character == "\n"
            if character == "/" and following == "/":
                output[index] = output[index + 1] = " "
                state = "line_comment"
                index += 2
                continue
            if character == "/" and following == "*":
                output[index] = output[index + 1] = " "
                state = "block_comment"
                index += 2
                continue
            if character == '"':
                output[index] = " "
                state = "string"
                escaped = False
            elif character == "'":
                output[index] = " "
                state = "character"
                escaped = False
            index += 1
            continue
        if state == "line_comment":
            if character == "\n":
                state = "normal"
                line_start = True
            else:
                output[index] = " "
            index += 1
            continue
        if state == "block_comment":
            if character == "*" and following == "/":
                output[index] = output[index + 1] = " "
                state = "normal"
                index += 2
            else:
                if character != "\n":
                    output[index] = " "
                index += 1
            continue
        if state in {"string", "character"}:
            if character != "\n":
                output[index] = " "
            if escaped:
                escaped = False
            elif character == "\\":
                escaped = True
            elif (state == "string" and character == '"') or (
                state == "character" and character == "'"
            ):
                state = "normal"
            if character == "\n":
                line_start = True
            index += 1
    return "".join(output)


def tokenize(text: str) -> list[Token]:
    tokens: list[Token] = []
    line = 1
    position = 0
    for match in TOKEN_RE.finditer(text):
        line += text.count("\n", position, match.start())
        tokens.append(Token(match.group(0), match.start(), match.end(), line))
        position = match.start()
    return tokens


def matching_open_paren(tokens: list[Token], close_index: int) -> int | None:
    depth = 0
    for index in range(close_index, -1, -1):
        value = tokens[index].value
        if value == ")":
            depth += 1
        elif value == "(":
            depth -= 1
            if depth == 0:
                return index
    return None


def parse_c_functions(text: str) -> tuple[list[CFunction], list[Token]]:
    cleaned = blank_non_code(text)
    tokens = tokenize(cleaned)
    functions: list[CFunction] = []
    top_level_tokens: list[Token] = []
    index = 0
    while index < len(tokens):
        token = tokens[index]
        if token.value != "{":
            top_level_tokens.append(token)
            index += 1
            continue
        close_paren = index - 1
        if close_paren >= 0 and tokens[close_paren].value == ")":
            open_paren = matching_open_paren(tokens, close_paren)
            name_index = None if open_paren is None else open_paren - 1
            if (
                name_index is not None
                and name_index >= 0
                and IDENTIFIER_RE.fullmatch(tokens[name_index].value)
                and tokens[name_index].value
                not in {"if", "for", "while", "switch", "sizeof"}
            ):
                depth = 1
                end = index + 1
                while end < len(tokens) and depth:
                    if tokens[end].value == "{":
                        depth += 1
                    elif tokens[end].value == "}":
                        depth -= 1
                    end += 1
                if depth:
                    raise GlobalUsageError(
                        f"unbalanced function body for {tokens[name_index].value}"
                    )
                while (
                    top_level_tokens
                    and top_level_tokens[-1].value not in {";", "}"}
                ):
                    top_level_tokens.pop()
                functions.append(
                    CFunction(
                        name=tokens[name_index].value,
                        tokens=tuple(tokens[index + 1 : end - 1]),
                    )
                )
                index = end
                continue
        depth = 1
        top_level_tokens.append(token)
        index += 1
        while index < len(tokens) and depth:
            top_level_tokens.append(tokens[index])
            if tokens[index].value == "{":
                depth += 1
            elif tokens[index].value == "}":
                depth -= 1
            index += 1
        if depth:
            raise GlobalUsageError("unbalanced top-level braces in C source")
    return functions, top_level_tokens


def infer_declarations(
    tokens: list[Token], known_names: set[str]
) -> tuple[dict[str, str], set[str]]:
    widths: dict[str, str] = {}
    arrays: set[str] = set()
    statement: list[Token] = []
    for token in tokens:
        statement.append(token)
        if token.value != ";":
            continue
        width = next(
            (TYPE_WIDTHS[item.value] for item in statement if item.value in TYPE_WIDTHS),
            "",
        )
        values = [item.value for item in statement]
        for index, value in enumerate(values):
            if value not in known_names:
                continue
            if width:
                widths[value] = width
            if index + 1 < len(values) and values[index + 1] == "[":
                arrays.add(value)
        statement = []
    return widths, arrays


def resolve_global(
    name: str,
    symbols_by_name: dict[str, int],
    function_addresses: set[int],
    function_names: set[str],
) -> int | None:
    if name in function_names:
        return None
    address = symbols_by_name.get(name)
    if address is None:
        match = GENERATED_NAME_RE.fullmatch(name)
        if match is None:
            return None
        address = int(match.group(1), 16)
    if address in function_addresses:
        return None
    return address


def skip_balanced_postfix(tokens: tuple[Token, ...], index: int) -> int:
    current = index + 1
    while current < len(tokens):
        if tokens[current].value == "[":
            depth = 1
            current += 1
            while current < len(tokens) and depth:
                if tokens[current].value == "[":
                    depth += 1
                elif tokens[current].value == "]":
                    depth -= 1
                current += 1
            continue
        if (
            tokens[current].value in {".", "->"}
            and current + 1 < len(tokens)
            and IDENTIFIER_RE.fullmatch(tokens[current + 1].value)
        ):
            current += 2
            continue
        break
    return current


def classify_c_access(
    tokens: tuple[Token, ...], index: int, arrays: set[str]
) -> str:
    previous = tokens[index - 1].value if index else ""
    following = tokens[index + 1].value if index + 1 < len(tokens) else ""
    if previous == "&":
        return "address"
    if previous in {"++", "--"} or following in {"++", "--"}:
        return "read_write"
    if previous == "(" and index >= 2 and tokens[index - 2].value == "sizeof":
        return "unknown"
    postfix_end = skip_balanced_postfix(tokens, index)
    if postfix_end < len(tokens):
        operator = tokens[postfix_end].value
        if operator == "=":
            return "write"
        if operator in COMPOUND_ASSIGNMENT_OPERATORS:
            return "read_write"
        if operator in {"++", "--"}:
            return "read_write"
    if tokens[index].value in arrays and following != "[":
        return "address"
    return "read"


def source_context(lines: list[str], line_number: int) -> str:
    if not 1 <= line_number <= len(lines):
        return ""
    context = " ".join(lines[line_number - 1].strip().split())
    if len(context) > 120:
        return context[:117] + "..."
    return context


def inline_assembly_text(text: str) -> str | None:
    if "__asm__" not in text:
        return None
    pieces: list[str] = []
    for line in text.splitlines():
        stripped = line.strip()
        if not (stripped.startswith('"') and stripped.endswith('"')):
            continue
        try:
            value = ast.literal_eval(stripped)
        except (SyntaxError, ValueError) as error:
            raise GlobalUsageError("invalid inline assembly string") from error
        if not isinstance(value, str):
            raise GlobalUsageError("inline assembly fragment is not a string")
        pieces.append(value)
    return "".join(pieces)


def classify_relocated_word(
    relocation: str, instruction_word: int | None
) -> tuple[str, str]:
    if relocation == "R_MIPS_HI16":
        return "address", ""
    if relocation == "R_MIPS_26":
        return "address", ""
    if relocation != "R_MIPS_LO16" or instruction_word is None:
        return "unknown", ""
    opcode = instruction_word >> 26
    if opcode in MIPS_OPCODE_ACCESS:
        return MIPS_OPCODE_ACCESS[opcode]
    if opcode in {0x08, 0x09, 0x0D, 0x0F}:
        return "address", ""
    return "unknown", ""


def collect_inline_assembly_c(
    source_name: str,
    text: str,
    function: Function,
    symbols_by_name: dict[str, int],
    aliases_by_address: dict[int, set[str]],
    function_addresses: set[int],
    function_names: set[str],
    usages: dict[tuple[int, int, str], Usage],
) -> bool:
    assembly = inline_assembly_text(text)
    if assembly is None:
        return False
    last_word: int | None = None
    for line in assembly.splitlines():
        instruction = " ".join(line.strip().split())
        word_match = re.fullmatch(r"\.word\s+(0x[0-9A-Fa-f]{1,8})", instruction)
        if word_match is not None:
            last_word = int(word_match.group(1), 0)
            continue
        relocation_match = re.fullmatch(
            r"\.reloc\s+\.-4,\s*(R_MIPS_(?:HI16|LO16|26)),\s*"
            r"([A-Za-z_][A-Za-z0-9_]*)",
            instruction,
        )
        if relocation_match is None:
            continue
        relocation, name = relocation_match.groups()
        global_address = resolve_global(
            name,
            symbols_by_name,
            function_addresses,
            function_names,
        )
        if global_address is None:
            continue
        aliases_by_address[global_address].add(name)
        access, width = classify_relocated_word(relocation, last_word)
        add_usage(
            usages,
            global_address,
            function,
            "C",
            source_name,
            access,
            name,
            f"{instruction} after .word 0x{last_word:08X}"
            if last_word is not None
            else instruction,
            width,
        )
    return True


def add_usage(
    usages: dict[tuple[int, int, str], Usage],
    global_address: int,
    function: Function,
    evidence_source: str,
    evidence_path: str,
    access: str,
    observed_name: str,
    context: str,
    width: str = "",
) -> None:
    key = (global_address, function.address, evidence_source)
    usage = usages.setdefault(
        key,
        Usage(
            global_address=global_address,
            function=function,
            evidence_source=evidence_source,
            evidence_path=evidence_path,
        ),
    )
    usage.accesses.add(access)
    usage.observed_names.add(observed_name)
    if context:
        usage.contexts.add(context)
    if width:
        usage.widths.add(width)


def collect_c_usages(
    root: Path,
    matching_sources: dict[int, str],
    inventory_by_address: dict[int, Function],
    symbols_by_name: dict[str, int],
    aliases_by_address: dict[int, set[str]],
    function_addresses: set[int],
    function_names: set[str],
    usages: dict[tuple[int, int, str], Usage],
) -> None:
    addresses_by_source: dict[str, list[int]] = defaultdict(list)
    for address, source in matching_sources.items():
        function = inventory_by_address.get(address)
        if function is None:
            raise GlobalUsageError(
                f"matching C address {address:#010x} is absent from functions.csv"
            )
        if function.status != "matching_c":
            raise GlobalUsageError(
                f"{function.name} is in matching_c.json but has status "
                f"{function.status}"
            )
        addresses_by_source[source].append(address)

    known_names = set(symbols_by_name)
    for source_name in sorted(addresses_by_source):
        source_path = resolve_within(root, source_name, must_exist=True)
        text = source_path.read_text(encoding="utf-8")
        lines = text.splitlines()
        parsed_functions, top_level_tokens = parse_c_functions(text)
        parsed_by_name = {function.name: function for function in parsed_functions}
        if len(parsed_by_name) != len(parsed_functions):
            raise GlobalUsageError(f"{source_name} defines duplicate function names")
        widths, arrays = infer_declarations(top_level_tokens, known_names)
        for address in sorted(addresses_by_source[source_name]):
            function = inventory_by_address[address]
            parsed = parsed_by_name.get(function.name)
            if parsed is None:
                if (
                    len(addresses_by_source[source_name]) == 1
                    and collect_inline_assembly_c(
                        source_name,
                        text,
                        function,
                        symbols_by_name,
                        aliases_by_address,
                        function_addresses,
                        function_names,
                        usages,
                    )
                ):
                    continue
                available = ", ".join(sorted(parsed_by_name))
                raise GlobalUsageError(
                    f"{source_name}: could not isolate {function.name}; "
                    f"found: {available}"
                )
            for index, token in enumerate(parsed.tokens):
                if index and parsed.tokens[index - 1].value in {".", "->"}:
                    continue
                if (
                    index + 1 < len(parsed.tokens)
                    and parsed.tokens[index + 1].value == ":"
                ):
                    continue
                global_address = resolve_global(
                    token.value,
                    symbols_by_name,
                    function_addresses,
                    function_names,
                )
                if global_address is None:
                    continue
                aliases_by_address[global_address].add(token.value)
                add_usage(
                    usages,
                    global_address,
                    function,
                    "C",
                    source_name,
                    classify_c_access(parsed.tokens, index, arrays),
                    token.value,
                    source_context(lines, token.line),
                    widths.get(token.value, ""),
                )


def assembly_instruction(line: str) -> str:
    if "*/" in line:
        line = line.split("*/", 1)[1]
    return " ".join(line.strip().split())


def classify_assembly_access(
    opcode: str, relocation_kind: str | None
) -> tuple[str, str]:
    opcode = opcode.lower()
    if opcode in LOAD_WIDTHS:
        return "read", LOAD_WIDTHS[opcode]
    if opcode in STORE_WIDTHS:
        return "write", STORE_WIDTHS[opcode]
    if opcode == "lui" and relocation_kind == "hi":
        return "address", ""
    if opcode in {"la", "li", "addiu", "addi", "ori", "addu"}:
        return "address", ""
    if opcode in {"j", "jal", "jr", "jalr"}:
        return "address", ""
    if opcode in {".word", ".4byte"}:
        return "address", "32"
    if opcode in {".half", ".short", ".2byte"}:
        return "address", "16"
    if opcode in {".byte"}:
        return "address", "8"
    return "unknown", ""


def collect_assembly_usages(
    root: Path,
    assembly_root: Path,
    inventory_by_name: dict[str, Function],
    symbols_by_name: dict[str, int],
    aliases_by_address: dict[int, set[str]],
    function_addresses: set[int],
    function_names: set[str],
    usages: dict[tuple[int, int, str], Usage],
) -> None:
    expected = {
        function.name
        for function in inventory_by_name.values()
        if function.module == "game"
        and function.status in {"unmatched_asm", "handwritten_asm"}
    }
    found: set[str] = set()
    current: Function | None = None
    for assembly_path in sorted(assembly_root.rglob("*.s")):
        relative_path = assembly_path.relative_to(root).as_posix()
        with assembly_path.open("r", encoding="utf-8") as handle:
            for line in handle:
                label_match = FUNCTION_LABEL_RE.match(line)
                if label_match is not None:
                    name = label_match.group(1)
                    function = inventory_by_name.get(name)
                    if (
                        function is not None
                        and function.module == "game"
                        and function.status
                        in {"unmatched_asm", "handwritten_asm"}
                    ):
                        if name in found:
                            raise GlobalUsageError(
                                f"assembly function {name} appears more than once"
                            )
                        found.add(name)
                        current = function
                    else:
                        current = None
                    continue
                if current is None:
                    continue
                stripped = line.strip()
                if stripped.startswith("endlabel "):
                    current = None
                    continue
                instruction = assembly_instruction(line)
                if not instruction or instruction.startswith(".L"):
                    continue
                opcode = instruction.split(None, 1)[0]
                relocations = {
                    name: kind
                    for kind, name in RELOCATION_RE.findall(instruction)
                }
                candidates = set(relocations)
                for name in IDENTIFIER_RE.findall(instruction):
                    if name in symbols_by_name or GENERATED_NAME_RE.fullmatch(name):
                        candidates.add(name)
                for name in sorted(candidates):
                    global_address = resolve_global(
                        name,
                        symbols_by_name,
                        function_addresses,
                        function_names,
                    )
                    if global_address is None:
                        continue
                    aliases_by_address[global_address].add(name)
                    access, width = classify_assembly_access(
                        opcode, relocations.get(name)
                    )
                    add_usage(
                        usages,
                        global_address,
                        current,
                        "assembly",
                        relative_path,
                        access,
                        name,
                        instruction,
                        width,
                    )
    missing = sorted(expected - found)
    if missing:
        preview = ", ".join(missing[:10])
        raise GlobalUsageError(
            f"generated assembly is missing {len(missing)} game functions: {preview}"
        )


def preferred_name(names: set[str], address: int) -> str:
    if not names:
        return f"D_{address:08X}"

    def score(name: str) -> tuple[int, int, str]:
        if re.match(r"^g[A-Z_]", name):
            category = 4
        elif CODEGEN_ALIAS_RE.fullmatch(name):
            category = 0
        elif GENERATED_NAME_RE.fullmatch(name):
            category = 1
        elif name.endswith(("_start", "_end")) or name == "runtime_gp":
            category = 0
        else:
            category = 3
        return category, -len(name), name

    return max(names, key=score)


def combined_access(accesses: set[str]) -> str:
    if "read_write" in accesses or {"read", "write"}.issubset(accesses):
        return "read_write"
    if "read" in accesses:
        return "read"
    if "write" in accesses:
        return "write"
    if "address" in accesses:
        return "address"
    return "unknown"


def format_context(contexts: set[str]) -> str:
    ordered = sorted(contexts)
    if len(ordered) > 2:
        ordered = ordered[:2] + [f"(+{len(contexts) - 2} more)"]
    return " | ".join(ordered)


def render_csv(
    usages: dict[tuple[int, int, str], Usage],
    aliases_by_address: dict[int, set[str]],
) -> str:
    output = io.StringIO(newline="")
    fieldnames = [
        "global_address",
        "global_name",
        "observed_names",
        "function_address",
        "function_name",
        "function_status",
        "function_module",
        "evidence_source",
        "evidence_path",
        "access",
        "width_bits",
        "context",
    ]
    writer = csv.DictWriter(output, fieldnames=fieldnames, lineterminator="\n")
    writer.writeheader()
    for usage in sorted(
        usages.values(),
        key=lambda item: (
            item.global_address,
            item.function.address,
            item.evidence_source,
        ),
    ):
        writer.writerow(
            {
                "global_address": f"0x{usage.global_address:08X}",
                "global_name": preferred_name(
                    aliases_by_address[usage.global_address],
                    usage.global_address,
                ),
                "observed_names": "|".join(sorted(usage.observed_names)),
                "function_address": f"0x{usage.function.address:08X}",
                "function_name": usage.function.name,
                "function_status": usage.function.status,
                "function_module": usage.function.module,
                "evidence_source": usage.evidence_source,
                "evidence_path": usage.evidence_path,
                "access": combined_access(usage.accesses),
                "width_bits": "|".join(
                    sorted(usage.widths, key=lambda value: int(value))
                ),
                "context": format_context(usage.contexts),
            }
        )
    return output.getvalue()


def render_summary(
    usages: dict[tuple[int, int, str], Usage],
    aliases_by_address: dict[int, set[str]],
) -> str:
    by_global: dict[int, list[Usage]] = defaultdict(list)
    for usage in usages.values():
        by_global[usage.global_address].append(usage)
    rankings = []
    for address, items in by_global.items():
        users = {item.function.address for item in items}
        accesses = {
            item.function.address: combined_access(item.accesses) for item in items
        }
        read_count = sum(
            access in {"read", "read_write"} for access in accesses.values()
        )
        write_count = sum(
            access in {"write", "read_write"} for access in accesses.values()
        )
        address_count = sum(access == "address" for access in accesses.values())
        c_count = len(
            {
                item.function.address
                for item in items
                if item.evidence_source == "C"
            }
        )
        assembly_count = len(
            {
                item.function.address
                for item in items
                if item.evidence_source == "assembly"
            }
        )
        rankings.append(
            (
                -len(users),
                address,
                preferred_name(aliases_by_address[address], address),
                len(users),
                read_count,
                write_count,
                address_count,
                c_count,
                assembly_count,
            )
        )
    rankings.sort()
    function_users = {
        usage.function.address for usage in usages.values()
    }
    c_rows = sum(
        usage.evidence_source == "C" for usage in usages.values()
    )
    assembly_rows = len(usages) - c_rows
    lines = [
        "# Game Global Usage",
        "",
        "Generated by `make global-usage` from matching C bodies, remaining "
        "game assembly, and the tracked symbol files. Psy-Q functions are excluded.",
        "",
        f"- Globals referenced: **{len(by_global)}**",
        f"- Game functions with global evidence: **{len(function_users)}**",
        f"- Function/global rows: **{len(usages)}** "
        f"({c_rows} C, {assembly_rows} assembly)",
        "",
        "## Most widely used globals",
        "",
        "| Rank | Address | Preferred name | Users | Read | Write | Address | C | ASM |",
        "| ---: | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]
    for rank, item in enumerate(rankings[:50], 1):
        (
            _,
            address,
            name,
            users,
            read_count,
            write_count,
            address_count,
            c_count,
            assembly_count,
        ) = item
        lines.append(
            f"| {rank} | `0x{address:08X}` | `{name}` | {users} | "
            f"{read_count} | {write_count} | {address_count} | "
            f"{c_count} | {assembly_count} |"
        )
    lines.extend(
        [
            "",
            "Counts are unique game functions. `read_write` contributes to both "
            "the read and write columns. The complete evidence, aliases, widths, "
            "and contexts are in [`global-usage.csv`](global-usage.csv).",
            "",
        ]
    )
    return "\n".join(lines)


def write_or_check(
    path: Path, content: str, check: bool, *, strict: bool = True
) -> bool:
    """Write the report, or verify it and report whether it is current.

    A strict path raises when it is stale. A non-strict path returns False
    instead, which lets an aggregate summary be reported as a snapshot rather
    than blocking every concurrent change that regenerates it.
    """
    if check:
        try:
            existing = path.read_text(encoding="utf-8")
        except FileNotFoundError as error:
            raise GlobalUsageError(f"{path} is missing; regenerate it") from error
        if existing != content:
            if not strict:
                return False
            raise GlobalUsageError(f"{path} is stale; regenerate it")
        return True
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f".{path.name}.new")
    try:
        temporary.write_text(content, encoding="utf-8", newline="")
        temporary.replace(path)
    finally:
        temporary.unlink(missing_ok=True)
    return True


def generate(root: Path) -> tuple[str, str, int, int]:
    inventory_path = resolve_within(
        root, "config/slus_01411/functions.csv", must_exist=True
    )
    matching_path = resolve_within(
        root, "config/slus_01411/matching_c.json", must_exist=True
    )
    symbol_paths = [
        resolve_within(root, "config/slus_01411/symbols.txt", must_exist=True),
        resolve_within(root, "config/slus_01411/c_symbols.ld", must_exist=True),
    ]
    assembly_root = resolve_within(root, "tmp/splat/asm", must_exist=True)

    inventory = load_inventory(inventory_path)
    inventory_by_address = {
        function.address: function for function in inventory
    }
    inventory_by_name = {function.name: function for function in inventory}
    matching_sources = load_matching_sources(matching_path)
    symbols_by_name, aliases_by_address, symbol_function_names = load_symbols(
        symbol_paths
    )
    function_addresses = set(inventory_by_address)
    function_names = set(inventory_by_name) | symbol_function_names
    usages: dict[tuple[int, int, str], Usage] = {}

    collect_c_usages(
        root,
        matching_sources,
        inventory_by_address,
        symbols_by_name,
        aliases_by_address,
        function_addresses,
        function_names,
        usages,
    )
    collect_assembly_usages(
        root,
        assembly_root,
        inventory_by_name,
        symbols_by_name,
        aliases_by_address,
        function_addresses,
        function_names,
        usages,
    )
    csv_text = render_csv(usages, aliases_by_address)
    summary_text = render_summary(usages, aliases_by_address)
    global_count = len({usage.global_address for usage in usages.values()})
    return csv_text, summary_text, len(usages), global_count


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Map game-global usage in matching C and remaining assembly."
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="verify that the tracked reports are up to date",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        csv_text, summary_text, row_count, global_count = generate(root)
        csv_path = resolve_within(root, "notes/global-usage.csv")
        summary_path = resolve_within(root, "notes/global-usage.md")
        write_or_check(csv_path, csv_text, args.check)
        summary_current = write_or_check(
            summary_path, summary_text, args.check, strict=False
        )
    except (
        GlobalUsageError,
        WorkspaceError,
        OSError,
        UnicodeError,
        ValueError,
        KeyError,
        TypeError,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    action = "verified" if args.check else "wrote"
    print(f"{action}: notes/global-usage.csv ({row_count} rows)")
    if summary_current:
        print(f"{action}: notes/global-usage.md ({global_count} globals)")
    else:
        print(
            f"stale snapshot: notes/global-usage.md "
            f"({global_count} globals); regenerate with make global-usage"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
