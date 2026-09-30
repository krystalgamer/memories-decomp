#!/usr/bin/env python3
"""Check the guest-width pointer annotations (G32, CALL32) in src/.

src/port_ptr.h defines both macros. They expand to nothing here, so no build
or other check notices when new code leaves them out, and a native 64-bit
build (MEMORIES_PC) would silently get an 8-byte pointer inside a structure
the game lays out with 4. This reads every C file and header under src/ as
text, with no compiler and no retail input, and rejects:

  members  a structure or union member that is a pointer, without G32 after
           each '*' of its declarator (`T *G32 p`, `void (*G32 f)(void)`),
           or after the typedef name for a pointer typedef (`Callback G32 f`);
  globals  the same for a file-scope pointer that no declaration in src/
           initializes: it is pinned to a retail address;
  locals   a local `T **p` that is given guest pointer storage, directly or
           through another such local: `p = gTable`, `p = &o->slots[i]`,
           `p = o->slots + 1` (write `T *G32 *p`). An explicit cast, such as
           `p = (T **)o->slots`, is the author's choice and is not reported;
  calls    a call through a G32 function pointer (a member, a pinned global,
           or a function-pointer table local that walks guest storage) that
           is not written `CALL32(type, f)(args)`.

Accepted exceptions:

  - A name that some declaration initializes (`T *p = &x;`) is data of the
    native build's own .data, so it stays a native pointer everywhere,
    including its plain extern declarations; D_8009AF18 is the documented
    case. G32 on such a name is allowed (D_8009B074 is `= 0` and G32).
  - MainMenuComparators (main_menu/module_rodata.h) only holds a static
    initializer's table of native function pointers; its members stay plain.

Every preprocessor branch is read, so each region's declarations are checked,
except `#if 0` and C++-only ones. Names are resolved by spelling, not by type:
the 64-bit compile ("changes address space of nested pointer", or LLVM
failing to lower a call) stays the complete check for what text cannot see,
such as guest storage reached through a helper's return value.

--fix inserts G32 at every member, global and local finding; a call needs the
callee's type for CALL32 and is left to the author.
"""

from __future__ import annotations

import argparse
import re
import sys
import time
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

EXEMPT_RECORDS = {"MainMenuComparators"}
MACRO_HEADER = "src/port_ptr.h"

KEYWORDS = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned",
    "void", "volatile", "while", "__inline", "__inline__", "__volatile__",
    "__const", "__signed__", "_Bool", "__asm__", "asm", "__attribute__",
    "__extension__", "__restrict", "__restrict__", "restrict", "G32", "CALL32",
}
BASIC_TYPE_WORDS = {
    "void", "char", "short", "int", "long", "float", "double", "signed",
    "unsigned", "_Bool", "__signed__",
}
QUALIFIERS = {"const", "volatile", "G32", "__restrict", "__restrict__",
              "restrict", "__const", "__volatile__"}
STORAGE = {"typedef", "extern", "static", "register", "auto", "inline",
           "__inline", "__inline__"}
DROP_GROUPS = {"__attribute__", "__asm__", "asm", "__asm"}
DROP_WORDS = {"__extension__"}

TOKEN = re.compile(
    r"""
    (?P<ws>[ \t\r\f\v]+)
  | (?P<str>"(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*')
  | (?P<id>[A-Za-z_]\w*)
  | (?P<num>\.?\d(?:[eEpP][+-]|[\w.])*)
  | (?P<op>->|\+\+|--|<<=|>>=|<<|>>|<=|>=|==|!=|&&|\|\||[-+*/%&|^!=]=|\#\#|\.\.\.|.)
    """,
    re.S | re.X,
)
COMMENT_OR_STRING = re.compile(
    r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.S
)
IDENT = re.compile(r"[A-Za-z_]\w*\Z")
DIRECTIVE = re.compile(r"\s*#\s*(\w*)(.*)", re.S)


class Tok(str):
    """A token: its text, plus the line it came from."""

    line: int

    def __new__(cls, text: str, line: int) -> "Tok":
        token = super().__new__(cls, text)
        token.line = line
        return token


def strip_comments(text: str) -> str:
    def keep_lines(match: re.Match) -> str:
        found = match.group(0)
        if found.startswith("/"):
            return " " + "\n" * found.count("\n")
        return found

    return COMMENT_OR_STRING.sub(keep_lines, text)


def events(text: str):
    """Yield ("tok", Tok) and ("dir", name, argument) in source order."""
    lines = strip_comments(text).split("\n")
    number = 0
    while number < len(lines):
        start = number
        line = lines[number]
        while line.endswith("\\") and number + 1 < len(lines):
            number += 1
            line = line[:-1] + " " + lines[number]
        number += 1
        directive = DIRECTIVE.match(line)
        if directive and line.lstrip().startswith("#"):
            yield ("dir", directive.group(1), directive.group(2).strip())
            continue
        for match in TOKEN.finditer(line):
            if match.lastgroup != "ws":
                yield ("tok", Tok(match.group(0), start + 1))


