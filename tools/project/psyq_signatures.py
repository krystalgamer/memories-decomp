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
4. The address must be a preserved Psy-Q function start in the inventory. A
   label in the middle of a function is a branch target, and a label on a
   game-owned function is a byte collision rather than an SDK identity.
5. No other library object may propose a *different* name for the same
   address without an explicit local resolution. Small routines are duplicated
   verbatim across libraries - `0x8007A840` is claimed by six objects across
   LIBCD and LIBDS - so `psyq_signature_resolutions.json` pins the complete
   proposal set, chosen inventory name, and whether the choice is supported by
   call-graph/data-flow evidence or retained as repository naming policy.

The report also states how many proposals agree with names the inventory
already carries. These proposal totals are computed only after objects that
match multiple payload locations have been discarded and labels have been
restricted to preserved function starts; they are not whole-inventory naming
coverage. That number is the reason to trust the rest: it is the tool
checking itself against work done independently and by hand, and a drop in it
means the matcher broke, not that the corpus is wrong.

The resolution file also pins a digest of every JSON file in the selected
catalogue. Reports therefore describe the reviewed catalogue revision rather
than whatever files happen to occupy the supplied directory.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
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
RESOLUTIONS_JSON = "config/slus_01411/psyq_signature_resolutions.json"
HEADER_SIZE = 0x800
ROOT = Path(__file__).resolve().parents[2]

# `text_1F0`, `loc_24`, `dword_800E9D90` and friends: IDA's own naming for
# something it could not identify, which is exactly what we are trying to fix.
PLACEHOLDER = re.compile(
    r"^(?:text|data|rdata|bss|sbss|sdata|loc|locret|sub|unk|byte|word|dword"
    r"|qword|off|asc|stru|flt|dbl|jpt|def)_[0-9A-Fa-f]+$"
)
MINIMUM_ANCHOR = 4
SIGNATURE_BYTE = re.compile(r"^[0-9A-Fa-f]{2}$")
SHA256 = re.compile(r"^[0-9a-f]{64}$")


def catalogue_sha256(signatures: Path) -> str:
    digest = hashlib.sha256(b"psyq-signature-catalogue-v1\0")
    paths = sorted(signatures.glob("*.json"))
    for path in paths:
        name = path.name.encode("utf-8")
        data = path.read_bytes()
        digest.update(len(name).to_bytes(4, "big"))
        digest.update(name)
        digest.update(len(data).to_bytes(8, "big"))
        digest.update(data)
    return digest.hexdigest()


def load_resolution_entries(
    root: Path,
    psyq_version: str,
) -> tuple[str, dict[int, dict[str, object]]]:
    path = resolve_within(root, RESOLUTIONS_JSON, must_exist=True)
    document = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(document, dict):
        raise SignatureError(f"{RESOLUTIONS_JSON}: document is not an object")
    if document.get("schema") != 1:
        raise SignatureError(f"{RESOLUTIONS_JSON}: unsupported schema")
    catalogues = document.get("catalogues")
    if not isinstance(catalogues, dict):
        raise SignatureError(f"{RESOLUTIONS_JSON}: catalogues is not an object")
    catalogue = catalogues.get(psyq_version)
    if not isinstance(catalogue, dict):
        raise SignatureError(
            f"{RESOLUTIONS_JSON}: missing Psy-Q {psyq_version} catalogue"
        )
    expected_hash = catalogue.get("sha256")
    if not isinstance(expected_hash, str) or not SHA256.fullmatch(expected_hash):
        raise SignatureError(
            f"{RESOLUTIONS_JSON}: Psy-Q {psyq_version} has invalid sha256"
        )
    entries = catalogue.get("resolutions")
    if not isinstance(entries, list):
        raise SignatureError(
            f"{RESOLUTIONS_JSON}: Psy-Q {psyq_version} resolutions "
            "is not an array"
        )
    resolutions: dict[int, dict[str, object]] = {}
    for index, entry in enumerate(entries):
        context = (
            f"{RESOLUTIONS_JSON}: Psy-Q {psyq_version} resolution {index}"
        )
        if not isinstance(entry, dict):
            raise SignatureError(f"{context} is not an object")
        try:
            address = int(entry["address"], 0)
        except (KeyError, TypeError, ValueError) as error:
            raise SignatureError(f"{context} has invalid address") from error
        names = entry.get("catalogue_names")
        if (
            not isinstance(names, list)
            or not names
            or any(not isinstance(name, str) or not name for name in names)
            or names != sorted(set(names))
        ):
            raise SignatureError(
                f"{context} catalogue_names must be sorted unique names"
            )
        selected_name = entry.get("selected_name")
        basis = entry.get("basis")
        evidence_text = entry.get("evidence")
        if not isinstance(selected_name, str) or not selected_name:
            raise SignatureError(f"{context} has invalid selected_name")
        if not isinstance(basis, str) or basis not in {
            "evidence",
            "naming_policy",
        }:
            raise SignatureError(
                f"{context} basis must be evidence or naming_policy"
            )
        if not isinstance(evidence_text, str) or not evidence_text.strip():
            raise SignatureError(f"{context} has invalid evidence")
        if address in resolutions:
            raise SignatureError(
                f"{context} duplicates address {address:#010x}"
            )
        resolutions[address] = {
            "catalogue_names": names,
            "selected_name": selected_name,
            "basis": basis,
            "evidence": evidence_text,
        }
    return expected_hash, resolutions


