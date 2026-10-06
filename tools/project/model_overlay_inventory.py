#!/usr/bin/env python3
"""Extract a MODEL-only image denominator and conservative backlog from a full census."""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import csv
import json
from pathlib import Path
import sys

from hashing import sha256_file
from overlay_function_inventory import CensusError, MODEL_PHASES, model_records, write_csv
from workspace import WorkspaceError, require_workspace_root, resolve_within


def family(image: dict) -> str | None:
    labels = set(image["loader_evidence"].split(";"))
    if "model_record" in labels:
        return "primary" if int(image["stage"]) in (11, 12) else "variant"
    if "model_intro_credits" in labels:
        return "intro_credits"
    if any(label.startswith("special_battle_slot") for label in labels):
        return "special_battle"
    if any(label.startswith("model_bulk_data_slot") for label in labels):
        return "bulk_data"
    if any(label.startswith("auxiliary_model_data_") for label in labels):
        return "auxiliary_data"
    return None


def validate_domain(images: list[dict]) -> None:
    regular = [row for row in images if family(row) in ("primary", "variant")]
    keys = [(int(row["model"]), int(row["stage"])) for row in regular]
    expected = {(model, stage) for model in model_records() for stage, *_ in MODEL_PHASES}
    if len(keys) != len(set(keys)) or set(keys) != expected:
        raise CensusError("MODEL inventory must contain every regular model/stage exactly once")
    specifications = {stage: (offset, count) for stage, offset, count, _ in MODEL_PHASES}
    records = {model: index for index, model in enumerate(model_records())}
    for row in regular:
        offset, count = specifications[int(row["stage"])]
        if (int(row["sector_offset"]), int(row["sector_count"])) != (
                records[int(row["model"])] * 276 + offset, count):
            raise CensusError("MODEL physical slice disagrees with the loader record layout")
    counts = Counter(family(row) for row in images)
    if counts["special_battle"] != 2 or counts["intro_credits"] != 1:
        raise CensusError("MODEL inventory requires both special banks and the intro/credits image")
    if counts["bulk_data"] != 1242 or counts["auxiliary_data"] != 7:
        raise CensusError("MODEL data-load context is incomplete")


def body_groups(functions: list[dict], copies: Counter) -> list[dict]:
    groups = defaultdict(list)
    for row in functions:
        if row["classification"] == "registered_boundary" or row["cfg_closed"] == "1":
            groups[row["body_sha256"]].append(row)
    result = []
    for digest, rows in sorted(groups.items()):
        matched = [row for row in rows if row["reference_status"] == "matching_c"]
        registered = [row for row in rows if row["classification"] == "registered_boundary"]
        result.append({
            "body_sha256": digest,
            "bytes": int(rows[0]["size"] or rows[0]["observed_extent"]),
            "unique_image_sites": len(rows),
            "physical_site_occurrences": sum(copies[row["image_id"]] for row in rows),
            "known_matching_c_sites": len(matched),
            "registered_assembly_sites": sum(row["reference_status"] == "unmatched_asm" for row in rows),
            "unregistered_candidate_sites": len(rows) - len(registered),
            "classification": "has_matching_c_byte_identity" if matched else
                              "registered_without_matching_c" if registered else
                              "candidate_only_not_proven_function",
            "caveat": "exact_bytes_only_not_relocation_normalized_or_complete_image_acceptance",
        })
    return result