# -- #if conditions: True, False, or None (unknown: read every branch) -----

FALSE_MACROS = {"__cplusplus", "_LANGUAGE_C_PLUS_PLUS", "c_plusplus",
                "__LANGUAGE_ASSEMBLY", "_LANGUAGE_ASSEMBLY", "LANGUAGE_ASSEMBLY"}
TRUE_MACROS = {"_LANGUAGE_C", "LANGUAGE_C", "__LANGUAGE_C"}


class Tri:
    def __init__(self, value):
        self.value = value

    def __and__(self, other):
        if self.value is False or other.value is False:
            return Tri(False)
        if self.value is None or other.value is None:
            return Tri(None)
        return Tri(True)

    def __or__(self, other):
        if self.value is True or other.value is True:
            return Tri(True)
        if self.value is None or other.value is None:
            return Tri(None)
        return Tri(False)

    def __invert__(self):
        return Tri(None if self.value is None else not self.value)


def condition(kind: str, text: str):
    if kind in ("ifdef", "ifndef"):
        name = text.split()[0] if text.split() else ""
        value = (True if name in TRUE_MACROS else
                 False if name in FALSE_MACROS else None)
        if value is None:
            return None
        return value if kind == "ifdef" else not value

    def macro(match: re.Match) -> str:
        name = match.group(1) or match.group(2)
        if name in TRUE_MACROS:
            return " T "
        if name in FALSE_MACROS:
            return " F "
        return " U "

    expression = re.sub(r"defined\s*\(\s*(\w+)\s*\)|defined\s+(\w+)", macro, text)
    if re.search(r"[<>]|==|!=|[-+*/%?:]", expression):
        return None
    expression = re.sub(r"\b\d+[uUlL]*\b",
                        lambda m: " T " if int(re.sub(r"[uUlL]", "", m.group(0)), 0) else " F ",
                        expression)
    expression = re.sub(r"\b(?!T\b|F\b|U\b)[A-Za-z_]\w*\b", " U ", expression)
    expression = expression.replace("&&", "&").replace("||", "|").replace("!", "~")
    if not re.fullmatch(r"[\sTFU&|~()]*", expression) or not expression.strip():
        return None
    try:
        return eval(expression, {"__builtins__": {}},
                    {"T": Tri(True), "F": Tri(False), "U": Tri(None)}).value
    except Exception:
        return None


# -- declarations -----------------------------------------------------------


def matching(tokens: list, index: int, opening: str, closing: str) -> int:
    """Index of the bracket matching tokens[index] (searching forward)."""
    depth = 0
    for position in range(index, len(tokens)):
        if tokens[position] == opening:
            depth += 1
        elif tokens[position] == closing:
            depth -= 1
            if depth == 0:
                return position
    return len(tokens) - 1


def matching_back(tokens: list, index: int, opening: str, closing: str) -> int:
    depth = 0
    for position in range(index, -1, -1):
        if tokens[position] == closing:
            depth += 1
        elif tokens[position] == opening:
            depth -= 1
            if depth == 0:
                return position
    return -1


def clean(tokens: list) -> list:
    """Drop __attribute__((...)), asm("...") labels and __extension__."""
    out = []
    index = 0
    while index < len(tokens):
        token = tokens[index]
        if token in DROP_GROUPS and index + 1 < len(tokens) and tokens[index + 1] == "(":
            index = matching(tokens, index + 1, "(", ")") + 1
            continue
        if token not in DROP_WORDS:
            out.append(token)
        index += 1
    return out


def split_top(tokens: list, separator: str) -> list:
    parts, current, depth = [], [], 0
    for token in tokens:
        if token in ("(", "["):
            depth += 1
        elif token in (")", "]"):
            depth -= 1
        if token == separator and depth == 0:
            parts.append(current)
            current = []
        else:
            current.append(token)
    parts.append(current)
    return parts


class Declarator:
    __slots__ = ("name", "line", "stars", "missing", "g32", "base", "function",
                 "function_pointer", "initialized", "array", "record_base", "init")

    def __repr__(self) -> str:  # pragma: no cover - debugging aid
        return (f"Declarator({self.name}@{self.line} stars={self.stars} "
                f"missing={self.missing} g32={self.g32} base={self.base})")