def load_resolutions(
    root: Path,
    signatures: Path,
    psyq_version: str,
) -> dict[int, dict[str, object]]:
    expected_hash, resolutions = load_resolution_entries(root, psyq_version)
    actual_hash = catalogue_sha256(signatures)
    if actual_hash != expected_hash:
        raise SignatureError(
            f"{signatures}: catalogue hash differs: {actual_hash}"
        )
    return resolutions


def check_resolution_inventory(root: Path) -> int:
    inventory = load_inventory(root)
    count = 0
    selected: dict[int, str] = {}
    for psyq_version in ("4.6", "4.7"):
        _catalogue_hash, resolutions = load_resolution_entries(
            root, psyq_version
        )
        for address, resolution in resolutions.items():
            row = inventory.get(address)
            if row is None:
                raise SignatureError(
                    f"{address:#010x}: Psy-Q {psyq_version} resolution "
                    "is not a function start"
                )
            if (
                row.get("status") != "sdk_asm"
                or not row.get("module", "").startswith("psyq/")
            ):
                raise SignatureError(
                    f"{address:#010x}: Psy-Q {psyq_version} resolution "
                    "is not an SDK function"
                )
            selected_name = resolution["selected_name"]
            if row["name"] != selected_name:
                raise SignatureError(
                    f"{address:#010x}: Psy-Q {psyq_version} resolution "
                    f"selects {selected_name} but inventory uses {row['name']}"
                )
            previous = selected.setdefault(address, selected_name)
            if previous != selected_name:
                raise SignatureError(
                    f"{address:#010x}: catalogue versions select "
                    f"different names: {previous}, {selected_name}"
                )
            count += 1
    return count


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


def validate_catalogue_scope(signatures: Path, psyq_version: str) -> None:
    if psyq_version != "4.7":
        return
    paths = sorted(path.name for path in signatures.glob("*.json"))
    if paths != ["LIBDS.LIB.json"]:
        raise SignatureError(
            f"{signatures}: Psy-Q 4.7 is permitted only for a directory "
            "containing exactly LIBDS.LIB.json"
        )


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
    objects = []
    unique = multiple = absent = unanchored = 0
    paths = sorted(signatures.glob("*.json"))
    if not paths:
        raise SignatureError(f"{signatures}: no JSON signature files")

    signature_entries = 0
    for path in paths:
        library = path.name[: -len(".json")]
        try:
            entries = json.loads(path.read_text(encoding="utf-8"))
        except UnicodeDecodeError as error:
            raise SignatureError(
                f"{path}: invalid UTF-8 at byte {error.start}: "
                f"{error.reason}"
            ) from error
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
            object_start = load_address + matches[0]
            objects.append(
                {
                    "provider": f"{library}/{entry_name}",
                    "start": object_start,
                    "end": object_start + len(pattern),
                }
            )
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
        "objects": objects,
    }


