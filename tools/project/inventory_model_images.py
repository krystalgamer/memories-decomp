#!/usr/bin/env python3

"""Give raw MODEL/SU baseline images closed-CFG function inventories.

`register_model_images.py` registered every MODEL code image as one raw `bin`
subsegment. This tool replaces a still-raw baseline with generated assembly for
each function whose control flow closes inside the image, keeping every other
byte as raw unclassified `bin`. Only untouched baselines are rewritten, so
matched or hand-maintained layouts are never changed.
"""

from __future__ import annotations

import argparse
import csv
import io
import json
from pathlib import Path
import re
import struct
import sys

from function_inventory import FIELDS
from overlay_extract import OVERLAY_MANIFESTS, OverlayError, load_manifest
from overlay_function_inventory import (
    SECTOR, CensusError, collect_images, discover_functions, load_pointers, read_payload,
    register_constant, resident_call_targets, verify_inputs,
)
from register_model_images import PREFIXES, layout_text
from workspace import WorkspaceError, require_workspace_root, resolve_within


HEADER = "# Generated function inventory: closed-CFG functions are generated assembly;\n" \
         "# every other byte remains raw and unclassified.\n"
NOTES = {
    "entry": "Loader entry; game-owned closed CFG with resolved switch tables and tail jumps; "
             "generated assembly retained. Other raw bytes remain unclassified.",
    "reachable": "Reached from the loader entry through local calls or in-image address references; "
                 "closed CFG; generated assembly retained.",
    "unreached": "Closed CFG found by {evidence}; no loader-entry call or address reference found, "
                 "so reachability is unresolved; generated assembly retained.",
}


class InventoryError(RuntimeError):
    pass


def baseline_text(module: dict, text: str) -> bool:
    sha1 = re.search(r"^sha1: ([0-9a-f]{40})$", text, re.MULTILINE)
    return sha1 is not None and text == layout_text(module, sha1[1])


def inventory_layout_text(module: dict, sha1: str, functions: list[tuple[int, int]]) -> str:
    """Return the layout for sorted, non-overlapping (offset, size) functions."""
    base = int(module["load_address"], 0)
    size = module["sector_count"] * SECTOR
    name = module["name"]
    raw = layout_text(module, sha1)
    head = raw[raw.index("\nname:") + 1:raw.index("segments:\n")]
    if not functions or functions[0][0] != 4:
        raise InventoryError(f"{name}: inventory must start with the loader entry at +4")
    segments = [("module_header", 0, "data")]
    cursor = 4
    for offset, length in functions:
        if offset < cursor or offset % 4 or length <= 0 or length % 4 or offset + length > size:
            raise InventoryError(f"{name}: invalid function span {offset:#x}+{length:#x}")
        if offset > cursor:
            segments.append((f"unclassified_{cursor:x}", cursor, "bin"))
        segments.append((f"unmatched_{offset:x}", offset, "asm"))
        cursor = offset + length
    if cursor < size:
        segments.append(("unclassified_tail", cursor, "bin"))
    lines = [HEADER.rstrip("\n"), head.rstrip("\n"), "", "segments:"]
    for segment, offset, kind in segments:
        lines += [
            f"  - name: {segment}", "    type: code", f"    start: {offset:#x}",
            f"    vram: {base + offset:#010x}", "    align: 4", "    subsegments:",
            f"      - [{offset:#x}, {kind}, overlays/{name}/{segment}]",
        ]
    lines.append(f"  - [{size:#x}]")
    return "\n".join(lines) + "\n"


def functions_text(rows: list[dict]) -> str:
    output = io.StringIO()
    writer = csv.DictWriter(output, fieldnames=FIELDS, lineterminator="\n")
    writer.writeheader()
    writer.writerows(rows)
    return output.getvalue()


def address_references(data: bytes, base: int, spans: list[tuple[int, int]]) -> set[int]:
    taken = set()
    for start, length in spans:
        for pc in range(start, start + length, 4):
            word = struct.unpack_from("<I", data, pc)[0]
            if word >> 26 in (9, 13):
                value = register_constant(data, start, pc + 4, word >> 16 & 31)
                if value is not None and base <= value < base + len(data):
                    taken.add(value - base)
    return taken