def declaration(tokens: list):
    """Split one declaration into (specifier words, [Declarator])."""
    tokens = clean(tokens)
    if not tokens:
        return set(), []
    # Specifiers: keywords, struct/union/enum tags, record placeholders and
    # at most one typedef name when no basic type word is present.
    index = 0
    base: list = []
    words: set = set()
    typed = False
    spec_g32 = False
    record_base = False
    while index < len(tokens):
        token = tokens[index]
        if token in ("struct", "union", "enum"):
            typed = True
            words.add(token)
            index += 1
            if index < len(tokens) and re.match(r"[A-Za-z_]", tokens[index]) \
                    and tokens[index] not in KEYWORDS:
                base.append(token + " " + tokens[index])
                index += 1
            continue
        if token.startswith("\0record"):
            typed = True
            record_base = True
            index += 1
            continue
        if token == "G32":
            spec_g32 = True
            index += 1
            continue
        if token in STORAGE or token in QUALIFIERS:
            words.add(token)
            index += 1
            continue
        if token in BASIC_TYPE_WORDS:
            typed = True
            words.add(token)
            index += 1
            continue
        if re.match(r"[A-Za-z_]", token) and token not in KEYWORDS and not typed:
            typed = True
            base.append(token)
            index += 1
            continue
        break
    found = []
    for chunk in split_top(tokens[index:], ","):
        item = declarator(chunk)
        if item is None:
            continue
        item.base = base
        item.record_base = record_base
        item.g32 = item.g32 or spec_g32
        found.append(item)
    return words, found


def old_style_header(tokens: list) -> bool:
    """`T f(a, b) s32 a;`: a K&R definition whose parameters follow."""
    tokens = clean(tokens)
    for index in range(1, len(tokens)):
        if tokens[index] == "(" and IDENT.match(tokens[index - 1]):
            close = matching(tokens, index, "(", ")")
            names = tokens[index + 1:close]
            return bool(names) and close + 1 < len(tokens) \
                and IDENT.match(tokens[close + 1]) is not None \
                and all(IDENT.match(t) or t == "," for t in names)
    return False


def declarator(tokens: list):
    # The name is the first identifier not inside a parameter list.
    name = None
    for index, token in enumerate(tokens):
        if token in ("*", "(") or token in QUALIFIERS:
            continue
        if re.match(r"[A-Za-z_]", token) and token not in KEYWORDS:
            name = index
        break
    if name is None:
        return None
    item = Declarator()
    item.name = str(tokens[name])
    item.line = tokens[name].line
    # The pointer run is the '*'s written directly before the name.
    stars = []
    index = name - 1
    while index >= 0 and (tokens[index] == "*" or tokens[index] in QUALIFIERS):
        if tokens[index] == "*":
            stars.append(index)
        index -= 1
    stars.reverse()
    missing = 0
    for position, star in enumerate(stars):
        end = stars[position + 1] if position + 1 < len(stars) else name
        if "G32" not in tokens[star + 1:end]:
            missing += 1
    item.stars = len(stars)
    item.missing = missing
    item.g32 = any(token == "G32" for token in tokens[:name]) and not stars
    after = name + 1
    item.function = after < len(tokens) and tokens[after] == "("
    item.array = after < len(tokens) and tokens[after] == "["
    while after < len(tokens) and tokens[after] == "[":
        after = matching(tokens, after, "[", "]") + 1
    closing = after
    while closing < len(tokens) and tokens[closing] == ")":
        closing += 1
    item.function_pointer = bool(stars) and closing > after and \
        closing < len(tokens) and tokens[closing] == "("
    depth = 0
    item.initialized = False
    item.init = []
    for index in range(name, len(tokens)):
        token = tokens[index]
        if token in ("(", "["):
            depth += 1
        elif token in (")", "]"):
            depth -= 1
        elif token == "=" and depth <= 0:
            item.initialized = True
            item.init = tokens[index + 1:]
            break
    return item


# -- one file ---------------------------------------------------------------


class Frame:
    __slots__ = ("kind", "buffer", "records", "record_id", "function", "scopes",
                 "old_style")

    def __init__(self, kind: str, record_id=None, function=None, scopes=()):
        self.kind = kind          # file, record, func, block, init, enum
        self.buffer: list = []
        self.records: list = []   # records whose body closed in this statement
        self.record_id = record_id
        self.function = function  # index into Unit.functions (func, block)
        self.scopes = scopes      # enclosing block ids, innermost last
        self.old_style = False    # between a K&R header and its body

    def clone(self) -> "Frame":
        other = Frame(self.kind, self.record_id, self.function, self.scopes)
        other.buffer = list(self.buffer)
        other.records = list(self.records)
        other.old_style = self.old_style
        return other


class Function:
    """The `T **` locals of one function body and what is assigned to them."""

    def __init__(self):
        self.pointer_pointers: dict = {}   # (block, name) -> Declarator
        self.sources: list = []            # ((block, name), line, root)
        self.fn_tables: set = set()        # G32 function-pointer locals
        self.blocks = 0

    def resolve(self, scopes: tuple, name: str):
        for scope in reversed(scopes):
            if (scope, name) in self.pointer_pointers:
                return (scope, name)
        return None

    def root(self, scopes: tuple, tokens: list):
        root = postfix_root(tokens)
        if root and root[0] == "ident":
            key = self.resolve(scopes, root[1])
            if key is not None:
                return ("local", key)
        return root