def classify(
    proposals: dict,
    inventory: dict,
    objects: list[dict] | None = None,
    resolutions: dict[int, dict[str, object]] | None = None,
) -> dict:
    ambiguous, agreed, disagreed, new = [], [], [], []
    ambiguous_locally_named, ambiguous_unresolved = [], []
    resolved = []
    outside_psyq, off_start = [], 0
    unused_resolutions = set(resolutions or {})
    for address in sorted(proposals):
        names = proposals[address]
        resolution = (resolutions or {}).get(address)
        row = inventory.get(address)
        if row is None:
            if resolution is not None:
                raise SignatureError(
                    f"{address:#010x}: resolution is not a function start"
                )
            off_start += 1
            continue
        if (
            row.get("status") != "sdk_asm"
            or not row.get("module", "").startswith("psyq/")
        ):
            if resolution is not None:
                raise SignatureError(
                    f"{address:#010x}: resolution is not a Psy-Q function"
                )
            outside_psyq.append(
                (
                    address,
                    sorted(names),
                    row["name"],
                    row.get("status", ""),
                    row.get("module", ""),
                    sorted(
                        provider
                        for providers in names.values()
                        for provider in providers
                    ),
                )
            )
            continue
        if resolution is not None:
            catalogue_names = sorted(names)
            if resolution["catalogue_names"] != catalogue_names:
                raise SignatureError(
                    f"{address:#010x}: resolution catalogue names differ: "
                    f"{catalogue_names}"
                )
            if resolution["selected_name"] != row["name"]:
                raise SignatureError(
                    f"{address:#010x}: resolution selects "
                    f"{resolution['selected_name']} but inventory uses "
                    f"{row['name']}"
                )
            unused_resolutions.remove(address)
            resolved.append(
                (
                    address,
                    row["name"],
                    catalogue_names,
                    resolution["basis"],
                    resolution["evidence"],
                )
            )
            continue
        if len(names) > 1:
            sorted_names = sorted(names)
            ambiguous.append((address, sorted_names))
            if not row.get("name", "").startswith("func_"):
                ambiguous_locally_named.append(
                    (address, sorted_names, row["name"])
                )
            else:
                ambiguous_unresolved.append(
                    (address, sorted_names, row.get("name"))
                )
            continue
        name = next(iter(names))
        if row["name"] == name:
            agreed.append((address, name))
        elif row["name"].startswith("func_"):
            new.append((address, name, row, sorted(names[name])))
        else:
            disagreed.append((address, name, row["name"], sorted(names[name])))
    address_named_inventory = [
        (address, row)
        for address, row in sorted(inventory.items())
        if (
            row.get("status") == "sdk_asm"
            and row.get("module", "").startswith("psyq/")
            and row.get("name", "").startswith("func_")
        )
    ]
    object_covered_inventory = []
    object_uncovered_inventory = []
    for address, row in address_named_inventory:
        providers = sorted(
            item["provider"]
            for item in objects or []
            if item["start"] <= address < item["end"]
        )
        if providers:
            object_covered_inventory.append((address, row, providers))
        else:
            object_uncovered_inventory.append((address, row))
    if unused_resolutions:
        addresses = ", ".join(
            f"{address:#010x}" for address in sorted(unused_resolutions)
        )
        raise SignatureError(f"resolutions have no catalogue proposal: {addresses}")
    return {
        "agreed": agreed,
        "disagreed": disagreed,
        "new": new,
        "resolved": resolved,
        "ambiguous": ambiguous,
        "ambiguous_locally_named": ambiguous_locally_named,
        "ambiguous_unresolved": ambiguous_unresolved,
        "outside_psyq": outside_psyq,
        "off_start": off_start,
        "address_named_inventory": address_named_inventory,
        "object_covered_inventory": object_covered_inventory,
        "object_uncovered_inventory": object_uncovered_inventory,
    }


def evidence(providers: list[str], psyq_version: str = "4.6") -> str:
    origin, _, offset = providers[0].rpartition("+")
    return (
        f"Unique exact Psy-Q {psyq_version} {origin} signature, label at "
        f"object offset {offset}; the object matches the payload once"
    )


