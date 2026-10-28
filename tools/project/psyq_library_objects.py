#!/usr/bin/env python3
"""Carry Psy-Q library function names from the North American inventory to a
regional one by matching the raw library objects.

`psyq_signatures.py` named the North American SDK functions from lab313ru's
signature catalogue. The regional inventories still carry hundreds of the
same library functions as `func_` placeholders, because their names were
applied only where game C needed one. The libraries themselves settle which
placeholder is which: every Psy-Q 4.6 `.LIB` member is an `LNK` object whose
patch records say exactly which words the linker rewrites. Masking those
words gives an exact pattern for the object's `.text`, independent of the
image it was linked into.

The tool takes the library directory as input and vendors nothing:

    tools/environments/python/bin/python tools/project/psyq_library_objects.py \
        --library-dir "../psyq46/LIB" --check
    tools/environments/python/bin/python tools/project/psyq_library_objects.py \
        --library-dir "../psyq46/LIB" --image slpm_86398 --report

A regional name is proposed only when all of these hold:

1. The object's masked `.text` matches the image exactly once, at a
   four-byte boundary.
2. The object's function label lands on a preserved `sdk_asm` start that is
   still spelled `func_`.
3. The same object and label offset also match exactly once in the North
   American image, on an inventory function whose name is not a placeholder.
   The regional row takes *that* name, so the local choices recorded in
   `psyq_signature_resolutions.json` carry over unchanged.
4. No other object proposes a different North American name for the same
   regional address, and the name is not already used elsewhere in the
   regional inventory.

`--check` is the control. It runs the same matcher on the North American
image, and every proposal there must equal the inventory name, apart from the
addresses whose resolution entry pins a different choice. A drop in that
agreement means the matcher broke.
"""

from __future__ import annotations

import argparse
import collections
import csv
import hashlib
import io
import json
import re
import struct
import sys
from dataclasses import dataclass, field
from pathlib import Path

from workspace import WorkspaceError, require_workspace_root, resolve_within


class LibraryError(RuntimeError):
    pass


IMAGES = {
    "slus_01411": "game/SLUS_014.11",
    "slpm_86398": "game/japanese/SLPM_863.98",
    "sles_03947": "game/europe/SLES_039.47",
    "sles_03948": "game/france/SLES_039.48",
    "sles_03949": "game/germany/SLES_039.49",
    "sles_03950": "game/italy/SLES_039.50",
    "sles_03951": "game/spain/SLES_039.51",
}
REFERENCE = "slus_01411"
RESOLUTIONS_JSON = "config/slus_01411/psyq_signature_resolutions.json"
HEADER_SIZE = 0x800
MINIMUM_CONCRETE_WORDS = 3
PLACEHOLDER = re.compile(r"^func_(?:[a-z]+_)?[0-9A-Fa-f]{8}(?:_[A-Za-z]+)?$")


# --- LNK object reader --------------------------------------------------------

class Reader:
    def __init__(self, data: bytes, offset: int = 0) -> None:
        self.data = data
        self.offset = offset

    def take(self, size: int) -> bytes:
        if self.offset + size > len(self.data):
            raise LibraryError(f"truncated record at {self.offset:#x}")
        value = self.data[self.offset:self.offset + size]
        self.offset += size
        return value

    def u8(self) -> int:
        return self.take(1)[0]

    def u16(self) -> int:
        return struct.unpack("<H", self.take(2))[0]

    def u32(self) -> int:
        return struct.unpack("<I", self.take(4))[0]

    def string(self) -> str:
        return self.take(self.u8()).decode("latin-1")


def skip_expression(reader: Reader) -> None:
    tag = reader.u8()
    if tag == 0x00:
        reader.u32()
    elif tag in (0x02, 0x04, 0x06, 0x08, 0x16):
        reader.u16()
    elif tag == 0x0C:
        reader.u16()
        reader.u32()
    elif 0x20 <= tag <= 0x3E:
        skip_expression(reader)
        skip_expression(reader)
    else:
        raise LibraryError(f"unknown patch expression {tag:#x} at {reader.offset - 1:#x}")