class Unit:
    """What one file declares and calls, before cross-file resolution."""

    def __init__(self, path: str):
        self.path = path
        self.members: list = []       # (record id, Declarator)
        self.record_names: dict = {}  # record id -> {typedef and tag names}
        self.record_parent: dict = {}
        self.globals: list = []       # Declarator
        self.typedefs: list = []      # Declarator
        self.functions: list = []     # Function
        self.calls: list = []         # (line, kind, name or (function, local key))
        self.call32 = 0


NOT_DECLARATION = {"return", "goto", "case", "default", "if", "while",
                   "switch", "sizeof", "break", "continue"}


def parse(path: str, text: str) -> Unit:
    unit = Unit(path)
    stack = [Frame("file")]
    conditions: list = []   # [mode, snapshot at #if, state after 1st branch]
    skipping = 0            # #if depth inside a skipped branch
    records = [0]

    def snapshot() -> list:
        return [frame.clone() for frame in stack]

    def statement(frame: Frame) -> None:
        tokens, closed = frame.buffer, frame.records
        frame.buffer, frame.records = [], []
        if not tokens:
            return
        if frame.kind == "record":
            words, found = declaration(tokens)
            unit.members.extend((frame.record_id, item) for item in found)
            name_records(tokens, closed, words, found)
        elif frame.kind == "file":
            if tokens[0] in DROP_GROUPS or frame.old_style:
                return
            if old_style_header(tokens):
                frame.old_style = True
                return
            words, found = declaration(tokens)
            name_records(tokens, closed, words, found)
            (unit.typedefs if "typedef" in words else unit.globals).extend(found)
        elif frame.function is not None:
            local(unit.functions[frame.function], frame.scopes, tokens)

    def name_records(tokens, closed, words, found) -> None:
        if "typedef" in words:
            for record in closed:
                unit.record_names.setdefault(record, set()).update(
                    item.name for item in found)
        for index in range(len(tokens) - 2):
            if tokens[index] in ("struct", "union") and tokens[index + 2] in closed:
                unit.record_names.setdefault(str(tokens[index + 2]), set()).add(
                    str(tokens[index + 1]))

    def local(function: Function, scopes: tuple, tokens: list) -> None:
        body = tokens
        if body[:2] == ["for", "("]:
            body = body[2:]
        while body and body[0] in ("else", "do"):
            body = body[1:]
        declared = set()
        if body and IDENT.match(body[0]) and body[0] not in NOT_DECLARATION:
            words, found = declaration(body)
            for item in found:
                if item.function or not (item.base or words & BASIC_TYPE_WORDS or item.record_base):
                    continue
                declared.add(item.name)
                if item.function_pointer and item.missing < item.stars:
                    function.fn_tables.add(item.name)
                if item.stars >= 2:
                    key = (scopes[-1], item.name)
                    function.pointer_pointers[key] = item
                    if item.init:
                        function.sources.append(
                            (key, item.line, function.root(scopes, item.init)))
        # Assignments `p = ...` to a known T ** local, anywhere in the statement.
        for index in range(len(tokens) - 1):
            name = tokens[index]
            if tokens[index + 1] != "=" or name in declared:
                continue
            key = function.resolve(scopes, name)
            if key is None:
                continue
            if index and tokens[index - 1] in ("->", ".", "*", "]"):
                continue
            right = []
            depth = 0
            for token in tokens[index + 2:]:
                if token in ("(", "["):
                    depth += 1
                elif token in (")", "]"):
                    depth -= 1
                    if depth < 0:
                        break
                elif token in (",", ";") and depth == 0:
                    break
                right.append(token)
            function.sources.append((key, name.line, function.root(scopes, right)))

    def call(frame: Frame) -> None:
        tokens = frame.buffer
        if len(tokens) < 2:
            return
        line = tokens[-1].line
        function = unit.functions[frame.function]
        if tokens[-2] == ")":
            opening = matching_back(tokens, len(tokens) - 2, "(", ")")
            if opening > 0 and tokens[opening - 1] == "CALL32":
                unit.call32 += 1
                return
            inner = tokens[opening + 1:-2]
            if inner[:1] != ["*"]:
                return  # a cast, or a call of a call's result
            inner = inner[1:]
            while inner[:1] == ["("] and inner[-1:] == [")"]:
                inner = inner[1:-1]
            callee(frame, inner, line)
            return
        callee(frame, tokens[:-1], line)

    def callee(frame: Frame, tokens: list, line: int) -> None:
        function = unit.functions[frame.function]
        end = len(tokens) - 1
        while end >= 0 and tokens[end] == "]":
            end = matching_back(tokens, end, "[", "]") - 1
        if end < 0 or not IDENT.match(tokens[end]) or tokens[end] in KEYWORDS:
            return
        name = str(tokens[end])
        key = function.resolve(frame.scopes, name)
        if end >= 1 and tokens[end - 1] in ("->", "."):
            unit.calls.append((line, "member", name))
        elif key is not None and function.pointer_pointers[key].function_pointer:
            # through a `T (**p)()` local: hot when p walks guest storage
            unit.calls.append((line, "table", (frame.function, key)))
        elif name in function.fn_tables:
            unit.calls.append((line, "local", name))
        else:
            unit.calls.append((line, "ident", name))

    def open_brace(token: Tok) -> None:
        frame = stack[-1]
        tokens = frame.buffer
        kind = None
        for index in range(len(tokens) - 1, max(len(tokens) - 3, -1), -1):
            if tokens[index] in ("struct", "union", "enum"):
                if all(IDENT.match(t) for t in tokens[index + 1:]):
                    kind = "enum" if tokens[index] == "enum" else "record"
                break
        if kind is None:
            if frame.kind in ("init", "enum", "record") or "=" in tokens:
                kind = "init"
            elif frame.kind == "file":
                kind = "func" if ")" in tokens or frame.old_style else "file"
                frame.old_style = False
            else:
                kind = "block"
        if kind == "record":
            records[0] += 1
            record = f"\0record{records[0]}"
            unit.record_parent[record] = frame.record_id
            frame.buffer.append(Tok(record, token.line))
            frame.records.append(record)
            stack.append(Frame("record", record_id=record, function=frame.function,
                               scopes=frame.scopes))
        elif kind in ("enum", "init"):
            frame.buffer.append(Tok("\0" + kind, token.line))
            stack.append(Frame(kind))
        elif kind == "func":
            frame.buffer = []
            unit.functions.append(Function())
            stack.append(Frame("func", function=len(unit.functions) - 1, scopes=(0,)))
        else:
            if frame.function is not None:
                statement(frame)  # `if (p = x) {`: the header is a statement too
                function = unit.functions[frame.function]
                function.blocks += 1
                scopes = frame.scopes + (function.blocks,)
            else:
                scopes = ()
            frame.buffer = []
            stack.append(Frame(kind, function=frame.function, scopes=scopes))

    def close_brace() -> None:
        if len(stack) == 1:
            return
        frame = stack.pop()
        if frame.kind not in ("init", "enum"):
            statement(frame)
        if frame.kind in ("func", "block", "file"):
            stack[-1].buffer = []

    for event in events(text):
        if event[0] == "dir":
            kind, argument = event[1], event[2]
            if kind in ("if", "ifdef", "ifndef"):
                if skipping:
                    skipping += 1
                    continue
                value = condition(kind, argument)
                if value is False:
                    conditions.append(["skip-then-take", None, None])
                    skipping = 1
                elif value is True:
                    conditions.append(["take-then-skip", None, None])
                else:
                    conditions.append(["all", snapshot(), None])
            elif kind in ("elif", "else"):
                if skipping > 1 or not conditions:
                    continue
                entry = conditions[-1]
                if entry[0] == "all":
                    if entry[2] is None:
                        entry[2] = snapshot()
                    stack[:] = [frame.clone() for frame in entry[1]]
                elif entry[0] == "skip-then-take":
                    value = False if kind == "elif" and condition("if", argument) is False else None
                    if value is None:
                        skipping = 0
                        entry[0] = "taken" if kind == "else" else "all"
                        entry[1] = snapshot()
                elif entry[0] in ("take-then-skip", "taken"):
                    skipping = 1
                    entry[0] = "skip-rest"
            elif kind == "endif":
                if skipping > 1:
                    skipping -= 1
                    continue
                if not conditions:
                    continue
                entry = conditions.pop()
                skipping = 0
                if entry[0] == "all" and entry[2] is not None:
                    stack[:] = entry[2]
            continue
        if skipping:
            continue
        token = event[1]
        frame = stack[-1]
        if token == "{":
            open_brace(token)
        elif token == "}":
            close_brace()
        elif frame.kind in ("init", "enum"):
            continue
        elif token == ";":
            statement(frame)
        else:
            frame.buffer.append(token)
            if token == "(" and frame.function is not None and frame.kind != "record":
                call(frame)
    return unit


