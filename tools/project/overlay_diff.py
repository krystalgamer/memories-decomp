#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import json
import re
import struct
import subprocess
import sys
from pathlib import Path, PurePosixPath
from typing import Any

from build_baseline import BuildError, compile_c, load_compiler_profiles, tool
from workspace import WorkspaceError, require_workspace_root, resolve_within


class OverlayDiffError(RuntimeError):
    pass


BUILD_DIRECTORY = "tmp/overlay-diff"
DEFAULT_PROFILE = "gcc_2_8_1_g0_split"
OBJDUMP_LINE = re.compile(r"\s*[0-9a-f]+:\t([0-9a-f]{8}) \t?(.*)")
RELOCATION_LINE = re.compile(r"^([0-9a-f]{8}) (R_MIPS_\S+)")
SYMBOL_LINE = re.compile(r"^([0-9a-f]{8}) (.{7}) \.text\t([0-9a-f]{8}) (\S+)$")

# An unlinked object leaves every relocated field zero where the module holds
# the resolved value, so those fields cannot be compared. Everything outside
# them still is, which is what catches a wrong register or instruction.
RELOCATION_MASKS = {
    "R_MIPS_26": 0xFC000000,
    "R_MIPS_HI16": 0xFFFF0000,
    "R_MIPS_LO16": 0xFFFF0000,
    "R_MIPS_GOT16": 0xFFFF0000,
    "R_MIPS_GPREL16": 0xFFFF0000,
    "R_MIPS_16": 0xFFFF0000,
    "R_MIPS_32": 0x00000000,
}


def load_module(root: Path, name: str) -> dict[str, Any]:
    path = resolve_within(root, "config/slus_01411/overlays.json", must_exist=True)
    manifest = json.loads(path.read_text(encoding="utf-8"))
    for module in manifest.get("modules", []):
        if module.get("name") == name:
            if module.get("layout") is None:
                raise OverlayDiffError(f"{name}: module has no build layout")
            return module
    names = sorted(str(m.get("name")) for m in manifest.get("modules", []))
    raise OverlayDiffError(f"unknown module {name}; known modules: {', '.join(names)}")


def inventory_entry(root: Path, name: str, address: int) -> dict[str, str]:
    path = resolve_within(
        root, f"config/slus_01411/overlays/{name}_functions.csv", must_exist=True
    )
    with path.open(encoding="utf-8", newline="") as handle:
        for row in csv.DictReader(handle):
            if int(row["address"], 16) == address:
                return row
    raise OverlayDiffError(f"{name}: {address:#010x} is not in the inventory")


def configured_source(root: Path, name: str, address: int) -> str | None:
    path = resolve_within(
        root, f"config/slus_01411/overlays/{name}_matching_c.json", must_exist=True
    )
    manifest = json.loads(path.read_text(encoding="utf-8"))
    for entry in manifest.get("functions", []):
        if int(str(entry["address"]), 16) == address:
            return str(entry["source"])
    return None


def target_words(root: Path, module: dict[str, Any], address: int, size: int) -> list[int]:
    binary = resolve_within(root, str(module["output"]), must_exist=True)
    load_address = int(str(module["load_address"]), 16)
    offset = address - load_address
    data = binary.read_bytes()
    if offset < 0 or offset + size > len(data):
        raise OverlayDiffError(
            f"{address:#010x}+{size:#x} lies outside {binary.name}"
        )
    return list(struct.unpack(f"<{size // 4}I", data[offset : offset + size]))


def relocation_masks(root: Path, obj: Path, count: int) -> list[int]:
    masks = [0xFFFFFFFF] * count
    for line in run_objdump(root, ["-r", "--section=.text", str(obj)]).splitlines():
        match = RELOCATION_LINE.match(line)
        if not match:
            continue
        kind = match.group(2)
        if kind not in RELOCATION_MASKS:
            raise OverlayDiffError(f"unhandled relocation {kind} in {obj.name}")
        index = int(match.group(1), 16) // 4
        if index < count:
            masks[index] &= RELOCATION_MASKS[kind]
    return masks


def text_symbol(root: Path, obj: Path, name: str) -> tuple[int, int]:
    """Offset and size of one function inside a possibly grouped object."""
    for line in run_objdump(root, ["-t", "--section=.text", str(obj)]).splitlines():
        match = SYMBOL_LINE.match(line)
        if match and match.group(4) == name and "F" in match.group(2):
            return int(match.group(1), 16), int(match.group(3), 16)
    raise OverlayDiffError(f"{obj.name} defines no .text symbol {name}")