def generate(root: Path, census: Path, output: Path, region: str) -> dict:
    source = json.loads((census / "summary.json").read_text())
    if not source["complete"] or region not in source["regions"]:
        raise CensusError("source census is incomplete or missing the requested region")
    provenance = source["regions"][region]
    for name, digest in {**provenance["metadata"], **source["loader_sources"]}.items():
        if sha256_file(resolve_within(root, name, must_exist=True)) != digest:
            raise CensusError(f"{name}: census metadata is stale; regenerate at the desired cutoff")

    def read(kind):
        name = f"{region}-{kind}.csv"
        path = census / name
        if sha256_file(path) != source["report_files"][name]:
            raise CensusError(f"{name}: census artifact hash mismatch")
        with path.open(newline="", encoding="utf-8") as handle:
            return list(csv.DictReader(handle))

    selected = [row for row in read("images") if family(row) is not None]
    validate_domain(selected)
    code = [row for row in selected if row["load_kind"] == "code_load"]
    data = [row for row in selected if row["load_kind"] == "data_load"]
    if len(code) != 3729 or len(code) + len(data) != len(selected):
        raise CensusError("unexpected MODEL code/data load classification")
    copies = Counter(row["image_id"] for row in code)
    functions = [row for row in read("functions") if row["image_id"] in copies]
    coverage = {row["image_id"]: row for row in read("coverage") if row["image_id"] in copies}
    if set(coverage) != set(copies):
        raise CensusError("MODEL code images lack complete census byte accounting")
    by_image = defaultdict(list)
    for row in functions:
        by_image[row["image_id"]].append(row)
    groups = body_groups(functions, copies)
    matching_hashes = {row["body_sha256"] for row in groups if row["known_matching_c_sites"]}
    images = []
    for row in code:
        sites = by_image[row["image_id"]]
        entries = [site for site in sites if site["offset"] == "0x4"]
        entry = entries[0] if entries and family(row) != "intro_credits" else {}
        images.append({
            "family": family(row), **row,
            "entry_classification": entry.get("classification", ""),
            "entry_reference_status": entry.get("reference_status", ""),
            "entry_observed_extent": entry.get("observed_extent", ""),
            "entry_body_sha256": entry.get("body_sha256", ""),
            "entry_has_matching_c_byte_identity": int(bool(entry) and entry["body_sha256"] in matching_hashes),
            "known_matching_c_sites": sum(site["reference_status"] == "matching_c" for site in sites),
            "registered_assembly_sites": sum(site["reference_status"] == "unmatched_asm" for site in sites),
            "closed_candidate_sites": sum(site["classification"] == "candidate_cfg_closed" for site in sites),
            "unresolved_candidate_sites": sum(site["classification"] == "unresolved_candidate" for site in sites),
            "unassigned_bytes": coverage[row["image_id"]]["unassigned_bytes"],
            "caveat": "known_sites_may_be_shared_by_identical_payloads_not_registered_at_this_physical_location",
        })
    if output.exists():
        raise CensusError(f"{output}: preserve prior inventory; use a fresh output directory")
    output.mkdir(parents=True)
    for name, rows in (("images", images), ("data-loads", data),
                       ("function-sites", functions), ("body-groups", groups),
                       ("coverage", list(coverage.values()))):
        if not rows:
            raise CensusError(f"empty MODEL {name} inventory")
        write_csv(output / f"{name}.csv", list(rows[0]), rows)
    families = {}
    for label in ("primary", "variant", "special_battle", "intro_credits"):
        rows = [row for row in images if row["family"] == label]
        families[label] = {
            "physical_images": len(rows),
            "unique_loaded_payloads": len({row["image_id"] for row in rows}),
            "configured_physical_images": sum(bool(row["configured_modules"]) for row in rows),
            "entry_images_with_matching_c_byte_identity": sum(row["entry_has_matching_c_byte_identity"] for row in rows),
            "entry_classifications": dict(Counter(
                row["entry_classification"] or "no_single_entry_assumed" for row in rows)),
            "distinct_entry_hashes_with_matching_c": len({
                row["entry_body_sha256"] for row in rows if row["entry_has_matching_c_byte_identity"]}),
            "distinct_registered_assembly_entry_hashes": len({
                row["entry_body_sha256"] for row in rows if row["entry_reference_status"] == "unmatched_asm"}),
            "distinct_closed_entry_candidates_without_matching_c": len({
                row["entry_body_sha256"] for row in rows
                if row["entry_classification"] == "candidate_cfg_closed"
                and not row["entry_has_matching_c_byte_identity"]}),
        }
    summary = {
        "schema": 1, "region": region, "families": families,
        "model_id_domain": 722, "regular_records": 621,
        "excluded_regular_ids": [[300, 350], [650, 700], [720, 721]],
        "code_image_instances": len(images), "unique_loaded_payloads": len(copies),
        "configured_physical_images": sum(bool(row["configured_modules"]) for row in images),
        "unconfigured_physical_images": sum(not row["configured_modules"] for row in images),
        "data_load_instances": len(data),
        "function_site_classifications": dict(Counter(row["classification"] for row in functions)),
        "exact_body_groups": dict(Counter(row["classification"] for row in groups)),
        "physical_unassigned_bytes": sum(int(row["unassigned_bytes"]) for row in images),
        "unique_payload_unassigned_bytes": sum(int(row["unassigned_bytes"]) for row in coverage.values()),
        "scope": "all six regular MODEL record phases plus SU special-battle and intro/credits code loads",
        "limitations": [
            "A loaded image includes data; image bytes are not C instruction bytes.",
            "Unconfigured images may contain already-matching C bytes; registration is not code novelty.",
            "Candidate sites may overlap or be data/SDK code; they are not a proven remaining-function denominator.",
            "Byte-identical C bodies are reuse leads, not linked ownership or complete-image acceptance.",
            "Unassigned bytes include headers, literals, textures and potentially undiscovered code.",
            "Bulk and auxiliary data loads are retained separately, not silently classified as executable or excluded.",
            "This closes the documented loader image domain, not all possible dynamic execution or archive-code classification.",
        ],
        "source_census_sha256": sha256_file(census / "summary.json"),
        "source_census_generator_sha256": source["generator_sha256"],
        "generator_sha256": sha256_file(Path(__file__)),
        "inputs": provenance["inputs"], "metadata": provenance["metadata"],
        "loader_sources": source["loader_sources"],
        "report_files": {path.name: sha256_file(path) for path in sorted(output.glob("*.csv"))},
    }
    (output / "summary.json").write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
    return summary


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--census", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--region", default="france")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        summary = generate(root, resolve_within(root, args.census, must_exist=True),
                           resolve_within(root, args.output), args.region)
        print(json.dumps({key: summary[key] for key in (
            "families", "code_image_instances", "unique_loaded_payloads",
            "configured_physical_images", "unconfigured_physical_images",
            "function_site_classifications", "exact_body_groups",
        )}, indent=2))
        return 0
    except (CensusError, WorkspaceError, OSError, ValueError, KeyError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