# -- the whole tree ---------------------------------------------------------


def postfix_root(tokens: list):
    """For `&a->b[i]`, `a.b + 1`, `&g[2]`, `g`: ("member", "b") or
    ("ident", "g"); None for a cast, a dereference, a call or anything else."""
    tokens = list(tokens)
    while tokens[:1] == ["&"]:
        tokens = tokens[1:]
    while tokens[:1] == ["("] and matching(tokens, 0, "(", ")") == len(tokens) - 1:
        tokens = tokens[1:-1]
        while tokens[:1] == ["&"]:
            tokens = tokens[1:]
    if not tokens:
        return None
    if tokens[0] == "(":
        close = matching(tokens, 0, "(", ")")
        inner = tokens[1:close]
        if inner and IDENT.match(inner[0]) and close + 1 < len(tokens) and \
                (IDENT.match(tokens[close + 1]) or tokens[close + 1] in ("(", "&", "*")) \
                and tokens[close + 1:close + 2] != ["->"]:
            return None  # (T **)expr: an explicit conversion
        index = close + 1
        root = None
    elif IDENT.match(tokens[0]) and tokens[0] not in KEYWORDS:
        root = ("ident", str(tokens[0]))
        index = 1
    else:
        return None
    while index < len(tokens):
        token = tokens[index]
        if token == "[":
            index = matching(tokens, index, "[", "]") + 1
        elif token in ("->", ".") and index + 1 < len(tokens):
            root = ("member", str(tokens[index + 1]))
            index += 2
        elif token == "(":
            # a call (or function-like macro) result: only its members count
            index = matching(tokens, index, "(", ")") + 1
            root = None
        else:
            break
    return root


