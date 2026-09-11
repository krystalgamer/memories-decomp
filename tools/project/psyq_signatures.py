#!/usr/bin/env python3
"""Locate Psy-Q library functions in the retail image by byte signature.

Six hundred functions in `SLUS_014.11` are Sony library code rather than game
code. Naming them is worth more than the count suggests: every one that keeps
its `func_XXXXXXXX` spelling is a call site in game C that reads as an address
instead of as `RotMatrix` or `GsSortSprite`, and a reader has no way to tell a
library call from an undecompiled game routine.

They were being named one at a time, by hand, from the manual and from call
sites. That is slow and it is also the wrong order: the manual tells you what a
name means, not that this executable contains that object. lab313ru's
`psx_psyq_signatures` catalogue does the second job directly - it carries, per
library object, the object's own bytes with relocated fields wildcarded, plus
the labels inside it and their offsets. An object that appears exactly once in
the payload is the strongest evidence tier `notes/psyq.md` recognises.

The catalogue is a third-party download and is not vendored here, so this tool
takes a path to it rather than fetching anything:

    tools/environments/python/bin/python tools/project/psyq_signatures.py \
        --signatures ../psx_psyq_signatures/460 --report

Five rules decide whether a label becomes a proposal, and they exist because
each of them was violated by something in the corpus:

1. The full masked pattern must match, and at a four-byte boundary. Anchoring
   on the longest concrete run and then verifying the whole mask enumerates
   every match, so uniqueness below is uniqueness over the whole payload.
2. The object must match exactly once. Some objects match nowhere, which is
   the normal case for a library that was not linked in.
3. The label must not be an IDA placeholder (`text_1F0`, `loc_24`). Roughly
   half of all labels are placeholders and carry no information; adopting them
   would replace one meaningless name with another.
4. The address must be a function start in the inventory. A label in the
   middle of a function is a branch target, not a symbol.
5. No other library object may propose a *different* name for the same
   address. Small routines are duplicated verbatim across libraries -
   `0x8007A840` is claimed by six objects across LIBCD and LIBDS - so a
   multi-claimed address is genuinely undecidable from bytes alone and needs a
   call-graph tiebreak instead.

The report also states how many proposals agree with names the inventory
already carries. That number is the reason to trust the rest: it is the tool
checking itself against work done independently and by hand, and a drop in it
means the matcher broke, not that the corpus is wrong.
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import struct
import sys
from pathlib import Path

from workspace import WorkspaceError, require_workspace_root, resolve_within


class SignatureError(RuntimeError):
    pass


EXECUTABLE = "game/SLUS_014.11"
FUNCTIONS_CSV = "config/slus_01411/functions.csv"
HEADER_SIZE = 0x800

# `text_1F0`, `loc_24`, `dword_800E9D90` and friends: IDA's own naming for
# something it could not identify, which is exactly what we are trying to fix.
PLACEHOLDER = re.compile(
    r"^(?:text|data|rdata|bss|sbss|sdata|loc|locret|sub|unk|byte|word|dword"
    r"|qword|off|asc|stru|flt|dbl|jpt|def)_[0-9A-Fa-f]+$"
)
MINIMUM_ANCHOR = 4
SIGNATURE_BYTE = re.compile(r"^[0-9A-Fa-f]{2}$")


def load_payload(root: Path) -> tuple[int, bytes]:
    path = resolve_within(root, EXECUTABLE, must_exist=True)
    image = path.read_bytes()
    if image[:8] != b"PS-X EXE":
        raise SignatureError(f"{EXECUTABLE}: not a PS-X EXE")
    _pc, _gp, load_address, payload_size = struct.unpack("<IIII", image[0x10:0x20])
    payload = image[HEADER_SIZE:HEADER_SIZE + payload_size]
    if len(payload) != payload_size:
        raise SignatureError(f"{EXECUTABLE}: truncated payload")
    return load_address, payload


def load_inventory(root: Path) -> dict[int, dict[str, str]]:
    path = resolve_within(root, FUNCTIONS_CSV, must_exist=True)
    with path.open(encoding="utf-8", newline="") as handle:
        return {int(row["address"], 16): row for row in csv.DictReader(handle)}


def parse_signature(text: str) -> tuple[bytes, bytes]:
    pattern = bytearray()
    mask = bytearray()
    tokens = text.split()
    if not tokens:
        raise SignatureError("signature is empty")
    for index, token in enumerate(tokens):
        if token == "??":
            pattern.append(0)
            mask.append(0)
        else:
            if not SIGNATURE_BYTE.fullmatch(token):
                raise SignatureError(
                    f"token {index} {token!r} is not ?? or two hexadecimal digits"
                )
            pattern.append(int(token, 16))
            mask.append(0xFF)
    return bytes(pattern), bytes(mask)


def longest_concrete_run(mask: bytes) -> tuple[int, int]:
    best = (0, 0)
    index = 0
    while index < len(mask):
        if mask[index] != 0xFF:
            index += 1
            continue
        end = index
        while end < len(mask) and mask[end] == 0xFF:
            end += 1
        if end - index > best[1] - best[0]:
            best = (index, end)
        index = end
    return best


def find_matches(payload: bytes, pattern: bytes, mask: bytes) -> list[int] | None:
    """Every offset where the masked pattern matches, or None if unanchorable."""
    start, end = longest_concrete_run(mask)
    if end - start < MINIMUM_ANCHOR:
        return None
    anchor = pattern[start:end]
    matches = []
    cursor = 0
    while True:
        found = payload.find(anchor, cursor)
        if found < 0:
            break
        cursor = found + 1
        offset = found - start
        if offset < 0 or offset % 4 or offset + len(pattern) > len(payload):
            continue
        window = payload[offset:offset + len(pattern)]
        if all(
            not mask[index] or window[index] == pattern[index]
            for index in range(len(pattern))
        ):
            matches.append(offset)
    return matches


def signature_entry_fields(
    path: Path, index: int, entry: dict
) -> tuple[str, str, list[tuple[str, int]]]:
    entry_name = entry.get("name")
    if not isinstance(entry_name, str):
        raise SignatureError(
            f"{path}: entry {index} name is not a string"
        )
    if not entry_name.strip():
        raise SignatureError(f"{path}: entry {index} name is empty")
    signature = entry["sig"]
    if not isinstance(signature, str):
        raise SignatureError(
            f"{path}: entry {index} sig is not a string"
        )
    labels = entry.get("labels", [])
    if not isinstance(labels, list):
        raise SignatureError(
            f"{path}: entry {index} labels is not an array"
        )

    validated_labels: list[tuple[str, int]] = []
    for label_index, label in enumerate(labels):
        if not isinstance(label, dict):
            raise SignatureError(
                f"{path}: entry {index} label {label_index} is not an object"
            )
        name = label.get("name")
        if not isinstance(name, str):
            raise SignatureError(
                f"{path}: entry {index} label {label_index} "
                "name is not a string"
            )
        if not name.strip():
            raise SignatureError(
                f"{path}: entry {index} label {label_index} name is empty"
            )
        offset = label.get("offset")
        if type(offset) is not int:
            raise SignatureError(
                f"{path}: entry {index} label {label_index} "
                "offset is not an integer"
            )
        validated_labels.append((name, offset))
    return entry_name, signature, validated_labels


def scan(signatures: Path, load_address: int, payload: bytes) -> dict:
    """address -> {name: [providers]}, plus counts for the report."""
    proposals: dict[int, dict[str, list[str]]] = {}
    unique = multiple = absent = unanchored = 0
    paths = sorted(signatures.glob("*.json"))
    if not paths:
        raise SignatureError(f"{signatures}: no JSON signature files")

    signature_entries = 0
    for path in paths:
        library = path.name[: -len(".json")]
        try:
            entries = json.loads(path.read_text(encoding="utf-8"))
        except json.JSONDecodeError as error:
            raise SignatureError(
                f"{path}: invalid JSON at line {error.lineno}, "
                f"column {error.colno}: {error.msg}"
            ) from error
        if not isinstance(entries, list):
            raise SignatureError(f"{path}: expected a JSON array")
        for index, entry in enumerate(entries):
            if not isinstance(entry, dict):
                raise SignatureError(
                    f"{path}: entry {index} is not a JSON object"
                )
            if "sig" not in entry:
                continue
            signature_entries += 1
            entry_name, signature, labels = signature_entry_fields(
                path, index, entry
            )
            try:
                pattern, mask = parse_signature(signature)
            except SignatureError as error:
                raise SignatureError(
                    f"{path}: entry {index} sig: {error}"
                ) from error
            for label_index, (_name, offset) in enumerate(labels):
                if offset < 0 or offset >= len(pattern):
                    raise SignatureError(
                        f"{path}: entry {index} label {label_index} "
                        f"offset {offset} is outside the "
                        f"{len(pattern)}-byte signature"
                    )
            matches = find_matches(payload, pattern, mask)
            if matches is None:
                unanchored += 1
                continue
            if not matches:
                absent += 1
                continue
            if len(matches) > 1:
                multiple += 1
                continue
            unique += 1
            for name, offset in labels:
                if PLACEHOLDER.match(name):
                    continue
                provider = f"{library}/{entry_name}+{offset:#x}"
                address = load_address + matches[0] + offset
                proposals.setdefault(address, {}).setdefault(name, []).append(provider)
    if signature_entries == 0:
        raise SignatureError(f"{signatures}: no signature entries")
    return {
        "proposals": proposals,
        "unique": unique,
        "multiple": multiple,
        "absent": absent,
        "unanchored": unanchored,
    }


def classify(proposals: dict, inventory: dict) -> dict:
    ambiguous, agreed, disagreed, new, off_start = [], [], [], [], 0
    for address in sorted(proposals):
        names = proposals[address]
        if len(names) > 1:
            ambiguous.append((address, sorted(names)))
            continue
        name = next(iter(names))
        row = inventory.get(address)
        if row is None:
            off_start += 1
            continue
        if row["name"] == name:
            agreed.append((address, name))
        elif row["name"].startswith("func_"):
            new.append((address, name, row, sorted(names[name])))
        else:
            disagreed.append((address, name, row["name"], sorted(names[name])))
    return {
        "agreed": agreed,
        "disagreed": disagreed,
        "new": new,
        "ambiguous": ambiguous,
        "off_start": off_start,
    }


def evidence(providers: list[str]) -> str:
    origin, _, offset = providers[0].rpartition("+")
    return (
        f"Unique exact Psy-Q 4.6 {origin} signature, label at object offset "
        f"{offset}; the object matches the payload once"
    )


def emit_map(result: dict) -> None:
    writer = csv.writer(sys.stdout, lineterminator="\n")
    for address, name, row, providers in result["new"]:
        writer.writerow(
            [
                "function",
                f"0x{address:08X}",
                name,
                "confirmed",
                evidence(providers),
                "",
            ]
        )


def report(scanned: dict, result: dict) -> None:
    print(f"objects matched once     : {scanned['unique']}")
    print(f"objects matched several  : {scanned['multiple']}")
    print(f"objects absent           : {scanned['absent']}")
    print(f"objects without anchor   : {scanned['unanchored']}")
    print()
    print(f"names already in the inventory, agreeing : {len(result['agreed'])}")
    print(f"names already in the inventory, differing: {len(result['disagreed'])}")
    print(f"new names for func_XXXXXXXX rows         : {len(result['new'])}")
    print(f"addresses claimed under several names    : {len(result['ambiguous'])}")
    print(f"labels away from a function start        : {result['off_start']}")
    if result["disagreed"]:
        print()
        print("differing:")
        for address, name, current, providers in result["disagreed"]:
            print(f"  {address:#010x} corpus {name} / inventory {current} [{providers[0]}]")
    if result["ambiguous"]:
        print()
        print("ambiguous, left alone:")
        for address, names in result["ambiguous"]:
            print(f"  {address:#010x} {', '.join(names)}")
    if result["new"]:
        print()
        print("new:")
        for address, name, row, providers in result["new"]:
            print(f"  {address:#010x} {row['size']:>7} {name:<28} {providers[0]}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Locate Psy-Q library functions in the retail image."
    )
    parser.add_argument(
        "--signatures",
        required=True,
        help="directory of lab313ru psx_psyq_signatures JSON files (460)",
    )
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--report", action="store_true")
    mode.add_argument(
        "--emit-map",
        action="store_true",
        help="write semantic-symbol-map.csv rows for the new names to stdout",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        signatures = Path(args.signatures).expanduser()
        if not signatures.is_dir():
            raise SignatureError(f"not a directory: {signatures}")
        load_address, payload = load_payload(root)
        inventory = load_inventory(root)
        scanned = scan(signatures, load_address, payload)
        result = classify(scanned["proposals"], inventory)
        if args.emit_map:
            emit_map(result)
        else:
            report(scanned, result)
        return 0
    except (SignatureError, WorkspaceError, OSError, ValueError, KeyError,
            json.JSONDecodeError, csv.Error) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
