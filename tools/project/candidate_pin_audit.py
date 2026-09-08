#!/usr/bin/env python3
"""Audit a candidate entry's register pins with a marginal *and* a joint arm.

Dropping each `register ... asm("...")` pin on its own and finding them all
inert does not show that they are jointly inert. Pins competing over the same
small register pool protect one another: each holds a register GCC would
otherwise take for a value whose pin remains, so no single removal changes
anything and the interaction only appears once several are gone.

Both outcomes occur in this project. On `func_80012E5C` and `func_80023144` the
pins that measure inert alone are also inert together and are genuine
decoration. On `func_80046294` five pins each measure the baseline with the
*identical* differing set when dropped alone and cost six positions when
dropped together.

The audit therefore reports three things per pin: the differing count, whether
the differing *set* is unchanged (two candidates agreeing on a count can be
structurally far apart), and finally the joint arm over every pin that looked
inert on its own.

Usage:

    tools/environments/python/bin/python tools/project/candidate_pin_audit.py \
        func_80046294

The candidate source and profile are read from the entry's Markdown note under
`notes/candidates/`; nothing is written outside `tmp/`.
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import subprocess
import sys
from pathlib import Path

from workspace import (
    WorkspaceError,
    local_environment,
    require_workspace_root,
    resolve_within,
)

FUNCTIONS_CSV = "config/slus_01411/functions.csv"
PROFILES_JSON = "config/slus_01411/compiler_profiles.json"
CANDIDATES_DIR = "notes/candidates"
TARGET = "game/SLUS_014.11"
MASPSX = "tools/vendor/maspsx/maspsx.py"
BINUTILS = "tools/toolchains/binutils-2.42/bin"
# One directory per function: a shared work path lets two runs clobber
# each other's object file, which fails as "symbol not found".
WORK = "tmp/pin-audit"
TEXT_VMA_TO_FILE = 0x8000F800

PIN = re.compile(
    r'^(\s*)register\s+(.+?)\s*(?:__asm__|asm)\s*\(\s*"([^"]+)"\s*\)'
    r"(\s*=\s*[^;]+)?;",
    re.M,
)


def variable_name(declarator: str) -> str:
    """The declared name, which is not always the last identifier.

    `void (*fn)(void)` ends in `void`; the name sits inside the parentheses
    that bind the pointer.
    """
    pointer = re.search(r"\(\s*\*+\s*(\w+)\s*\)", declarator)
    if pointer:
        return pointer.group(1)
    return re.findall(r"\w+", declarator)[-1]


def pin_sites(source: str) -> list[tuple[int, int, str, str]]:
    """Every pinned declaration, identified by position rather than by name.

    Two blocks can declare the same variable pinned to the same register, so
    matching on the declarator alone collapses them into one and the audit
    silently measures the first twice.
    """
    return [(m.start(), m.end(), m.group(2), m.group(3))
            for m in PIN.finditer(source)]


def drop_pins(source: str, indices) -> str:
    """Rewrite the selected pinned declarations as plain ones."""
    sites = pin_sites(source)
    out = source
    for index in sorted(indices, reverse=True):
        start, end, declarator, _ = sites[index]
        match = PIN.match(source, start)
        indent = match.group(1)
        init = match.group(4) or ""
        out = out[:start] + f"{indent}{declarator}{init};" + out[end:]
    return out


def entry_source(root: Path, name: str) -> tuple[str, str]:
    note = resolve_within(root, f"{CANDIDATES_DIR}/{name}.md")
    if not note.is_file():
        raise WorkspaceError(f"no candidate note for {name}")
    text = note.read_text()
    profile = re.search(r"`(gcc_[a-z0-9_]+)`", text)
    blocks = re.findall(r"```c\n(.*?)```", text, re.S)
    if not profile or not blocks:
        raise WorkspaceError(f"{name}: note has no source block or profile")
    source = max(blocks, key=len)
    # Entry sources are written as they would sit in src/game/, so their
    # includes are relative to that directory. Rebase them on src/ instead.
    source = source.replace('#include "../types.h"', '#include "types.h"')
    source = source.replace('#include "../psyq/', '#include "psyq/')
    source = re.sub(r'#include "([a-z_0-9]+\.h)"',
                    r'#include "game/\1"', source)
    source = source.replace('#include "game/types.h"', '#include "types.h"')
    return source, profile.group(1)


def inventory(root: Path) -> dict[str, tuple[int, int]]:
    entries = {}
    with resolve_within(root, FUNCTIONS_CSV).open() as handle:
        for row in csv.DictReader(handle):
            entries[row["name"]] = (int(row["address"], 16),
                                    int(row["size"], 16))
    return entries


def _run_compiler(root, profile, candidate, raw, env):
    result = subprocess.run(
        [str(resolve_within(root, profile["compiler"])), "-S",
         "-I", str(resolve_within(root, "src")),
         *profile["compiler_flags"], "-o", str(raw), str(candidate)],
        cwd=root, env=env, capture_output=True, text=True,
    )
    if result.returncode != 0:
        detail = (result.stderr or result.stdout).strip().splitlines()
        raise WorkspaceError(
            "the entry's stored source does not compile:\n  "
            + "\n  ".join(detail[:6])
        )


def build(root: Path, source: str, profile: dict, env: dict,
          symbol: str) -> Path:
    work = resolve_within(root, f"{WORK}/{symbol}")
    work.mkdir(parents=True, exist_ok=True)
    candidate = work / "candidate.c"
    candidate.write_text(source)
    raw = work / "candidate.s"
    translated = work / "candidate.maspsx.s"
    obj = work / "candidate.o"
    _run_compiler(root, profile, candidate, raw, env)
    with raw.open("rb") as fin, translated.open("wb") as fout:
        subprocess.run(
            [sys.executable, str(resolve_within(root, MASPSX)),
             f"--aspsx-version={profile['aspsx_version']}",
             *profile["maspsx_flags"]],
            cwd=root, env=env, stdin=fin, stdout=fout, check=True,
        )
    subprocess.run(
        [str(resolve_within(root, f"{BINUTILS}/mipsel-none-elf-as")),
         "-EL", "-mips1", f"-G{profile['data_limit']}",
         "-o", str(obj), str(translated)],
        cwd=root, env=env, check=True, capture_output=True,
    )
    return obj


def symbol_table(root: Path) -> dict[str, int]:
    """Every symbol address the project fixes, for resolving relocations.

    Masking relocated immediates instead would make `lui $v1,%hi(A)` and
    `lui $v1,%hi(B)` compare equal, so a swapped pair of symbols would read as
    a match. Resolving them keeps the comparison exact.
    """
    symbols: dict[str, int] = {}
    for name in ("config/slus_01411/symbols.txt",
                 "config/slus_01411/c_symbols.ld",
                 "config/slus_01411/link_symbols.ld"):
        path = resolve_within(root, name)
        if not path.is_file():
            continue
        for line in path.read_text().splitlines():
            match = re.match(r"\s*([A-Za-z_]\w*)\s*=\s*(0x[0-9A-Fa-f]+)\s*;",
                             line)
            if match:
                symbols[match.group(1)] = int(match.group(2), 16)
    with resolve_within(root, FUNCTIONS_CSV).open() as handle:
        for row in csv.DictReader(handle):
            symbols.setdefault(row["name"], int(row["address"], 16))
    return symbols


def address_of(symbol: str, symbols: dict[str, int]) -> int | None:
    if symbol in symbols:
        return symbols[symbol]
    match = re.fullmatch(r"[A-Za-z]+_([0-9A-Fa-f]{8})", symbol)
    return int(match.group(1), 16) if match else None


def relocate(word, kind, symbol, symbols, gp, lo_addend=0):
    """Apply one relocation, or return None when it cannot be resolved.

    MIPS uses REL relocations, so the addend lives in the instruction's own
    immediate field. Replacing the whole immediate with the symbol value
    discards it and silently mis-resolves every reference to `sym + k`, which
    then reads as a difference. A paired `%hi`/`%lo` splits the addend between
    the two instructions, so the high half needs its partner's low half.
    """
    value = address_of(symbol, symbols)
    if value is None:
        return None
    imm = word & 0xFFFF
    if imm & 0x8000:
        imm -= 0x10000
    if kind == "R_MIPS_HI16":
        full = value + (imm << 16) + lo_addend
        return (word & 0xFFFF0000) | (((full + 0x8000) >> 16) & 0xFFFF)
    if kind == "R_MIPS_LO16":
        return (word & 0xFFFF0000) | ((value + imm) & 0xFFFF)
    if kind == "R_MIPS_GPREL16":
        if gp is None:
            return None
        return (word & 0xFFFF0000) | ((value + imm - gp) & 0xFFFF)
    if kind == "R_MIPS_26":
        target = value + ((word & 0x03FFFFFF) << 2)
        return (word & 0xFC000000) | ((target >> 2) & 0x03FFFFFF)
    return None


def object_function(root: Path, obj: Path, symbol: str, env: dict):
    """Words and .text relocations for `symbol`.

    `objdump -r` prints one block per section and the offsets restart in each,
    so the current section header has to be tracked. Without that a .rodata
    jump-table entry against .text collides with a .text offset and silently
    overrides the real relocation there.
    """
    objdump = str(resolve_within(root, f"{BINUTILS}/mipsel-none-elf-objdump"))
    table = subprocess.run([objdump, "-t", str(obj)], cwd=root, env=env,
                           capture_output=True, text=True, check=True).stdout
    start = size = None
    for line in table.splitlines():
        fields = line.split()
        if fields[-1:] == [symbol] and ".text" in line:
            start = int(fields[0], 16)
            size = int(fields[-2], 16)
            break
    if start is None:
        raise WorkspaceError(f"{symbol} not found in {obj}")
    headers = subprocess.run([objdump, "-h", str(obj)], cwd=root, env=env,
                             capture_output=True, text=True, check=True).stdout
    offset = None
    for line in headers.splitlines():
        parts = line.split()
        if len(parts) > 5 and parts[1] == ".text":
            offset = int(parts[5], 16)
    blob = obj.read_bytes()[offset + start:offset + start + size]
    words = [int.from_bytes(blob[i:i + 4], "little")
             for i in range(0, len(blob), 4)]
    dump = subprocess.run([objdump, "-r", str(obj)], cwd=root, env=env,
                          capture_output=True, text=True, check=True).stdout
    relocations = {}
    section = None
    for line in dump.splitlines():
        header = re.match(r"RELOCATION RECORDS FOR \[([^\]]+)\]", line)
        if header:
            section = header.group(1)
            continue
        if section != ".text":
            continue
        parts = line.split()
        if len(parts) == 3 and parts[1].startswith("R_MIPS"):
            try:
                position = int(parts[0], 16)
            except ValueError:
                continue
            if start <= position < start + size:
                relocations[(position - start) // 4] = (parts[1], parts[2])
    return words, relocations


def target_words(root: Path, address: int, size: int) -> list[int]:
    image = resolve_within(root, TARGET)
    if not image.is_file():
        raise WorkspaceError(f"missing retail image at {TARGET}")
    start = address - TEXT_VMA_TO_FILE
    blob = image.read_bytes()[start:start + size]
    return [int.from_bytes(blob[i:i + 4], "little")
            for i in range(0, len(blob), 4)]


def differing(build_words, retail, relocations, symbols, gp):
    """Positions whose linked bytes differ, relocations resolved exactly."""
    positions = []
    for index, (left, right) in enumerate(zip(build_words, retail)):
        relocation = relocations.get(index)
        if relocation is None:
            same = left == right
        else:
            lo_addend = 0
            if relocation[0] == "R_MIPS_HI16":
                for later in range(index + 1, len(build_words)):
                    pair = relocations.get(later)
                    if (pair and pair[0] == "R_MIPS_LO16"
                            and pair[1] == relocation[1]):
                        lo_addend = build_words[later] & 0xFFFF
                        if lo_addend & 0x8000:
                            lo_addend -= 0x10000
                        break
            fixed = relocate(left, relocation[0], relocation[1], symbols, gp,
                             lo_addend)
            if fixed is not None:
                same = fixed == right
            elif relocation[0] == "R_MIPS_26":
                same = left >> 26 == right >> 26
            else:
                same = left == right or left >> 16 == right >> 16
        if not same:
            positions.append(index * 4)
    return tuple(positions)


def measure(root, source, profile, symbol, address, size, env, symbols):
    obj = build(root, source, profile, env, symbol)
    words, relocations = object_function(root, obj, symbol, env)
    retail = target_words(root, address, size)
    if len(words) != len(retail):
        return None, None, (len(words), len(retail))
    gp = symbols.get("runtime_gp")
    positions = differing(words, retail, relocations, symbols, gp)
    return len(positions), positions, (len(words), len(retail))


def main() -> int:
    parser = argparse.ArgumentParser(
        description="audit a candidate's register pins, marginally and jointly",
    )
    parser.add_argument("name", help="function name such as func_80046294")
    parser.add_argument(
        "--exhaustive", action="store_true",
        help="measure every subset of pins, not just single drops and the "
             "jointly-inert arm; a greedy search can stop before the best "
             "configuration",
    )
    parser.add_argument(
        "--max-pins", type=int, default=8,
        help="refuse --exhaustive above this many pins (default 8)",
    )
    args = parser.parse_args()

    root = require_workspace_root()
    env = {**local_environment(root)}
    source, profile_name = entry_source(root, args.name)
    profiles = json.loads(
        resolve_within(root, PROFILES_JSON).read_text())["profiles"]
    if profile_name not in profiles:
        raise WorkspaceError(f"unknown profile {profile_name}")
    profile = profiles[profile_name]
    symbols = symbol_table(root)
    entries = inventory(root)
    if args.name not in entries:
        raise WorkspaceError(f"{args.name} is not in {FUNCTIONS_CSV}")
    address, size = entries[args.name]

    base_count, base_set, counts = measure(
        root, source, profile, args.name, address, size, env, symbols)
    print(f"{args.name}  profile={profile_name}  "
          f"instructions={counts[0]}/{counts[1]}  baseline={base_count}")
    if base_count is None:
        print("  instruction count differs from the target; audit aborted")
        return 1

    sites = pin_sites(source)
    if not sites:
        print("  no register pins in the stored source")
        return 0

    inert = []
    for index, (_, _, declarator, register) in enumerate(sites):
        count, positions, _ = measure(
            root, drop_pins(source, [index]), profile, args.name, address,
            size, env, symbols)
        same = positions == base_set
        note = ""
        if count == base_count and same:
            inert.append(index)
            note = "  <- inert alone"
        elif count is not None and count < base_count:
            note = "  <- BETTER than the baseline"
        print(f"  drop {variable_name(declarator):<12s} {register:<5s} -> "
              f"{str(count):>4s}  "
              f"{'same set' if same else 'DIFFERENT set'}{note}")

    label = lambda idx: variable_name(sites[idx][2])

    if args.exhaustive:
        if len(sites) > args.max_pins:
            print(f"  --exhaustive needs at most {args.max_pins} pins; "
                  f"this entry has {len(sites)}")
            return 1
        print(f"  enumerating all {2 ** len(sites)} subsets "
              f"(a greedy single-drop search can stop short of the best one)")
        best = (base_count, ())
        for mask in range(1, 2 ** len(sites)):
            chosen = [i for i in range(len(sites)) if mask >> i & 1]
            count, positions, _ = measure(
                root, drop_pins(source, chosen), profile, args.name, address,
                size, env, symbols)
            if count is not None and count < best[0]:
                best = (count, tuple(chosen))
        if best[1]:
            names = ", ".join(label(i) for i in best[1])
            print(f"  BEST subset: drop [{names}] -> {best[0]} "
                  f"(baseline {base_count})")
        else:
            print(f"  no subset beats the baseline of {base_count}")
        return 0

    if len(inert) < 2:
        print("  fewer than two pins look inert; no joint arm to run")
        print("  (pass --exhaustive to search every subset instead)")
        return 0

    count, positions, _ = measure(
        root, drop_pins(source, inert), profile, args.name, address, size,
        env, symbols)
    names = ", ".join(label(i) for i in inert)
    if count == base_count and positions == base_set:
        verdict = "jointly inert - these pins are decoration and can be removed"
    else:
        verdict = ("JOINTLY LOAD-BEARING - the marginal arms are misleading; "
                   "keep them")
    print(f"  JOINT drop [{names}] -> {count}  {verdict}")
    print("  (pass --exhaustive to search every subset; dropping two pins can "
          "beat dropping either one)")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except WorkspaceError as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