def check(units: list):
    counts: Counter = Counter()
    sites: list = []   # (path, line, kind, name) of every annotated site
    findings: list = []

    # Typedefs, to a fixed point (a typedef of a pointer typedef).
    pointer_typedefs, function_typedefs, fnptr_typedefs = set(), set(), set()
    typedefs = [item for unit in units for item in unit.typedefs]
    changed = True
    while changed:
        changed = False
        for item in typedefs:
            base = item.base[-1] if item.base else None
            groups = []
            if item.function:
                groups.append(function_typedefs)
            else:
                if item.stars or (not item.array and base in pointer_typedefs):
                    groups.append(pointer_typedefs)
                if item.function_pointer or (item.stars == 1 and base in function_typedefs) \
                        or (not item.stars and not item.array and base in fnptr_typedefs):
                    groups.append(fnptr_typedefs)
            for group in groups:
                if item.name not in group:
                    group.add(item.name)
                    changed = True

    def pointer(item) -> bool:
        if item.function:
            return False
        return bool(item.stars) or (bool(item.base) and item.base[-1] in pointer_typedefs)

    def function_pointer(item) -> bool:
        base = item.base[-1] if item.base else None
        return item.function_pointer or (item.stars == 1 and base in function_typedefs) \
            or (not item.stars and base in fnptr_typedefs)

    def annotated(item) -> bool:
        return item.missing == 0 if item.stars else item.g32

    def fix(item, stars: str):
        return (item.name, stars if item.stars else "typedef")

    def how(item) -> str:
        return "after each '*'" if item.stars else "after the typedef name"

    def exempt(unit: Unit, record) -> bool:
        while record is not None:
            if unit.record_names.get(record, set()) & EXEMPT_RECORDS:
                return True
            record = unit.record_parent.get(record)
        return False

    guest_members, fn_members = set(), set()
    for unit in units:
        for record, item in unit.members:
            if not pointer(item) or exempt(unit, record):
                continue
            guest_members.add(item.name)
            if function_pointer(item):
                fn_members.add(item.name)
            if annotated(item):
                sites.append((unit.path, item.line, "member", item.name))
            else:
                findings.append((unit.path, item.line,
                                 f"member '{item.name}' is a stored pointer: "
                                 f"write G32 {how(item)}", fix(item, "all")))

    initialized = {item.name for unit in units for item in unit.globals
                   if item.initialized and not item.function}
    guest_globals, fn_globals = set(), set()
    for unit in units:
        for item in unit.globals:
            if not pointer(item) or item.name in initialized:
                continue
            guest_globals.add(item.name)
            if function_pointer(item):
                fn_globals.add(item.name)
            if annotated(item):
                sites.append((unit.path, item.line, "global", item.name))
            else:
                findings.append((unit.path, item.line,
                                 f"global '{item.name}' has no initializer, so it is "
                                 f"pinned to a retail address: write G32 {how(item)}",
                                 fix(item, "all")))

    for unit in units:
        hot: dict = {}
        for number, function in enumerate(unit.functions):
            guest = {key for key, item in function.pointer_pointers.items()
                     if item.missing < item.stars}
            sites.extend((unit.path, item.line, "local", item.name)
                         for key, item in function.pointer_pointers.items() if key in guest)
            flagged: dict = {}
            changed = True
            while changed:
                changed = False
                for key, line, root in function.sources:
                    if key in guest or key in flagged or root is None:
                        continue
                    kind, name = root
                    if kind == "member" and name in guest_members or \
                            kind == "ident" and name in guest_globals or \
                            kind == "local" and (name in guest or name in flagged):
                        flagged[key] = line
                        changed = True
            hot[number] = guest | set(flagged)
            for key in flagged:
                item = function.pointer_pointers[key]
                findings.append((unit.path, item.line,
                                 f"local '{item.name}' walks guest pointer storage "
                                 f"(line {flagged[key]}): write T *G32 *{item.name}",
                                 fix(item, "first")))
        for line, kind, name in unit.calls:
            if kind == "local" or kind == "member" and name in fn_members or \
                    kind == "ident" and name in fn_globals or \
                    kind == "table" and name[1] in hot.get(name[0], ()):
                name = name[1][1] if kind == "table" else name
                findings.append((unit.path, line,
                                 f"call through the G32 function pointer '{name}': "
                                 "write CALL32(type, f)(args)", None))

    for _, _, kind, _ in set(sites):
        counts[kind] += 1
    counts["call32"] = sum(unit.call32 for unit in units)
    counts["files"] = len(units)
    return sorted(set(findings)), counts, sorted(set(sites))