def candidate_words(
    root: Path, source: str, profile: str, address: int, name: str, size: int
) -> tuple[list[int], list[str], list[int]]:
    segment = {
        "source": source,
        "profile": profile,
        "address": f"{address:#010x}",
        "object": str(PurePosixPath(source).with_suffix(".o")),
    }
    obj = compile_c(
        root,
        tool(root, "as"),
        segment,
        load_compiler_profiles(root),
        object_directory=BUILD_DIRECTORY,
        asm_directory=f"{BUILD_DIRECTORY}/asm",
    )
    listing = run_objdump(root, ["-d", "--section=.text", str(obj)])
    words: list[int] = []
    text: list[str] = []
    for line in listing.splitlines():
        match = OBJDUMP_LINE.match(line)
        if match:
            words.append(int(match.group(1), 16))
            text.append(match.group(2).replace("\t", " ").strip())
    offset, symbol_size = text_symbol(root, obj, name)
    if symbol_size != size:
        # A candidate that is not yet the right length is the normal state of
        # a function being worked on, and the diff is what shows why. Slicing
        # by the object's own symbol still compares the right function.
        print(
            f"note: candidate is {symbol_size:#x} bytes against {size:#x} "
            "in the inventory"
        )
    masks = relocation_masks(root, obj, len(words))
    start, stop = offset // 4, (offset + symbol_size) // 4
    if stop > len(words):
        raise OverlayDiffError(f"{obj.name} disassembles short of {name}")
    return words[start:stop], text[start:stop], masks[start:stop]


def run_objdump(root: Path, arguments: list[str]) -> str:
    completed = subprocess.run(
        [str(tool(root, "objdump")), *arguments],
        capture_output=True,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        raise OverlayDiffError(f"objdump failed: {completed.stderr.strip()}")
    return completed.stdout


def disassemble(root: Path, words: list[int]) -> list[str]:
    path = resolve_within(root, f"{BUILD_DIRECTORY}/target.bin")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(b"".join(struct.pack("<I", word) for word in words))
    listing = run_objdump(
        root, ["-D", "-b", "binary", "-m", "mips:3000", "-EL", str(path)]
    )
    return [
        match.group(2).replace("\t", " ").strip()
        for match in map(OBJDUMP_LINE.match, listing.splitlines())
        if match
    ]


def report(
    target: list[int],
    candidate: list[int],
    left: list[str],
    right: list[str],
    masks: list[int],
) -> bool:
    def masked(words: list[int]) -> list[int]:
        return [word & masks[i] if i < len(masks) else word for i, word in enumerate(words)]

    masked_target = masked(target)
    masked_candidate = masked(candidate)
    matched = masked_target == masked_candidate
    print(
        f"target {len(target)} instructions, candidate {len(candidate)}: "
        + ("MATCH" if matched else "DIFF")
    )
    if matched:
        return True
    for index in range(max(len(left), len(right))):
        a = left[index] if index < len(left) else ""
        b = right[index] if index < len(right) else ""
        same = (
            index < len(masked_target)
            and index < len(masked_candidate)
            and masked_target[index] == masked_candidate[index]
        )
        print(f"{'  ' if same else '>>'} {index:3d}  {a:<40}  {b}")
    return False


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Compare one compiled overlay function against the retail module "
            "bytes without building or linking the whole module."
        )
    )
    parser.add_argument("module", help="overlay module name such as main_menu")
    parser.add_argument("address", help="function address such as 0x80184344")
    parser.add_argument(
        "source",
        nargs="?",
        help="candidate C source; defaults to the configured matching source",
    )
    parser.add_argument(
        "--profile",
        default=DEFAULT_PROFILE,
        help=f"named compiler profile (default {DEFAULT_PROFILE})",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        address = int(args.address, 16)
        module = load_module(root, args.module)
        entry = inventory_entry(root, args.module, address)
        size = int(entry["size"], 16)
        source = args.source or configured_source(root, args.module, address)
        if source is None:
            raise OverlayDiffError(
                f"{address:#010x} has no configured source; pass one explicitly"
            )
        target = target_words(root, module, address, size)
        candidate, right, masks = candidate_words(
            root, source, args.profile, address, entry["name"], size
        )
        left = disassemble(root, target)
        return 0 if report(target, candidate, left, right, masks) else 1
    except (
        BuildError,
        OverlayDiffError,
        WorkspaceError,
        OSError,
        KeyError,
        TypeError,
        ValueError,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
