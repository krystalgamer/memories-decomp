#!/usr/bin/env python3

"""Inventory registered overlay functions, loader images and unresolved archive regions."""

from __future__ import annotations

import argparse
import csv
import hashlib
import heapq
from importlib.metadata import version
import json
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

import rabbitizer

from hashing import sha256_file
from overlay_extract import OVERLAY_MANIFESTS, OverlayError, load_manifest
from workspace import WorkspaceError, require_workspace_root, resolve_within


SECTOR = 2048
MODEL_PHASES = (
    (7, 180, 10, 0x80010014), (8, 190, 10, 0x80010018),
    (9, 200, 10, 0x80010014), (10, 210, 10, 0x80010018),
    (11, 220, 2, 0x8001000C), (12, 222, 2, 0x80010010),
)
LOAD_POINTER_ADDRESSES = (0x80010000, 0x80010004, 0x80010008, 0x8001000C,
                         0x80010010, 0x80010014, 0x80010018, 0x8001002C,
                         0x80010030, 0x800101D8, 0x800101DC)
PROLOGUE = re.compile(rb"[\x00-\xff][\x80-\xff]\xbd\x27")
RETURN = re.compile(rb"\x08\x00\xe0\x03")
LOADER_SOURCES = (
    "src/game/model_load_monster_merge.c", "src/game/model_texture_transfer.c",
    "src/game/model.h", "src/game/european/model_load_monster_merge.c",
    "src/game/main_run_boot_sequence.c", "src/game/main_boot_load_stages.c",
    "src/game/duel_load_terrain_package.c", "src/game/duel_load_package_stage.c",
    "src/game/european/duel_load_terrain_package.c", "src/game/european/duel_load_package_stage.c",
    "src/game/japanese/duel_load_terrain_package.c", "src/game/japanese/duel_load_package_stage.c",
    "src/game/model_intro_controller.c", "src/game/european/model_intro_controller.c",
    "src/game/main_menu_load_package_stage.c", "src/game/european/main_menu_load_package_stage.c",
    "src/game/name_entry_load_package_stage.c", "src/game/japanese/name_entry_load_package_stage.c",
    "src/game/european/name_entry_load_package_stage.c",
    "src/game/frontend_package_stages.c", "src/game/european/options_package_stages.c",
)


class CensusError(RuntimeError):
    pass


def model_records() -> list[int]:
    return [model for model in range(0x2D2)
            if not (0x12C <= model < 0x15E or 0x28A <= model < 0x2BC or model == 0x2D0)]


def complement(size: int, spans: list[tuple[int, int]]) -> list[tuple[int, int]]:
    cursor = 0
    gaps = []
    for begin, end in sorted(spans):
        if begin < 0 or end <= begin or end > size:
            raise CensusError(f"invalid covered interval {begin}:{end} in {size} bytes")
        if begin > cursor:
            gaps.append((cursor, begin))
        cursor = max(cursor, end)
    if cursor < size:
        gaps.append((cursor, size))
    return gaps


def instruction(word: int, address: int):
    return rabbitizer.Instruction(word, address, rabbitizer.InstrCategory.R3000GTE)


def valid_instruction(ins, word: int) -> bool:
    op = word >> 26
    if not (op <= 19 or 32 <= op <= 38 or op in (40, 41, 42, 43, 46) or
            48 <= op <= 51 or 56 <= op <= 59):
        return False
    if op == 0 and word & 63 not in (
        0, 2, 3, 4, 6, 7, 8, 9, 12, 13, 16, 17, 18, 19, 24, 25, 26, 27,
        32, 33, 34, 35, 36, 37, 38, 39, 42, 43,
    ):
        return False
    if op == 1 and word >> 16 & 31 not in (0, 1, 16, 17):
        return False
    return ins.isValid() and ins.isImplemented() and not ins.isBranchLikely()


