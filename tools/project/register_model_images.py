#!/usr/bin/env python3
"""Register the complete MODEL loader domain without promoting candidate functions."""

from __future__ import annotations

import argparse
from collections import defaultdict
import csv
import hashlib
import json
from pathlib import Path
import re
import sys

from function_inventory import FIELDS
from hashing import sha256_file
from model_overlay_inventory import validate_domain
from overlay_extract import OVERLAY_MANIFESTS, OverlayError, load_manifest, read_modules
from overlay_function_inventory import (
    CensusError, MODEL_PHASES, SECTOR, extra_loads, load_pointers, model_records,
    verify_inputs,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within


PREFIXES = {
    "usa": "", "japan": "japanese_", "europe": "european_",
    "spain": "spanish_", "france": "french_", "germany": "german_",
    "italy": "italian_",
}


def physical_key(module: dict, sector: int | None = None) -> tuple:
    return (module["archive"], int(module["sector_offset"]) if sector is None else sector,
            int(module["sector_count"]), int(str(module["load_address"]), 0))


def registered_keys(modules: list[dict]) -> dict[tuple, dict]:
    result = {}
    for module in modules:
        for sector in [module["sector_offset"], *module.get("duplicate_sector_offsets", [])]:
            key = physical_key(module, sector)
            if key in result:
                raise CensusError(f"duplicate physical registration: {key}")
            result[key] = module
    return result


def missing_groups(images: list[dict], modules: list[dict]) -> list[list[dict]]:
    registered = registered_keys(modules)
    groups = defaultdict(list)
    for image in images:
        key = physical_key(image)
        if key in registered:
            if registered[key]["sha256"] != image["sha256"]:
                raise CensusError(f"registered payload disagrees with inventory: {key}")
            continue
        # Never combine archives, load addresses, or merely identical entry bodies.
        groups[(key[0], key[2], key[3], image["sha256"])].append(image)
    return [sorted(rows, key=physical_key) for _, rows in sorted(groups.items())]


def layout_text(module: dict, sha1: str) -> str:
    name = module["name"]
    base = int(module["load_address"], 0)
    size = module["sector_count"] * SECTOR
    directory = f"tmp/overlays/{name}"
    return f"""# Baseline image only: instruction-shaped bytes remain unclassified.
name: {name}
sha1: {sha1}

options:
  basename: {name}
  base_path: ../../..
  target_path: {module['output']}
  elf_path: {directory}/build/{name}.elf
  platform: psx
  compiler: PSYQ
  endianness: little
  asm_path: {directory}/asm
  src_path: src
  build_path: {directory}/build
  asset_path: {directory}/assets
  cache_path: {directory}/cache
  data_path: data
  nonmatchings_path: nonmatchings
  matchings_path: matchings
  ld_script_path: {directory}/{name}.ld
  generated_asm_macros_directory: {directory}/include
  lib_path: {directory}/lib
  o_path: {directory}/build
  undefined_funcs_auto_path: {directory}/undefined_funcs_auto.txt
  undefined_syms_auto_path: {directory}/undefined_syms_auto.txt
  find_file_boundaries: false
  check_consecutive_segment_types: false
  o_as_suffix: true
  use_legacy_include_asm: false
  create_asm_dependencies: true
  ld_dependencies: true
  subalign: 2
  section_order:
    - ".text"
    - ".rodata"
    - ".data"
    - ".sdata"
    - ".sbss"
    - ".bss"

segments:
  - name: unclassified_image
    type: code
    start: 0x0
    vram: {base:#010x}
    align: 4
    subsegments:
      - [0x0, bin, overlays/{name}/unclassified_image]
  - [{size:#x}]
"""


def append_modules(text: str, modules: list[dict]) -> str:
    if not modules:
        return text
    match = re.search(r'"modules"\s*:\s*', text)
    if match is None:
        raise CensusError("manifest has no module list")
    existing, end = json.JSONDecoder().raw_decode(text, match.end())
    if not isinstance(existing, list) or not existing:
        raise CensusError("expected a nonempty module list")
    line = text.rfind("\n", match.end(), end - 1) + 1
    if text[line:end - 1].strip():
        raise CensusError("expected an independently indented module-list terminator")
    additions = ",\n".join(
        "\n".join("    " + row for row in json.dumps(m, indent=2, sort_keys=True).splitlines())
        for m in modules
    )
    return text[:line].rstrip() + ",\n" + additions + "\n" + text[line:]


def inventory_images(root: Path, region: str, inputs: dict) -> list[dict]:
    directory = root / "notes/overlays/model-inventory" / region
    summary = json.loads((directory / "summary.json").read_text())
    if summary["inputs"] != inputs:
        raise CensusError(f"{region}: inventory retail inputs disagree")
    rows = {}
    for name in ("images.csv", "data-loads.csv"):
        path = directory / name
        if sha256_file(path) != summary["report_files"][name]:
            raise CensusError(f"{region}/{name}: inventory checksum mismatch")
        with path.open(newline="", encoding="utf-8") as handle:
            rows[name] = list(csv.DictReader(handle))
    images = rows["images.csv"]
    validate_domain(images + rows["data-loads.csv"])
    if len(images) != 3729 or any(row["load_kind"] != "code_load" for row in images):
        raise CensusError(f"{region}: invalid executable image domain")
    executable = next(name for name in inputs if not name.endswith(".MRG"))
    pointers = load_pointers(resolve_within(root, executable, must_exist=True).read_bytes())
    archives = {Path(name).name: name for name in inputs if name.endswith(".MRG")}
    expected = {
        (archives["MODEL.MRG"], record * 276 + offset, count, pointers[pointer])
        for record, _model in enumerate(model_records())
        for _stage, offset, count, pointer in MODEL_PHASES
    }
    expected.update(
        (archives[archive], sector, count, base)
        for archive, sector, count, base, label, _entries in extra_loads(region, pointers)
        if label == "model_intro_credits" or label.startswith("special_battle_slot")
    )
    keys = [physical_key(row) for row in images]
    if len(set(keys)) != len(keys) or set(keys) != expected:
        raise CensusError(f"{region}: inventory disagrees with executable loader destinations")
    return images


def register_region(root: Path, region: str, check: bool = False) -> dict:
    inputs = verify_inputs(root, region)
    images = inventory_images(root, region, inputs)
    sector_size, modules = load_manifest(root, region)
    if sector_size != SECTOR:
        raise CensusError(f"{region}: unsupported sector size")
    groups = missing_groups(images, modules)
    if check and groups:
        raise CensusError(f"{region}: {sum(map(len, groups))} MODEL images remain unregistered")
    manifest_path = root / OVERLAY_MANIFESTS[region]
    manifest_text = manifest_path.read_text()
    manifest = json.loads(manifest_text)
    new_modules = []
    for group in groups:
        first = group[0]
        bank = "model" if Path(first["archive"]).name == "MODEL.MRG" else "su"
        stem = f"model_image_{bank}_{int(first['sector_offset'])}_{int(first['load_address'], 0):08x}"
        name = PREFIXES[region] + stem
        module = {
            "name": name, "archive": first["archive"],
            "archive_sha256": inputs[first["archive"]],
            "sector_offset": int(first["sector_offset"]),
            "sector_count": int(first["sector_count"]),
            "load_address": first["load_address"], "sha256": first["sha256"],
            "output": f"tmp/overlays/{name}/module.bin",
            "layout": str(manifest_path.parent.relative_to(root) / "overlays" / f"{stem}.yaml"),
        }
        if len(group) > 1:
            module["duplicate_sector_offsets"] = [int(row["sector_offset"]) for row in group[1:]]
        new_modules.append(module)
    # Verify every payload and duplicate through the ordinary archive reader before
    # changing any tracked metadata. Existing C/assembly layouts are never rewritten.
    relevant = {physical_key(row) for row in images}
    selected = [m for m in modules if physical_key(m) in relevant] + new_modules
    payloads = read_modules(root, SECTOR, selected)
    new_payloads = payloads[len(selected) - len(new_modules):] if new_modules else []
    writes = {}
    for module, (_output, payload) in zip(new_modules, new_payloads):
        path = root / module["layout"]
        writes[path] = layout_text(module, hashlib.sha1(payload).hexdigest())
        writes[path.with_name(path.stem + "_functions.csv")] = ",".join(FIELDS) + "\n"
        writes[path.with_name(path.stem + "_matching_c.json")] = (
            json.dumps({"schema": 1, "functions": []}, indent=2, sort_keys=True) + "\n"
        )
    for path in writes:
        if path.exists():
            raise CensusError(f"refusing to overwrite existing metadata: {path.relative_to(root)}")
    updated_manifest = append_modules(manifest_text, new_modules)
    for module in selected:
        if module not in new_modules:
            resolve_within(root, module["layout"], must_exist=True)
    for path, text in writes.items():
        path.write_text(text, encoding="utf-8")
    if new_modules:
        manifest["modules"].extend(new_modules)
        manifest_path.write_text(updated_manifest)
    if missing_groups(images, manifest["modules"]):
        raise CensusError(f"{region}: incomplete registration")
    return {"region": region, "physical_images": len(images), "new_layouts": len(new_modules),
            "new_physical_images": sum(map(len, groups)), "new_functions": 0}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--region", choices=tuple(OVERLAY_MANIFESTS), action="append", required=True)
    parser.add_argument("--check", action="store_true", help="verify coverage without writing files")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        for region in dict.fromkeys(args.region):
            print(json.dumps(register_region(root, region, args.check)), flush=True)
    except (CensusError, OverlayError, WorkspaceError, OSError, ValueError, KeyError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