# Fixed-layout records that carry no information this tool needs, as the
# sequence of field kinds after the opcode: 1/2/4 are integers, "s" a string.
SKIPPED_RECORDS = {
    18: (2, 4, "s"),
    20: (2, 1, "s"),
    28: (2, "s"),
    46: (1,),
    50: (2, 4),
    52: (2, 4, "s"),
    56: (2,),
    58: (2, 1),
    60: (2,),
    62: (1,),
    64: (4,),
    74: (2, 4, 2, 4, 2, 4, 2, 4, 4, "s"),
    76: (2, 4, 4),
    78: (2, 4, 4),
    80: (2, 4, 4),
    82: (2, 4, 2, 2, 4, "s"),
    86: (2, 4, 2, 2, 4, "s"),
}


@dataclass
class LibraryObject:
    library: str
    member: str
    sections: dict[int, str] = field(default_factory=dict)
    text: dict[int, bytearray] = field(default_factory=dict)
    patched: dict[int, set[int]] = field(default_factory=dict)
    labels: list[tuple[str, int, int]] = field(default_factory=list)

    @property
    def identity(self) -> str:
        return f"{self.library}/{self.member}"


def parse_object(library: str, member: str, data: bytes) -> LibraryObject:
    reader = Reader(data)
    if reader.take(3) != b"LNK":
        raise LibraryError(f"{library}/{member}: not an LNK object")
    reader.u8()
    result = LibraryObject(library, member)
    section = None
    while True:
        opcode = reader.u8()
        if opcode == 0:
            return result
        if opcode == 2:
            size = reader.u16()
            result.text.setdefault(section, bytearray()).extend(reader.take(size))
        elif opcode == 6:
            section = reader.u16()
        elif opcode == 8:
            result.text.setdefault(section, bytearray()).extend(bytes(reader.u32()))
        elif opcode == 10:
            reader.u8()
            offset = reader.u16()
            skip_expression(reader)
            result.patched.setdefault(section, set()).add(offset // 4)
        elif opcode == 12:
            reader.u16()
            label_section = reader.u16()
            offset = reader.u32()
            result.labels.append((reader.string(), label_section, offset))
        elif opcode == 14:
            reader.u16()
            reader.string()
        elif opcode == 16:
            identifier = reader.u16()
            reader.u16()
            reader.u8()
            result.sections[identifier] = reader.string()
        elif opcode == 48:
            reader.u16()
            reader.u16()
            reader.u32()
            reader.string()
        elif opcode == 84:
            for kind in (2, 4, 2, 2, 4, "s"):
                reader.string() if kind == "s" else reader.take(kind)
            for _ in range(reader.u16()):
                reader.u32()
            reader.string()
        elif opcode in SKIPPED_RECORDS:
            for kind in SKIPPED_RECORDS[opcode]:
                reader.string() if kind == "s" else reader.take(kind)
        else:
            raise LibraryError(
                f"{library}/{member}: unknown record {opcode} at {reader.offset - 1:#x}"
            )


def parse_library(path: Path) -> list[LibraryObject]:
    data = path.read_bytes()
    reader = Reader(data)
    if reader.take(3) != b"LIB":
        raise LibraryError(f"{path.name}: not a Psy-Q LIB archive")
    reader.u8()
    objects = []
    while reader.offset < len(data):
        start = reader.offset
        member = reader.take(8).decode("latin-1").strip()
        reader.u32()
        header = reader.u32()
        size = reader.u32()
        if size <= header or start + size > len(data):
            raise LibraryError(f"{path.name}/{member}: bad member extent")
        objects.append(parse_object(path.name, member, data[start + header:start + size]))
        reader.offset = start + size
    return objects


def library_digest(paths: list[Path]) -> str:
    digest = hashlib.sha256()
    for path in paths:
        digest.update(path.name.encode())
        digest.update(hashlib.sha256(path.read_bytes()).digest())
    return digest.hexdigest()


# --- matching -----------------------------------------------------------------

def load_image(root: Path, image: str) -> tuple[int, tuple[int, ...]]:
    path = resolve_within(root, IMAGES[image], must_exist=True)
    data = path.read_bytes()
    if data[:8] != b"PS-X EXE":
        raise LibraryError(f"{IMAGES[image]}: not a PS-X EXE")
    load_address, payload_size = struct.unpack("<II", data[0x18:0x20])
    payload = data[HEADER_SIZE:HEADER_SIZE + payload_size]
    if len(payload) != payload_size:
        raise LibraryError(f"{IMAGES[image]}: truncated payload")
    return load_address, struct.unpack(f"<{payload_size // 4}I", payload)


def load_inventory(root: Path, image: str) -> list[dict[str, str]]:
    path = resolve_within(root, f"config/{image}/functions.csv", must_exist=True)
    with path.open(encoding="utf-8", newline="") as handle:
        return list(csv.DictReader(handle))


def match_labels(
    objects: list[LibraryObject], load_address: int, words: tuple[int, ...]
) -> dict[int, set[tuple[str, int, str]]]:
    """Address -> {(object identity, label offset, label)} for unique matches."""
    index: dict[int, list[int]] = collections.defaultdict(list)
    for position, word in enumerate(words):
        index[word].append(position)
    found: dict[int, set[tuple[str, int, str]]] = collections.defaultdict(set)
    for library_object in objects:
        for section, name in library_object.sections.items():
            if name != ".text":
                continue
            text = bytes(library_object.text.get(section, b""))
            count = len(text) // 4
            pattern = struct.unpack(f"<{count}I", text[:count * 4])
            patched = library_object.patched.get(section, set())
            concrete = [k for k in range(count) if k not in patched and pattern[k] != 0]
            if len(concrete) < MINIMUM_CONCRETE_WORDS:
                continue
            anchor = min(concrete, key=lambda k: len(index.get(pattern[k], ())))
            hits = []
            for position in index.get(pattern[anchor], ()):
                start = position - anchor
                if start < 0 or start + count > len(words):
                    continue
                if all(k in patched or words[start + k] == pattern[k] for k in range(count)):
                    hits.append(start)
                    if len(hits) > 1:
                        break
            if len(hits) != 1:
                continue
            base = load_address + hits[0] * 4
            for label, label_section, offset in library_object.labels:
                if label_section == section:
                    found[base + offset].add((library_object.identity, offset, label))
    return found


def pinned_resolutions(root: Path) -> dict[int, str]:
    path = resolve_within(root, RESOLUTIONS_JSON, must_exist=True)
    document = json.loads(path.read_text(encoding="utf-8"))
    pinned = {}
    for catalogue in document["catalogues"].values():
        for entry in catalogue["resolutions"]:
            pinned[int(entry["address"], 16)] = entry["selected_name"]
    return pinned


def control(root: Path, objects: list[LibraryObject]) -> dict:
    load_address, words = load_image(root, REFERENCE)
    inventory = {int(row["address"], 16): row for row in load_inventory(root, REFERENCE)}
    found = match_labels(objects, load_address, words)
    pinned = pinned_resolutions(root)
    agree, pinned_choice, disagree = 0, [], []
    names: dict[tuple[str, int], str] = {}
    for address, hits in sorted(found.items()):
        row = inventory.get(address)
        if row is None or row["status"] != "sdk_asm":
            continue
        labels = {label for _, _, label in hits}
        if row["name"] in labels:
            agree += 1
        elif address in pinned and pinned[address] == row["name"]:
            pinned_choice.append((address, row["name"], sorted(labels)))
        elif not PLACEHOLDER.match(row["name"]):
            disagree.append((address, row["name"], sorted(labels)))
        if not PLACEHOLDER.match(row["name"]):
            for identity, offset, _ in hits:
                names[(identity, offset)] = row["name"]
    return {"agree": agree, "pinned": pinned_choice, "disagree": disagree, "names": names}


def require_reference_evidence(reference: dict) -> None:
    """Reject a control that confirmed no North American name.

    Zero agreements means no library object matched a named ``sdk_asm``
    function (an empty or unrelated library directory), so the control
    verified nothing and must not pass as a clean result. Zero *proposals*
    after a control with agreements is a different, legitimate outcome.
    """
    if reference["agree"] == 0:
        raise LibraryError(
            f"{REFERENCE} control is inconclusive: no library object matched a "
            "named sdk_asm function; check --library-dir")


def propose(root: Path, objects: list[LibraryObject], image: str) -> dict:
    reference = control(root, objects)
    load_address, words = load_image(root, image)
    rows = load_inventory(root, image)
    by_address = {int(row["address"], 16): row for row in rows}
    used = collections.Counter(row["name"] for row in rows)
    found = match_labels(objects, load_address, words)
    proposals, skipped = {}, collections.Counter()
    for address, hits in sorted(found.items()):
        row = by_address.get(address)
        if row is None:
            skipped["not a function start"] += 1
            continue
        if row["status"] != "sdk_asm":
            skipped[f"status {row['status']}"] += 1
            continue
        candidates = {reference["names"].get((identity, offset)) for identity, offset, _ in hits}
        candidates.discard(None)
        if not PLACEHOLDER.match(row["name"]):
            skipped["already named"] += 1
            continue
        if not candidates:
            skipped["no North American name"] += 1
            continue
        if len(candidates) > 1:
            skipped["conflicting North American names"] += 1
            continue
        name = candidates.pop()
        if used[name]:
            skipped["name already used in this inventory"] += 1
            continue
        proposals[address] = (row["name"], name, sorted({identity for identity, _, _ in hits}))
    names = collections.Counter(name for _, name, _ in proposals.values())
    for address in [a for a, (_, name, _) in proposals.items() if names[name] > 1]:
        del proposals[address]
        skipped["name proposed twice"] += 1
    return {"reference": reference, "proposals": proposals, "skipped": skipped}


def apply(root: Path, image: str, proposals: dict) -> int:
    path = resolve_within(root, f"config/{image}/functions.csv", must_exist=True)
    text = path.read_text(encoding="utf-8")
    handle = io.StringIO(text, newline="")
    reader = csv.DictReader(handle)
    fieldnames = reader.fieldnames
    rows = list(reader)
    changed = 0
    for row in rows:
        proposal = proposals.get(int(row["address"], 16))
        if proposal and row["name"] == proposal[0]:
            row["name"] = proposal[1]
            changed += 1
    output = io.StringIO(newline="")
    writer = csv.DictWriter(output, fieldnames=fieldnames, lineterminator="\n")
    writer.writeheader()
    writer.writerows(rows)
    path.write_text(output.getvalue(), encoding="utf-8")
    return changed


def load_objects(library_dir: Path) -> tuple[list[LibraryObject], str]:
    paths = sorted(library_dir.glob("*.LIB"))
    if not paths:
        raise LibraryError(f"{library_dir}: no .LIB files")
    objects = []
    for path in paths:
        objects.extend(parse_library(path))
    return objects, library_digest(paths)


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--library-dir", required=True,
                        help="directory holding the Psy-Q 4.6 .LIB archives")
    parser.add_argument("--image", choices=sorted(set(IMAGES) - {REFERENCE}))
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--check", action="store_true",
                      help="reproduce the North American inventory names")
    mode.add_argument("--report", action="store_true")
    mode.add_argument("--apply", action="store_true",
                      help="rename the proposed rows in the image's functions.csv")
    args = parser.parse_args(argv)
    if (args.report or args.apply) and not args.image:
        parser.error("--report and --apply need --image")
    return args


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    try:
        root = require_workspace_root()
        library_dir = Path(args.library_dir).expanduser()
        if not library_dir.is_dir():
            raise LibraryError(f"not a directory: {library_dir}")
        objects, digest = load_objects(library_dir)
        print(f"libraries: {len(objects)} objects, sha256 {digest}")
        if args.check:
            reference = control(root, objects)
            print(f"{REFERENCE}: {reference['agree']} agree, "
                  f"{len(reference['pinned'])} pinned by {RESOLUTIONS_JSON}, "
                  f"{len(reference['disagree'])} disagree")
            for address, name, labels in reference["pinned"] + reference["disagree"]:
                print(f"  0x{address:08X} {name} <- {'|'.join(labels)}")
            require_reference_evidence(reference)
            return 1 if reference["disagree"] else 0
        result = propose(root, objects, args.image)
        require_reference_evidence(result["reference"])
        if result["reference"]["disagree"]:
            raise LibraryError("North American control disagrees; run --check")
        for address, (old, new, identities) in sorted(result["proposals"].items()):
            print(f"0x{address:08X} {old} -> {new}  {' '.join(identities)}")
        print(f"{args.image}: {len(result['proposals'])} proposals; skipped "
              + ", ".join(f"{k} {v}" for k, v in sorted(result["skipped"].items())))
        if args.apply:
            print(f"renamed {apply(root, args.image, result['proposals'])} rows")
        return 0
    except (LibraryError, WorkspaceError, OSError, ValueError, KeyError,
            json.JSONDecodeError, csv.Error) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