def emit_map(result: dict, psyq_version: str = "4.6") -> None:
    writer = csv.writer(sys.stdout, lineterminator="\n")
    for address, name, row, providers in result["new"]:
        writer.writerow(
            [
                "function",
                f"0x{address:08X}",
                name,
                "confirmed",
                evidence(providers, psyq_version),
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
    evidence_count = sum(
        basis == "evidence"
        for _address, _selected, _names, basis, _text in result["resolved"]
    )
    naming_count = sum(
        basis == "naming_policy"
        for _address, _selected, _names, basis, _text in result["resolved"]
    )
    print(f"catalogue conflicts resolved by evidence : {evidence_count}")
    print(f"catalogue names retained by policy       : {naming_count}")
    print(f"addresses claimed under several names    : {len(result['ambiguous'])}")
    print(
        "ambiguous addresses with local inventory names: "
        f"{len(result['ambiguous_locally_named'])}"
    )
    print(
        "ambiguous addresses still unresolved     : "
        f"{len(result['ambiguous_unresolved'])}"
    )
    print(
        "Psy-Q inventory rows still address-named : "
        f"{len(result['address_named_inventory'])}"
    )
    print(
        "  within unique matched library objects  : "
        f"{len(result['object_covered_inventory'])}"
    )
    print(
        "  outside unique matched library objects : "
        f"{len(result['object_uncovered_inventory'])}"
    )
    print(f"labels on non-Psy-Q function starts      : {len(result['outside_psyq'])}")
    print(f"labels away from a function start        : {result['off_start']}")
    if result["disagreed"]:
        print()
        print("differing:")
        for address, name, current, providers in result["disagreed"]:
            print(f"  {address:#010x} corpus {name} / inventory {current} [{providers[0]}]")
    if result["resolved"]:
        print()
        print("resolved catalogue conflicts:")
        for address, selected, names, basis, evidence_text in result["resolved"]:
            print(
                f"  {address:#010x} {selected} over {', '.join(names)}: "
                f"[{basis}] {evidence_text}"
            )
    if result["ambiguous_locally_named"]:
        print()
        print("ambiguous signatures with local inventory names, left unchanged:")
        for address, names, current in result["ambiguous_locally_named"]:
            print(
                f"  {address:#010x} inventory {current} / "
                f"corpus {', '.join(names)}"
            )
    if result["ambiguous_unresolved"]:
        print()
        print("ambiguous and still unresolved:")
        for address, names, current in result["ambiguous_unresolved"]:
            current_text = "no function start" if current is None else current
            print(
                f"  {address:#010x} inventory {current_text} / "
                f"corpus {', '.join(names)}"
            )
    if result["outside_psyq"]:
        print()
        print("non-Psy-Q function starts, left alone:")
        for item in result["outside_psyq"]:
            address, names, current, status, module, providers = item
            print(
                f"  {address:#010x} corpus {', '.join(names)} / "
                f"inventory {current} "
                f"({status}, {module}) [{providers[0]}]"
            )
    if result["new"]:
        print()
        print("new:")
        for address, name, row, providers in result["new"]:
            print(f"  {address:#010x} {row['size']:>7} {name:<28} {providers[0]}")


def report_coverage(result: dict) -> None:
    writer = csv.writer(sys.stdout, lineterminator="\n")
    writer.writerow(["address", "size", "name", "unique_signature_objects"])
    for address, row, providers in result["object_covered_inventory"]:
        writer.writerow(
            [
                f"0x{address:08X}",
                row.get("size", ""),
                row.get("name", ""),
                ";".join(providers),
            ]
        )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Locate Psy-Q library functions in the retail image."
    )
    parser.add_argument(
        "--signatures",
        help="directory of lab313ru psx_psyq_signatures JSON files (460)",
    )
    parser.add_argument(
        "--psyq-version",
        choices=("4.6", "4.7"),
        default="4.6",
        help="catalogue version written into --emit-map evidence",
    )
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--report", action="store_true")
    mode.add_argument(
        "--coverage-report",
        action="store_true",
        help=(
            "write address-named Psy-Q rows that fall inside uniquely "
            "matched library objects"
        ),
    )
    mode.add_argument(
        "--emit-map",
        action="store_true",
        help="write semantic-symbol-map.csv rows for the new names to stdout",
    )
    mode.add_argument(
        "--check-resolutions",
        action="store_true",
        help="validate tracked local resolutions without retail inputs",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        if args.check_resolutions:
            count = check_resolution_inventory(ROOT)
            print(f"Psy-Q signature resolutions: OK ({count} entries)")
            return 0
        root = require_workspace_root()
        if not args.signatures:
            raise SignatureError("--signatures is required for a signature sweep")
        signatures = Path(args.signatures).expanduser()
        if not signatures.is_dir():
            raise SignatureError(f"not a directory: {signatures}")
        validate_catalogue_scope(signatures, args.psyq_version)
        load_address, payload = load_payload(root)
        inventory = load_inventory(root)
        resolutions = load_resolutions(root, signatures, args.psyq_version)
        scanned = scan(signatures, load_address, payload)
        result = classify(
            scanned["proposals"],
            inventory,
            scanned["objects"],
            resolutions,
        )
        if args.emit_map:
            emit_map(result, args.psyq_version)
        elif args.coverage_report:
            report_coverage(result)
        else:
            report(scanned, result)
        return 0
    except (SignatureError, WorkspaceError, OSError, ValueError, KeyError,
            json.JSONDecodeError, csv.Error) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