def walk_function(data: bytes, base: int, start: int, size: int | None = None) -> dict:
    limit = len(data) if size is None else start + size
    if start < 0 or start % 4 or limit > len(data) or limit <= start:
        raise CensusError(f"invalid function interval {start}:{limit}")
    pending = [start]
    visited: set[int] = set()
    calls: set[int] = set()
    external: set[int] = set()
    problems: set[str] = set()
    returns = 0
    indirect_calls = 0
    while pending:
        pc = pending.pop()
        if pc in visited:
            continue
        if pc < start or pc + 4 > limit or pc % 4:
            problems.add(f"flow_outside_span@{pc:#x}")
            continue
        word = struct.unpack_from("<I", data, pc)[0]
        ins = instruction(word, base + pc)
        if not valid_instruction(ins, word):
            problems.add(f"unsupported_instruction@{pc:#x}")
            continue
        visited.add(pc)
        if not ins.hasDelaySlot():
            if word >> 26 == 0 and word & 63 == 13:
                continue
            pending.append(pc + 4)
            continue
        if pc + 8 > limit:
            problems.add(f"missing_delay_slot@{pc:#x}")
            continue
        delay_word = struct.unpack_from("<I", data, pc + 4)[0]
        delay = instruction(delay_word, base + pc + 4)
        if not valid_instruction(delay, delay_word) or delay.hasDelaySlot():
            problems.add(f"unsupported_delay_slot@{pc:#x}")
            continue
        visited.add(pc + 4)
        if ins.isReturn():
            returns += 1
            continue
        op = word >> 26
        if ins.doesLink():
            if op == 3:
                target = ((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                (calls if base <= target < base + len(data) else external).add(target)
            elif ins.isBranch():
                displacement = (word & 65535) - (65536 if word & 32768 else 0)
                target = base + pc + 4 + displacement * 4
                (calls if base <= target < base + len(data) else external).add(target)
            else:
                indirect_calls += 1
            pending.append(pc + 8)
        elif ins.isBranch():
            displacement = (word & 65535) - (65536 if word & 32768 else 0)
            pending.append(pc + 4 + displacement * 4)
            if not ins.isUnconditionalBranch():
                pending.append(pc + 8)
        elif op == 2:
            target = ((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
            if base + start <= target < base + limit:
                pending.append(target - base)
            else:
                problems.add(f"tail_target@{target:#x}")
        else:
            problems.add(f"indirect_jump@{pc:#x}")
    extent = max(visited, default=start - 4) + 4 - start
    contiguous = bool(visited) and len(visited) * 4 == extent
    if not returns:
        problems.add("no_reached_return")
    if not contiguous:
        problems.add("noncontiguous_reachable_words")
    if size is not None and len(visited) * 4 != size:
        problems.add("registered_span_not_fully_reached")
    return {
        "visited": visited, "calls": calls, "external": external,
        "problems": problems, "returns": returns, "indirect_calls": indirect_calls,
        "extent": extent, "closed": not problems and contiguous and returns > 0,
    }


def prologue_offsets(data: bytes):
    for match in PROLOGUE.finditer(data):
        offset = match.start()
        frame = (-struct.unpack_from("<h", data, offset)[0])
        if offset % 4 == 0 and frame % 8 == 0:
            yield offset


def discover_functions(data: bytes, base: int, known: dict, entries: set[int], resident_calls: set[int]):
    seeds: dict[int, set[str]] = defaultdict(set)
    for offset in known:
        seeds[offset].add("registered_boundary")
    for offset in entries:
        seeds[offset].add("loader_entry")
    for offset in prologue_offsets(data):
        seeds[offset].add("stack_prologue")
    for address in resident_calls:
        if base <= address < base + len(data):
            seeds[address - base].add("resident_address_reference")
    for pc in range(0, len(data), 4):
        word = struct.unpack_from("<I", data, pc)[0]
        if word >> 26 == 3:
            target = ((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
            if base <= target < base + len(data):
                seeds[target - base].add("raw_local_call")
    pending = list(seeds)
    heapq.heapify(pending)
    analyzed = {}
    while pending:
        offset = heapq.heappop(pending)
        if offset in analyzed:
            continue
        if offset not in known and any(begin < offset < begin + row["size"] for begin, row in known.items()):
            continue
        result = walk_function(data, base, offset, known[offset]["size"] if offset in known else None)
        analyzed[offset] = result
        for target in result["calls"]:
            child = target - base
            seeds[child].add("cfg_local_call")
            if child not in analyzed:
                heapq.heappush(pending, child)
    rows = []
    for offset, result in sorted(analyzed.items()):
        registered = known.get(offset)
        size = registered["size"] if registered else ""
        span = size or result["extent"]
        rows.append({
            "offset": f"0x{offset:X}", "address": f"0x{base + offset:08X}",
            "name": registered["name"] if registered else f"candidate_{base + offset:08X}",
            "classification": "registered_boundary" if registered else
                              "candidate_cfg_closed" if result["closed"] else "unresolved_candidate",
            "size": size, "observed_extent": result["extent"],
            "reachable_bytes": len(result["visited"]) * 4,
            "cfg_closed": int(result["closed"]), "seed_evidence": ";".join(sorted(seeds[offset])),
            "reference_status": registered["status"] if registered else "",
            "body_sha256": hashlib.sha256(data[offset:offset + span]).hexdigest() if span else "",
            "local_calls": ";".join(f"0x{x:08X}" for x in sorted(result["calls"])),
            "external_calls": ";".join(f"0x{x:08X}" for x in sorted(result["external"])),
            "indirect_calls": result["indirect_calls"],
            "unresolved": ";".join(sorted(result["problems"])),
        })
    return rows


def write_csv(path: Path, fields: list[str], rows) -> int:
    count = 0
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, lineterminator="\n")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)
            count += 1
    return count


def load_pointers(data: bytes) -> dict[int, int]:
    if len(data) < 0x800 or data[:8] != b"PS-X EXE":
        raise CensusError("missing executable header")
    base = struct.unpack_from("<I", data, 0x18)[0]
    result = {}
    for address in LOAD_POINTER_ADDRESSES:
        offset = address - base + 0x800
        if offset < 0x800 or offset + 4 > len(data):
            raise CensusError(f"load-pointer storage {address:#x} outside executable")
        value = struct.unpack_from("<I", data, offset)[0]
        if not 0x80000000 <= value < 0x80200000 or value % 4:
            raise CensusError(f"invalid RAM load pointer at {address:#x}: {value:#x}")
        result[address] = value
    return result


def extra_loads(region: str, pointers: dict[int, int]):
    pal = region not in ("usa", "japan")
    boot = 9509 if pal else 5810 if region == "japan" else 5827
    yield "WA_MRG.MRG", boot, 3, pointers[0x800101D8], "boot_compliance", {0xF4, 0x160}
    first, stride, count = (7193, 240, 44) if pal else (
        (5953, 239, 48) if region == "japan" else (5970, 235, 44))
    for terrain in range(7):
        yield "WA_MRG.MRG", first + terrain * stride, count, pointers[0x800101DC], f"duel_terrain_{terrain}", set()
    yield "SU.MRG", 1767 if pal else 1223, 16, pointers[0x80010030], "model_intro_credits", set()
    for slot in (0, 1):
        yield "SU.MRG", (1492 if pal else 948) + 194 + slot * 10, 10, \
            pointers[0x80010014 + slot * 4], f"special_battle_slot{slot}", {4}
    for index in range(7):
        yield "SU.MRG", (680 if pal else 136) + index * 116, 18, \
            pointers[0x80010008], f"auxiliary_model_data_{index}", set()
    for language in range(5 if pal else 1):
        yield "SU.MRG", 98 + language * 136, 16, pointers[0x8001002C], f"main_menu_language_{language}", set()
    yield "WA_MRG.MRG", 9374 if pal else 7979 if region == "japan" else 7968, \
        15, pointers[0x800101D8], "name_entry", set()
    if pal:
        for language in range(5):
            yield "WA_MRG.MRG", 10170 + language * 41, 6, pointers[0x800101D8], \
                f"options_language_{language}", set()


def read_payload(root: Path, image: dict) -> bytes:
    with resolve_within(root, image["archive"], must_exist=True).open("rb") as handle:
        handle.seek(image["sector_offset"] * SECTOR)
        data = handle.read(image["sector_count"] * SECTOR)
    if len(data) != image["sector_count"] * SECTOR:
        raise CensusError(f"short archive read: {image}")
    return data


def collect_images(root: Path, region: str, inputs: dict, pointers: dict[int, int]):
    sector_size, modules = load_manifest(root, region)
    if sector_size != SECTOR:
        raise CensusError(f"{region}: unsupported sector size {sector_size}")
    by_name = {Path(path).name: path for path in inputs if path.endswith(".MRG")}
    images = {}
    metadata_hashes = {}
    registered_counts = Counter()

    def add(archive, sector, count, base, label, entries, model="", stage="", known=None, kind="code_load"):
        key = (archive, sector, count, base)
        row = images.setdefault(key, {
            "archive": archive, "sector_offset": sector, "sector_count": count,
            "load_address": base, "labels": set(), "entries": set(), "known": {},
            "model": model, "stage": stage, "configured": set(), "kind": kind,
        })
        row["labels"].add(label)
        row["entries"].update(entries)
        if known is not None:
            row["configured"].add(label)
            for offset, function in known.items():
                if offset in row["known"] and row["known"][offset] != function:
                    raise CensusError(f"{region}: contradictory registered boundaries for {key}")
                row["known"][offset] = function
        return row

    records = model_records()
    model_path = by_name["MODEL.MRG"]
    if resolve_within(root, model_path).stat().st_size != len(records) * 276 * SECTOR:
        raise CensusError(f"{region}: MODEL archive does not match the loader record domain")
    for record, model in enumerate(records):
        for stage, offset, count, pointer in MODEL_PHASES:
            add(model_path, record * 276 + offset, count, pointers[pointer], "model_record", {4}, model, stage)
        for slot in (0, 1):
            add(model_path, record * 276, 96, pointers[0x80010000 + slot * 4],
                f"model_bulk_data_slot{slot}", set(), model, 0, kind="data_load")
    for name, sector, count, base, label, entries in extra_loads(region, pointers):
        add(by_name[name], sector, count, base, label, entries,
            kind="data_load" if label.startswith("auxiliary_model_data_") else "code_load")
    for module in modules:
        if module["archive"] not in inputs or module["archive_sha256"] != inputs[module["archive"]]:
            raise CensusError(f"{region}: configured archive checksum disagrees for {module['name']}")
        layout = resolve_within(root, module["layout"], must_exist=True)
        inventory = layout.with_name(layout.stem + "_functions.csv")
        metadata_hashes[str(inventory.relative_to(root))] = sha256_file(inventory)
        with inventory.open(encoding="utf-8", newline="") as handle:
            functions = list(csv.DictReader(handle))
        base = int(module["load_address"], 0)
        known = {}
        for function in functions:
            offset, size = int(function["address"], 0) - base, int(function["size"], 0)
            if offset < 0 or offset % 4 or size <= 0 or size % 4 or offset + size > module["sector_count"] * SECTOR:
                raise CensusError(f"{inventory}: invalid function range {function}")
            if offset in known:
                raise CensusError(f"{inventory}: duplicate function boundary")
            known[offset] = {"size": size, "name": function["name"], "status": function["status"]}
            registered_counts[function["status"]] += 1
        ordered = sorted(known)
        if any(left + known[left]["size"] > right for left, right in zip(ordered, ordered[1:])):
            raise CensusError(f"{inventory}: overlapping registered function ranges")
        for sector in [module["sector_offset"], *module.get("duplicate_sector_offsets", [])]:
            row = add(module["archive"], sector, module["sector_count"], base, module["name"], set(), known=known)
            if hashlib.sha256(read_payload(root, row)).hexdigest() != module["sha256"]:
                raise CensusError(f"{module['name']}: retail payload hash mismatch at {sector}")
    return sorted(images.values(), key=lambda row: (row["archive"], row["sector_offset"], row["load_address"])), \
        metadata_hashes, dict(registered_counts)


def verify_inputs(root: Path, region: str):
    config = Path(OVERLAY_MANIFESTS[region]).parent
    checksums = resolve_within(root, config / "files.sha256", must_exist=True)
    result = {}
    for line in checksums.read_text().splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        expected, name = line.split()
        name = name.removeprefix("*")
        if Path(name).name in ("MODEL.MRG", "SU.MRG", "WA_MRG.MRG") or re.fullmatch(r"S(?:LUS|LPM|LES)_\d{3}\.\d{2}", Path(name).name):
            path = resolve_within(root, name, must_exist=True)
            if sha256_file(path) != expected:
                raise CensusError(f"{name}: retail checksum mismatch")
            result[name] = expected
    if len(result) != 4:
        raise CensusError(f"{region}: expected the executable and all three archive checksums")
    return result


def resident_call_targets(root: Path, inputs: dict, region: str) -> set[int]:
    executable, = [path for path in inputs if not path.endswith(".MRG")]
    data = resolve_within(root, executable).read_bytes()
    if data[:8] != b"PS-X EXE":
        raise CensusError(f"{executable}: missing executable header")
    load_address = struct.unpack_from("<I", data, 0x18)[0]
    path = resolve_within(root, Path(OVERLAY_MANIFESTS[region]).parent / "functions.csv")
    calls = set()
    with path.open(newline="") as handle:
        for row in csv.DictReader(handle):
            start, size = int(row["address"], 0), int(row["size"], 0)
            offset = start - load_address + 0x800
            if offset < 0x800 or offset + size > len(data):
                raise CensusError(f"{path}: function outside executable payload")
            for pc in range(offset, offset + size, 4):
                word = struct.unpack_from("<I", data, pc)[0]
                if word >> 26 == 3:
                    calls.add(((start + pc - offset + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2))
    return calls


def unmapped_regions(root: Path, inputs: dict, images: list[dict], hints):
    for archive in sorted(path for path in inputs if path.endswith(".MRG")):
        path = resolve_within(root, archive)
        spans = [(row["sector_offset"] * SECTOR,
                  (row["sector_offset"] + row["sector_count"]) * SECTOR)
                 for row in images if row["archive"] == archive]
        with path.open("rb") as handle:
            for begin, end in complement(path.stat().st_size, spans):
                prologues = returns = 0
                handle.seek(begin)
                position = begin
                while position < end:
                    chunk = handle.read(min(1024 * 1024, end - position))
                    if not chunk:
                        raise CensusError(f"{archive}: short read in unmapped region")
                    for offset in prologue_offsets(chunk):
                        prologues += 1
                        hints.writerow({"archive": archive, "file_offset": position + offset,
                                        "hint": "stack_prologue", "load_address": "",
                                        "caveat": "instruction_pattern_not_a_proven_function"})
                    for match in RETURN.finditer(chunk):
                        if match.start() % 4 == 0:
                            returns += 1
                            hints.writerow({"archive": archive, "file_offset": position + match.start(),
                                            "hint": "return_instruction", "load_address": "",
                                            "caveat": "instruction_pattern_not_a_proven_function"})
                    position += len(chunk)
                yield {"archive": archive, "begin": begin, "end": end, "bytes": end - begin,
                       "stack_prologue_hints": prologues, "return_hints": returns,
                       "classification": "outside_catalogued_loads_not_proven_code_or_data"}


def inventory_region(root: Path, region: str, output: Path) -> dict:
    inputs = verify_inputs(root, region)
    executable, = [path for path in inputs if not path.endswith(".MRG")]
    pointers = load_pointers(resolve_within(root, executable).read_bytes())
    images, metadata, registered = collect_images(root, region, inputs, pointers)
    calls = resident_call_targets(root, inputs, region)
    groups = {}
    image_rows = []
    for row in images:
        data = read_payload(root, row)
        digest = hashlib.sha256(data).hexdigest()
        identity = f"{row['load_address']:08X}-{digest}"
        group = groups.setdefault(identity, {"sample": row, "known": {}, "entries": set(), "kinds": set()})
        group["kinds"].add(row["kind"])
        for offset, function in row["known"].items():
            if offset in group["known"] and group["known"][offset] != function:
                raise CensusError(f"{region}: identical images have contradictory registered functions")
            group["known"][offset] = function
        group["entries"].update(row["entries"])
        image_rows.append({
            "image_id": identity, "archive": row["archive"],
            "sector_offset": row["sector_offset"], "sector_count": row["sector_count"],
            "load_address": f"0x{row['load_address']:08X}", "sha256": digest,
            "header_word": f"0x{struct.unpack_from('<I', data)[0]:08X}",
            "model": row["model"], "stage": row["stage"],
            "load_kind": row["kind"],
            "loader_evidence": ";".join(sorted(row["labels"])),
            "configured_modules": ";".join(sorted(row["configured"])),
        })
    write_csv(output / f"{region}-images.csv", list(image_rows[0]), image_rows)
    counts = Counter()
    data_counts = Counter()
    body_hashes = set()
    data_body_hashes = set()
    data_rows = []
    coverage_rows = []
    fields = ["image_id", "load_kind", "offset", "address", "name", "classification", "size", "observed_extent",
              "reachable_bytes", "cfg_closed", "seed_evidence", "reference_status", "body_sha256",
              "local_calls", "external_calls", "indirect_calls", "unresolved"]

    def functions():
        for identity, group in sorted(groups.items()):
            sample = group["sample"]
            data_only = group["kinds"] == {"data_load"}
            data = read_payload(root, sample)
            rows = discover_functions(data, sample["load_address"],
                                      group["known"], group["entries"], set() if data_only else calls)
            if data_only:
                data_rows.append({
                    "image_id": identity,
                    "closed_candidates": sum(row["cfg_closed"] for row in rows),
                    "unresolved_candidates": sum(not row["cfg_closed"] for row in rows),
                    "candidate_offsets": ";".join(row["offset"] for row in rows),
                    "caveat": "instruction_shaped_data_not_proven_executable_functions",
                })
            else:
                registered_spans = [(offset, offset + function["size"])
                                    for offset, function in group["known"].items()]
                candidate_spans = [(int(row["offset"], 0), int(row["offset"], 0) + row["observed_extent"])
                                   for row in rows if row["classification"] == "candidate_cfg_closed"]
                gaps = complement(len(data), registered_spans + candidate_spans)
                returns = [offset + begin for begin, end in gaps
                           for offset in (match.start() for match in RETURN.finditer(data[begin:end]))
                           if (offset + begin) % 4 == 0]
                registered_bytes = sum(end - begin for begin, end in registered_spans)
                unassigned_bytes = sum(end - begin for begin, end in gaps)
                coverage_rows.append({
                    "image_id": identity, "image_bytes": len(data),
                    "registered_function_bytes": registered_bytes,
                    "additional_candidate_span_bytes": len(data) - registered_bytes - unassigned_bytes,
                    "unassigned_bytes": unassigned_bytes,
                    "unassigned_ranges": ";".join(f"0x{begin:X}:0x{end:X}" for begin, end in gaps),
                    "unassigned_return_hints": ";".join(f"0x{offset:X}" for offset in returns),
                    "caveat": "candidate_spans_and_remaining_bytes_are_not_proven_code_data_classification",
                })
            for row in rows:
                (data_counts if data_only else counts)[row["classification"]] += 1
                if row["classification"] == "registered_boundary" or row["cfg_closed"]:
                    (data_body_hashes if data_only else body_hashes).add(row["body_sha256"])
                if not data_only:
                    yield {"image_id": identity, "load_kind": ";".join(sorted(group["kinds"])), **row}

    write_csv(output / f"{region}-functions.csv", fields, functions())
    write_csv(output / f"{region}-data-hints.csv",
              ["image_id", "closed_candidates", "unresolved_candidates", "candidate_offsets", "caveat"], data_rows)
    write_csv(output / f"{region}-coverage.csv",
              ["image_id", "image_bytes", "registered_function_bytes", "additional_candidate_span_bytes",
               "unassigned_bytes", "unassigned_ranges", "unassigned_return_hints", "caveat"], coverage_rows)
    with (output / f"{region}-unmapped-hints.csv").open("w", encoding="utf-8", newline="") as handle:
        hints = csv.DictWriter(handle, fieldnames=["archive", "file_offset", "hint", "load_address", "caveat"],
                               lineterminator="\n")
        hints.writeheader()
        gaps = list(unmapped_regions(root, inputs, images, hints))
    write_csv(output / f"{region}-unmapped.csv",
              ["archive", "begin", "end", "bytes", "stack_prologue_hints", "return_hints", "classification"], gaps)
    metadata[str(Path(OVERLAY_MANIFESTS[region]))] = sha256_file(root / OVERLAY_MANIFESTS[region])
    config = Path(OVERLAY_MANIFESTS[region]).parent
    for name in ("files.sha256", "functions.csv"):
        metadata[str(config / name)] = sha256_file(root / config / name)
    return {
        "inputs": inputs, "metadata": metadata, "physical_images": len(images),
        "load_pointers": {f"0x{address:08X}": f"0x{value:08X}" for address, value in pointers.items()},
        "unique_loaded_images": len(groups), "model_records": len(model_records()),
        "code_load_instances": sum(row["kind"] == "code_load" for row in images),
        "data_load_instances": sum(row["kind"] == "data_load" for row in images),
        "model_phase_instances": len(model_records()) * len(MODEL_PHASES),
        "model_bulk_data_load_instances": len(model_records()) * 2,
        "registered_function_instances": registered,
        "unique_image_function_sites": dict(sorted(counts.items())),
        "data_load_instruction_candidates": dict(sorted(data_counts.items())),
        "distinct_registered_or_closed_candidate_body_hashes": len(body_hashes),
        "distinct_closed_data_candidate_body_hashes": len(data_body_hashes),
        "unmapped_archive_bytes": sum(row["bytes"] for row in gaps),
        "unmapped_stack_prologue_hints": sum(row["stack_prologue_hints"] for row in gaps),
        "unmapped_return_hints": sum(row["return_hints"] for row in gaps),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--region", choices=tuple(OVERLAY_MANIFESTS), action="append")
    parser.add_argument("--output", default="tmp/overlay-function-inventory")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        output = resolve_within(root, args.output)
        if output.exists():
            raise CensusError(f"{args.output}: output already exists; use a fresh directory")
        output.mkdir(parents=True)
        regions = sorted(set(args.region or OVERLAY_MANIFESTS))
        summary = {"schema": 1, "complete": False, "requested_regions": regions,
                   "regions": {}, "loader_sources": {},
                   "generator_sha256": sha256_file(Path(__file__)),
                   "rabbitizer_version": version("rabbitizer")}
        for path in LOADER_SOURCES:
            summary["loader_sources"][path] = sha256_file(resolve_within(root, path, must_exist=True))
        for region in regions:
            summary["regions"][region] = inventory_region(root, region, output)
            (output / "summary.json").write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
            row = summary["regions"][region]
            print(f"{region}: {row['physical_images']} loads, {row['unique_loaded_images']} unique images; "
                  f"function sites {row['unique_image_function_sites']}", flush=True)
        summary["report_files"] = {path.name: sha256_file(path) for path in sorted(output.glob("*.csv"))}
        summary["complete"] = True
        (output / "summary.json").write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
        return 0
    except (CensusError, OverlayError, WorkspaceError, OSError, ValueError, KeyError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
