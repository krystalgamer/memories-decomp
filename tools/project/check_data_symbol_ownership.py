#!/usr/bin/env python3
"""Check that no C data definition collides with a linker-script assignment.

A symbol can be given an address in `c_symbols.ld` *and* defined in a C
translation unit. The assignment wins, the definition never loads, and the
source then describes storage the image does not take from it. The byte-exact
build cannot see this: the bytes still come from somewhere, so the executable
matches and nothing complains. Every data carve has therefore had to be
checked by hand, symbol by symbol.

That is what this automates. For each unit in `data_c.json` it reads the
initialized definitions out of the source and reports any whose name is also
assigned in a linker script.

It deliberately does **not** report tentative definitions -- a global written
without `extern` and without an initializer, which the compiler emits as a
common symbol. Those look like exactly the same mistake and are not one. The
spelling is a deliberate `-G` lever: a common symbol is placed in small data
and addressed through `$gp`, and writing `extern` instead moves it. Fifteen
of them exist today, and rewriting three in `fade_update.c` -- whose profile
`gcc_2_8_1_cc_g8_as_g1_split_comm` is named for this very property -- shrank
the executable from 0x1D0800 to 0x1D07F4 bytes. A check that flagged them
would report fifteen defects, all of them load-bearing, so it reports none.

The distinction is the point: an *initialized* definition that duplicates an
assignment is inert and wrong, and an *uninitialized* one is how several units
reach small data at all.

    tools/environments/python/bin/python \\
        tools/project/check_data_symbol_ownership.py
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

DATA_C_JSON = "config/slus_01411/data_c.json"
LINKER_SCRIPTS = (
    "config/slus_01411/c_symbols.ld",
    "config/slus_01411/link_symbols.ld",
)

_ASSIGNMENT = re.compile(r"^\s*([A-Za-z_]\w*)\s*=\s*[^=]")
_COMMENT = re.compile(r"/\*.*?\*/|//[^\n]*", re.S)


def strip_comments(text: str) -> str:
    return _COMMENT.sub(" ", text)


def strip_attributes(text: str) -> str:
    """Remove __attribute__((...)) runs, matching nested parentheses.

    A regex cannot do this: section attributes contain their own parentheses,
    as in __attribute__((section(".sdata"))), so a non-greedy match stops at
    the wrong one.
    """
    out = []
    i = 0
    while True:
        start = text.find("__attribute__", i)
        if start < 0:
            out.append(text[i:])
            return "".join(out)
        out.append(text[i:start])
        j = text.find("(", start)
        if j < 0:
            return "".join(out) + text[start:]
        depth = 0
        while j < len(text):
            if text[j] == "(":
                depth += 1
            elif text[j] == ")":
                depth -= 1
                if depth == 0:
                    j += 1
                    break
            j += 1
        i = j


def linker_assignments(root: Path) -> dict[str, str]:
    found: dict[str, str] = {}
    for rel in LINKER_SCRIPTS:
        path = root / rel
        if not path.exists():
            continue
        for number, line in enumerate(path.read_text().splitlines(), 1):
            if "=" not in line:
                continue
            match = _ASSIGNMENT.match(strip_comments(line))
            if match:
                found.setdefault(match.group(1), f"{rel}:{number}")
    return found


_FUNCTION_POINTER = re.compile(r"\(\s*\*+\s*([A-Za-z_]\w*)\s*(?:\[[^\]]*\])*\s*\)")


def initialized_definitions(source: str) -> list[str]:
    """Names defined with an initializer at file scope.

    Splitting on `;` is safe here because an initializer never contains one,
    and it keeps declarations that wrap across lines in one piece -- several
    carves put the attribute on its own line, so a line-based reader would
    miss the `=`.

    Function-pointer tables need their own pattern. `void (*D_80090DF8[])(u8 *)`
    declares its name inside parentheses, so the rule that takes the last
    identifier before `=` would read the parameter type instead. An earlier
    draft skipped every declaration containing `(` to avoid function bodies
    and silently dropped eight real tables, which is the worst thing a check
    like this can do: it reported success over symbols it had never looked at.
    """
    text = strip_attributes(strip_comments(source))
    names = []
    for chunk in text.split(";"):
        if "=" not in chunk:
            continue
        head = chunk.split("=", 1)[0]
        if "{" in head:
            continue
        words = head.split()
        if not words or words[0] in ("extern", "static", "typedef"):
            continue
        pointer = _FUNCTION_POINTER.search(head)
        if pointer:
            names.append(pointer.group(1))
            continue
        if "(" in head:
            continue
        name = re.sub(r"\[[^\]]*\]", " ", head).split()
        if not name:
            continue
        candidate = name[-1].lstrip("*")
        if re.fullmatch(r"[A-Za-z_]\w*", candidate):
            names.append(candidate)
    return names


def tentative_definitions(source: str) -> list[str]:
    """Names declared at file scope with neither `extern` nor an initializer.

    Reported only so the count can be stated; these are the `-G` lever the
    module docstring describes and are never treated as violations.
    """
    text = strip_attributes(strip_comments(source))
    names = []
    for chunk in text.split(";"):
        if "=" in chunk or "(" in chunk or "{" in chunk:
            continue
        words = chunk.split()
        if len(words) < 2 or words[0] in ("extern", "static", "typedef"):
            continue
        candidate = re.sub(r"\[[^\]]*\]", " ", chunk).split()[-1].lstrip("*")
        if re.fullmatch(r"[A-Za-z_]\w*", candidate):
            names.append(candidate)
    return names


def check(root: Path) -> tuple[list[str], int, int]:
    assigned = linker_assignments(root)
    units = json.loads((root / DATA_C_JSON).read_text())["units"]
    problems = []
    defined = 0
    tentative = 0
    for unit in units:
        rel = unit["source"]
        path = root / rel
        if not path.exists():
            problems.append(f"{rel}: listed in data_c.json but missing")
            continue
        source = path.read_text()
        tentative += len(tentative_definitions(source))
        for name in initialized_definitions(source):
            defined += 1
            if name in assigned:
                problems.append(
                    f"{rel}: {name} is defined here and assigned in "
                    f"{assigned[name]}; the assignment wins and this "
                    f"definition never loads"
                )
    return problems, defined, tentative


def self_test() -> None:
    sample = """
    #include "../types.h"
    /* a comment with = in it */
    u32 D_1 __attribute__((section(".sdata"))) = 0x10;
    volatile u16 D_2[4] __attribute__((section(".sdata"))) = {
        0x0, 0x20,
    };
    char *D_3
        __attribute__((section(".sdata"))) = &thing;
    extern u32 D_4;
    static u32 sPad __attribute__((section(".sdata"))) = 0;
    u8 D_5;
    void (*D_6[])(u8 *) = { f, g };
    void (*D_7)(void) = f;
    void f(int x) { int y = x; }
    """
    got = initialized_definitions(sample)
    for want in ("D_1", "D_2", "D_3", "D_6", "D_7"):
        assert want in got, f"self-test: {want} missing from {got}"
    for unwanted in ("D_4", "sPad", "y", "u8", "void"):
        assert unwanted not in got, f"self-test: {unwanted} wrongly reported"
    assert "D_5" in tentative_definitions(sample)
    assert "D_1" not in tentative_definitions(sample)
    assert strip_attributes('u32 x __attribute__((section(".sdata"))) = 1;') \
        .split("=")[0].split()[-1] == "x"
    print("check_data_symbol_ownership self-test: ok")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if args.self_test:
        self_test()
        return 0
    problems, defined, tentative = check(ROOT)
    if problems:
        for line in problems:
            print(f"error: {line}", file=sys.stderr)
        return 1
    print(
        f"data symbol ownership: {defined} initialized definitions clear of "
        f"linker assignments ({tentative} tentative definitions not checked)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