def apply_fix(text: str, line: int, name: str, stars: str):
    """Insert G32 into one declaration line; None when the text is unusual."""
    lines = text.split("\n")
    source = lines[line - 1]
    for match in re.finditer(rf"\b{re.escape(name)}\b", source):
        start = match.start()
        if stars == "typedef":
            if re.search(r"[A-Za-z_]\w*\s+$", source[:start]):
                lines[line - 1] = source[:start] + "G32 " + source[start:]
                return "\n".join(lines)
            continue
        position = start
        while position > 0 and source[position - 1] in " \t(":
            position -= 1
        found = []
        while True:
            qualifier = re.search(r"(?:const|volatile)\s*$", source[:position])
            if qualifier:
                position = qualifier.start()
            elif position > 0 and source[position - 1] in " \t":
                position -= 1
            elif position > 0 and source[position - 1] == "*":
                found.append(position)
                position -= 1
            else:
                break
        if not found:
            continue
        found.reverse()  # leftmost first
        for star in (found[:1] if stars == "first" else found)[::-1]:
            space = "" if source[star:star + 1] in (" ", "\t") else " "
            source = source[:star] + "G32" + space + source[star:]
        lines[line - 1] = source
        return "\n".join(lines)
    return None


def fix_tree(findings: list, root: Path = ROOT) -> int:
    """Write G32 where the findings say; CALL32 needs the callee type, so
    calls are left for a person. Returns how many sites were left."""
    left = 0
    by_path: dict = {}
    for path, line, message, hint in findings:
        by_path.setdefault(path, []).append((line, hint, message))
    for path, items in sorted(by_path.items()):
        file = root / path
        with open(file, encoding="utf-8", errors="surrogateescape", newline="") as handle:
            text = handle.read()
        for line, hint, message in sorted(items, reverse=True, key=lambda x: x[0]):
            fixed = apply_fix(text, line, *hint) if hint else None
            if fixed is None:
                left += 1
                print(f"{path}:{line}: not fixed: {message}", file=sys.stderr)
            else:
                text = fixed
        with open(file, "w", encoding="utf-8", errors="surrogateescape", newline="") as out:
            out.write(text)
    return left


def sources(root: Path = ROOT) -> list:
    return sorted(path for path in (root / "src").rglob("*")
                  if path.suffix in (".c", ".h") and path.is_file())


def run(root: Path = ROOT):
    units = []
    for path in sources(root):
        relative = path.relative_to(root).as_posix()
        if relative == MACRO_HEADER:
            continue
        text = path.read_text(encoding="utf-8", errors="surrogateescape")
        units.append(parse(relative, text))
    return check(units)


# -- self-test --------------------------------------------------------------