def image_functions(data: bytes, base: int, entries: set[int], calls: set[int]) -> list[tuple[int, int, str]]:
    rows = [row for row in discover_functions(data, base, {}, entries, calls, resolve_flow=True)
            if row["cfg_closed"]]
    spans = {int(row["offset"], 0): row for row in rows}
    ordered = sorted(spans)
    if any(left + spans[left]["observed_extent"] > right for left, right in zip(ordered, ordered[1:])):
        raise InventoryError("closed candidate spans overlap")
    taken = address_references(data, base, [(offset, spans[offset]["observed_extent"]) for offset in ordered])
    reached = set()
    pending = [offset for offset in ordered if offset in entries or offset in taken
               or "resident_address_reference" in spans[offset]["seed_evidence"]]
    while pending:
        offset = pending.pop()
        if offset in reached or offset not in spans:
            continue
        reached.add(offset)
        pending.extend(int(target, 16) - base for target in spans[offset]["local_calls"].split(";") if target)
    result = []
    for offset in ordered:
        kind = "entry" if offset in entries else "reachable" if offset in reached else "unreached"
        evidence = spans[offset]["seed_evidence"].replace("_", " ").replace(";", " and ")
        result.append((offset, spans[offset]["observed_extent"], NOTES[kind].format(evidence=evidence)))
    return result


def inventory_region(root: Path, region: str, check: bool) -> dict:
    inputs = verify_inputs(root, region)
    executable, = [path for path in inputs if not path.endswith(".MRG")]
    pointers = load_pointers(resolve_within(root, executable).read_bytes())
    images, _, _ = collect_images(root, region, inputs, pointers)
    calls = resident_call_targets(root, inputs, region)
    by_name = {label: image for image in images for label in image["configured"]}
    _, modules = load_manifest(root, region)
    prefix = PREFIXES[region] + "model_image_"
    summary = {"baselines": 0, "inventoried": 0, "functions": 0, "function_bytes": 0, "stale": []}
    for module in modules:
        if not module["name"].startswith(prefix) or "linker_symbols" in module:
            continue
        layout = resolve_within(root, module["layout"], must_exist=True)
        inventory = layout.with_name(layout.stem + "_functions.csv")
        manifest = layout.with_name(layout.stem + "_matching_c.json")
        text = layout.read_text(encoding="utf-8")
        if json.loads(manifest.read_text(encoding="utf-8")) != {"schema": 1, "functions": []}:
            continue
        image = by_name[module["name"]]
        data = read_payload(root, image)
        base = image["load_address"]
        try:
            functions = image_functions(data, base, image["entries"], calls)
        except InventoryError as error:
            raise InventoryError(f"{module['name']}: {error}") from None
        sha1 = re.search(r"^sha1: ([0-9a-f]{40})$", text, re.MULTILINE)[1]
        expected_layout = inventory_layout_text(module, sha1, [(offset, size) for offset, size, _ in functions])
        expected_rows = functions_text([{
            "address": f"0x{base + offset:08X}", "size": f"0x{size:X}", "name": f"func_{base + offset:08X}",
            "status": "unmatched_asm", "module": f"overlay/{layout.stem}",
            "notes": notes,
        } for offset, size, notes in functions])
        is_baseline = baseline_text(module, text) and \
            inventory.read_text(encoding="utf-8") == ",".join(FIELDS) + "\n"
        if not is_baseline and not text.startswith(HEADER):
            continue
        summary["baselines"] += is_baseline
        summary["inventoried"] += 1
        summary["functions"] += len(functions)
        summary["function_bytes"] += sum(size for _, size, _ in functions)
        if text == expected_layout and inventory.read_text(encoding="utf-8") == expected_rows:
            continue
        if check:
            summary["stale"].append(module["name"])
            continue
        layout.write_text(expected_layout, encoding="utf-8")
        inventory.write_text(expected_rows, encoding="utf-8")
    return summary


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--region", choices=tuple(OVERLAY_MANIFESTS), action="append", required=True)
    parser.add_argument("--check", action="store_true", help="fail if any raw baseline or inventory is stale")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        failed = False
        for region in args.region:
            summary = inventory_region(root, region, args.check)
            print(f"{region}: {summary['inventoried']} layouts ({summary['baselines']} raw baselines), "
                  f"{summary['functions']} functions, {summary['function_bytes']:,} bytes"
                  + (f", {len(summary['stale'])} stale" if args.check else ""), flush=True)
            failed |= bool(summary["stale"])
        return 1 if failed else 0
    except (InventoryError, CensusError, OverlayError, WorkspaceError, OSError, ValueError, KeyError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