# (source, expected finding lines). Each case is its own file.
CASES = (
    # Members: every '*' of the declarator, function pointers, typedefs.
    ("typedef struct { u8 *G32 a; u8 *b; void (*G32 f)(void); void (*g)(void);\n"
     "  u8 *G32 *G32 c; u8 *G32 *d; s32 n; u8 *G32 t[4]; u8 *u[4]; } S;\n", {1, 2}),
    ("typedef void (*Callback)(void);\n"
     "struct T { Callback G32 ok;\n Callback bad; };\n", {3}),
    # Nested and anonymous records, bit-fields, regional branches.
    ("struct U { union { u8 *G32 x;\n u16 *y; } v; s32 bits : 3; };\n"
     "#ifdef VERSION_EUROPE\nstruct V { u8 *G32 e;\n#else\nstruct V { u8 *e;\n#endif\n};\n",
     {2, 6}),
    # #if 0 and C++ branches are never read.
    ("#if 0\nstruct W { u8 *p; };\n#endif\n"
     "#if defined(__cplusplus)\nextern \"C\" {\n#endif\nstruct X { u8 *G32 q; };\n", set()),
    # Exempt record.
    ("typedef struct {\n s32 (*entries[6])();\n} MainMenuComparators;\n", set()),
    # Globals: pinned vs initialized; functions are not globals.
    ("extern u8 *G32 gA;\nextern u8 *gB;\nextern u8 *gC;\nu8 *gC = 0;\n"
     "u8 *Func(void);\nvoid (*gTable[])(void) = { 0 };\nextern void (*gPinned[4])(void);\n"
     "static u8 *G32 volatile gV asm(\"gV\");\n", {2, 7}),
    # Locals and calls.
    ("typedef struct { u8 *G32 slots[4]; void (*G32 cb)(int); } O;\n"
     "extern O *G32 gO;\nextern void (*G32 gFn)(void);\n"
     "void f(O *o) {\n"
     "    u8 **a = &o->slots[1];\n"
     "    u8 *G32 *b = o->slots;\n"
     "    u8 **c;\n"
     "    u8 **d = (u8 **)o->slots;\n"
     "    u8 **e;\n"
     "    c = b + 1;\n"
     "    e = a;\n"
     "    o->cb(1);\n"
     "    CALL32(void (*)(int), o->cb)(2);\n"
     "    gFn();\n"
     "    (*o->cb)(3);\n"
     "}\n", {5, 7, 9, 12, 14, 15}),
    # K&R definitions: parameter declarations are not globals, and an
    # attribute before a typedef name is not a K&R header.
    ("void g(a, b)\nu8 *a;\ns32 b;\n{\n    u8 *p;\n}\n"
     "typedef struct { s16 x; } __attribute__((packed)) P;\nextern u8 *gAfter;\n", {8}),
    # A macro or call result as the base, same-named locals in other blocks,
    # and a plain function-pointer table local that walks a pinned table.
    ("typedef struct { u8 *G32 streams[2]; } Owner;\n"
     "extern void (*G32 gHandlers[4])(u8 *);\n"
     "void h(void *o) {\n"
     "    u8 **s = &OWNER(o)->streams[1];\n"
     "    { u8 *G32 *t = &OWNER(o)->streams[0]; }\n"
     "    { u8 **t = (u8 **)o; }\n"
     "    void (**table)(u8 *);\n"
     "    table = gHandlers;\n"
     "    table[1](0);\n"
     "    CALL32(void (*)(u8 *), gHandlers[2])(0);\n"
     "}\n", {4, 7, 9}),
)

# (line, name, stars, fixed line) for --fix.
FIXES = (
    ("    u8 *data;", "data", "all", "    u8 *G32 data;"),
    ("    u8 **pp;", "pp", "all", "    u8 *G32 *G32 pp;"),
    ("    u8 **pp = x;", "pp", "first", "    u8 *G32 *pp = x;"),
    ("    void (*cb[4])(void);", "cb", "all", "    void (*G32 cb[4])(void);"),
    ("extern T *volatile gV;", "gV", "all", "extern T *G32 volatile gV;"),
    ("    unsigned long*  addr;", "addr", "all", "    unsigned long*G32  addr;"),
    ("    Callback update;", "update", "typedef", "    Callback G32 update;"),
)


def self_test() -> int:
    failures = 0
    for number, (source, expected) in enumerate(CASES, 1):
        unit = parse(f"case{number}.c", source)
        found = {finding[1] for finding in check([unit])[0]}
        if found != expected:
            failures += 1
            print(f"check-g32 self-test case {number}: expected lines "
                  f"{sorted(expected)}, found {sorted(found)}", file=sys.stderr)
    for line, name, stars, expected in FIXES:
        fixed = apply_fix(line, 1, name, stars)
        if fixed != expected:
            failures += 1
            print(f"check-g32 self-test fix: {line!r} gave {fixed!r}, "
                  f"expected {expected!r}", file=sys.stderr)
    if failures:
        return 1
    print(f"check-g32 self-test: {len(CASES)} cases and {len(FIXES)} fixes OK")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--self-test", action="store_true",
                        help="run the built-in cases instead of reading src/")
    parser.add_argument("--report", action="store_true",
                        help="also print how many annotated sites of each kind exist")
    parser.add_argument("--fix", action="store_true",
                        help="insert G32 at every member, global and local finding")
    args = parser.parse_args()
    if args.self_test:
        return self_test()
    started = time.monotonic()
    findings, counts, _ = run()
    if args.fix and findings:
        left = fix_tree(findings)
        print(f"check-g32: annotated {len(findings) - left} site(s), {left} left")
        findings, counts, _ = run()
    for path, line, message, _ in findings:
        print(f"{path}:{line}: {message}")
    summary = (f"check-g32: {counts['files']} files, {counts['member']} G32 members, "
               f"{counts['global']} G32 globals, {counts['local']} T *G32 * locals, "
               f"{counts['call32']} CALL32 calls ({time.monotonic() - started:.1f}s)")
    if findings:
        print(summary, file=sys.stderr)
        print(f"check-g32: {len(findings)} unannotated site(s); see src/port_ptr.h",
              file=sys.stderr)
        return 1
    if args.report:
        print(summary)
    return 0


if __name__ == "__main__":
    sys.exit(main())
