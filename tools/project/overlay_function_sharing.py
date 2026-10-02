#!/usr/bin/env python3

"""Compare overlay byte bodies without promoting candidates or peer C status."""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import csv
from dataclasses import dataclass, replace
import hashlib
import itertools
import json
from pathlib import Path
import struct
import sys

from find_siblings import normalize_instruction
from hashing import sha256_file
from overlay_extract import OVERLAY_MANIFESTS
from workspace import WorkspaceError, require_workspace_root, resolve_within


class SharingError(RuntimeError):
    pass


@dataclass(frozen=True, slots=True)
class Site:
    region: str
    image_id: str
    offset: int
    size: int
    body: str
    evidence: str
    status: str
    name: str


def read_csv(path: Path):
    with path.open(encoding="utf-8", newline="") as handle:
        yield from csv.DictReader(handle)


def write_csv(path: Path, fields: list[str], rows):
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def align_boundaries(original: dict[str, list[Site]], image_ids: dict[str, set[str]]):
    registered: dict[str, dict[int, Site]] = defaultdict(dict)
    for region in sorted(original):
        for site in original[region]:
            if site.evidence != "local_registered":
                continue
            previous = registered[site.image_id].setdefault(site.offset, site)
            if (previous.size, previous.body) != (site.size, site.body):
                raise SharingError(f"conflicting registered boundary: {site.image_id} +{site.offset:#x}")
    for identity, entries in registered.items():
        offsets = sorted(entries)
        if any(left + entries[left].size > right for left, right in zip(offsets, offsets[1:])):
            raise SharingError(f"overlapping cross-release registered boundaries: {identity}")
    aligned = []
    adjustments = []
    for region in sorted(original):
        by_image: dict[str, dict[int, Site]] = defaultdict(dict)
        for site in original[region]:
            if site.offset in by_image[site.image_id]:
                raise SharingError(f"duplicate local site: {region} {site.image_id} +{site.offset:#x}")
            by_image[site.image_id][site.offset] = site
        for identity in sorted(image_ids[region]):
            known = registered[identity]
            local = by_image[identity]
            for offset, donor in sorted(known.items()):
                existing = local.get(offset)
                if existing and existing.evidence == "local_registered":
                    aligned.append(existing)
                else:
                    aligned.append(replace(donor, region=region, evidence="peer_registered", status=""))
            for offset, site in sorted(local.items()):
                if site.evidence != "closed_candidate":
                    continue
                overlaps = [start for start, owner in known.items()
                            if offset < start + owner.size and start < offset + site.size]
                if overlaps:
                    adjustments.append({
                        "region": region, "image_id": identity, "offset": f"0x{offset:X}",
                        "observed_extent": site.size, "body_sha256": site.body,
                        "registered_overlap_offsets": ";".join(f"0x{start:X}" for start in sorted(overlaps)),
                        "reason": "registered_span_preferred_for_comparison_only",
                    })
                else:
                    aligned.append(site)
    return aligned, adjustments


def fingerprints(data: bytes) -> tuple[str, str]:
    if len(data) % 4:
        raise SharingError("instruction body is not word aligned")
    words = [word for word, in struct.iter_unpack("<I", data)]
    masked = b"".join(struct.pack("<I", word & 0xFC000000 if word >> 26 in (2, 3) else word)
                      for word in words)
    shape = "\n".join(normalize_instruction(word) for word in words).encode()
    return hashlib.sha256(masked).hexdigest(), hashlib.sha256(shape).hexdigest()


def group_bodies(sites: list[Site]) -> dict[str, list[Site]]:
    groups = defaultdict(list)
    for site in sites:
        groups[site.body].append(site)
    for body, members in groups.items():
        if len({site.size for site in members}) != 1:
            raise SharingError(f"body fingerprint has inconsistent sizes: {body}")
    return dict(groups)


def reuse_sites(sites: list[Site]):
    donors = {}
    for site in sorted(sites, key=lambda row: (row.region, row.image_id, row.offset)):
        if site.status == "matching_c":
            donors.setdefault(site.body, site)
    return [(site, donors[site.body]) for site in sites
            if site.size > 16 and site.status != "matching_c" and site.body in donors]


def load_census(root: Path, census: Path):
    summary_path = census / "summary.json"
    summary = json.loads(summary_path.read_text())
    regions = summary["requested_regions"]
    if (summary.get("schema") != 1 or summary["complete"] is not True or
            sorted(summary["regions"]) != regions or len(regions) < 2 or
            not set(regions) <= OVERLAY_MANIFESTS.keys()):
        raise SharingError("census is incomplete or its region list disagrees")
    expected = {f"{region}-{kind}.csv" for region in regions for kind in
                ("images", "functions", "coverage", "data-hints", "unmapped", "unmapped-hints")}
    if set(summary["report_files"]) != expected:
        raise SharingError("census CSV manifest is incomplete")
    for name, digest in summary["report_files"].items():
        if sha256_file(census / name) != digest:
            raise SharingError(f"census CSV fingerprint mismatch: {name}")
    original = {}
    images = {}
    physical = {}
    image_ids = {}
    archives = {}
    unresolved = 0
    for region in regions:
        for name, digest in summary["regions"][region]["inputs"].items():
            if name.endswith(".MRG"):
                previous = archives.setdefault(name, digest)
                if previous != digest:
                    raise SharingError(f"contradictory archive fingerprint: {name}")
        image_ids[region] = set()
        physical[region] = {}
        for row in read_csv(census / f"{region}-images.csv"):
            if row["load_kind"] != "code_load":
                continue
            base = int(row["load_address"], 0)
            identity = row["image_id"]
            if (row["archive"] not in summary["regions"][region]["inputs"] or
                    not row["archive"].endswith(".MRG") or int(row["sector_offset"]) < 0 or
                    int(row["sector_count"]) <= 0 or not 0x80000000 <= base < 0x80200000 or base % 4):
                raise SharingError(f"invalid image load provenance: {region} {identity}")
            if identity != f"{base:08X}-{row['sha256']}":
                raise SharingError(f"invalid image identity: {identity}")
            image_ids[region].add(identity)
            images.setdefault((region, identity), row)
            key = (Path(row["archive"]).name, int(row["sector_offset"]), int(row["sector_count"]), base)
            if key in physical[region]:
                raise SharingError(f"duplicate physical code load: {region} {key}")
            physical[region][key] = identity
        original[region] = []
        for row in read_csv(census / f"{region}-functions.csv"):
            classification = row["classification"]
            if classification == "unresolved_candidate":
                unresolved += 1
                continue
            if classification not in ("registered_boundary", "candidate_cfg_closed"):
                raise SharingError(f"unknown census classification: {classification}")
            registered = classification == "registered_boundary"
            if not registered and (row["size"] or row["reference_status"]):
                raise SharingError("candidate claims registered size or C status")
            size = int(row["size"] if registered else row["observed_extent"])
            if not registered and (row["cfg_closed"] != "1" or row["unresolved"] or
                                   int(row["reachable_bytes"]) != size):
                raise SharingError("closed candidate has inconsistent control-flow evidence")
            offset = int(row["offset"], 0)
            image = images[(region, row["image_id"])]
            if offset < 0 or size <= 0 or offset % 4 or size % 4 or offset + size > int(image["sector_count"]) * 2048:
                raise SharingError(f"invalid function span: {region} {row['image_id']} +{offset:#x}")
            original[region].append(Site(
                region, row["image_id"], offset, size, row["body_sha256"],
                "local_registered" if registered else "closed_candidate", row["reference_status"], row["name"],
            ))
    for name, digest in sorted(archives.items()):
        if sha256_file(resolve_within(root, name, must_exist=True)) != digest:
            raise SharingError(f"retail archive fingerprint mismatch: {name}")
    return summary, original, images, image_ids, physical, archives, unresolved


def analyze(root: Path, census: Path, output: Path):
    if output.exists():
        raise SharingError(f"{output.relative_to(root)}: output already exists; use a fresh directory")
    source, original, images, image_ids, physical, archives, unresolved = load_census(root, census)
    aligned, adjustments = align_boundaries(original, image_ids)
    groups = group_bodies(aligned)
    if not groups:
        raise SharingError("census has no registered or closed-candidate bodies to compare")
    regions = sorted(original)
    output.mkdir(parents=True)
    report = {
        "schema": 1, "complete": False, "source_census": str(census.relative_to(root)),
        "source_summary_sha256": sha256_file(census / "summary.json"),
        "generator_sha256": sha256_file(Path(__file__)),
        "shape_helper_sha256": sha256_file(root / "tools/project/find_siblings.py"),
        "archive_sha256": archives, "regions": {}, "totals": {},
    }
    (output / "summary.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    body_rows = []
    jump_groups = defaultdict(set)
    shape_groups = defaultdict(set)
    for body, members in sorted(groups.items()):
        site = members[0]
        image = images[(site.region, site.image_id)]
        with resolve_within(root, image["archive"], must_exist=True).open("rb") as handle:
            handle.seek(int(image["sector_offset"]) * 2048)
            payload = handle.read(int(image["sector_count"]) * 2048)
        if hashlib.sha256(payload).hexdigest() != image["sha256"]:
            raise SharingError(f"representative image fingerprint mismatch: {site.image_id}")
        data = payload[site.offset:site.offset + site.size]
        if len(data) != site.size or hashlib.sha256(data).hexdigest() != body:
            raise SharingError(f"representative function fingerprint mismatch: {body}")
        jump, shape = fingerprints(data)
        jump_groups[jump].add(body)
        shape_groups[shape].add(body)
        body_rows.append({
            "body_sha256": body, "bytes": site.size, "regions": ";".join(sorted({row.region for row in members})),
            "sites": len(members), "local_registered_sites": sum(row.evidence == "local_registered" for row in members),
            "peer_registered_sites": sum(row.evidence == "peer_registered" for row in members),
            "closed_candidate_sites": sum(row.evidence == "closed_candidate" for row in members),
            "matching_c_sites": sum(row.status == "matching_c" for row in members),
            "representative_region": site.region, "representative_image": site.image_id,
            "representative_offset": f"0x{site.offset:X}", "jump_target_mask": jump, "instruction_shape": shape,
        })
    write_csv(output / "bodies.csv", list(body_rows[0]), body_rows)
    write_csv(output / "boundary-adjustments.csv",
              ["region", "image_id", "offset", "observed_extent", "body_sha256",
               "registered_overlap_offsets", "reason"], adjustments)
    reuse = reuse_sites(aligned)
    write_csv(output / "c-donor-sites.csv",
              ["region", "image_id", "offset", "bytes", "body_sha256", "boundary_evidence", "local_status",
               "donor_region", "donor_image_id", "donor_offset", "donor_name"],
              ({"region": site.region, "image_id": site.image_id, "offset": f"0x{site.offset:X}",
                "bytes": site.size, "body_sha256": site.body, "boundary_evidence": site.evidence,
                "local_status": site.status, "donor_region": donor.region, "donor_image_id": donor.image_id,
                "donor_offset": f"0x{donor.offset:X}", "donor_name": donor.name}
               for site, donor in sorted(reuse, key=lambda pair: (pair[0].region, -pair[0].size,
                                                                  pair[0].body, pair[0].image_id, pair[0].offset))))
    for name, clusters in (("jump-target-clusters.csv", jump_groups), ("instruction-shape-clusters.csv", shape_groups)):
        write_csv(output / name, ["fingerprint", "exact_body_variants", "regions", "sites", "body_sha256s"],
                  ({"fingerprint": signature, "exact_body_variants": len(keys),
                    "regions": ";".join(sorted({site.region for key in keys for site in groups[key]})),
                    "sites": sum(len(groups[key]) for key in keys), "body_sha256s": ";".join(sorted(keys))}
                   for signature, keys in sorted(clusters.items())))
    region_sets = {}
    for region in regions:
        sites = [site for site in aligned if site.region == region]
        targets = [site for site, _ in reuse if site.region == region]
        region_sets[region] = {site.body for site in sites}
        report["regions"][region] = {
            "physical_code_loads": len(physical[region]), "unique_code_images": len(image_ids[region]),
            "aligned_sites": len(sites), "exact_bodies": len(region_sets[region]),
            "boundary_evidence": dict(sorted(Counter(site.evidence for site in sites).items())),
            "nontrivial_c_donor_sites": len(targets), "distinct_c_donor_bodies": len({site.body for site in targets}),
            "c_donor_boundary_evidence": dict(sorted(Counter(site.evidence for site in targets).items())),
            "summed_donor_site_bytes_not_disjoint_coverage": sum(site.size for site in targets),
        }
    pairs = []
    for left, right in itertools.combinations(regions, 2):
        common_slots = physical[left].keys() & physical[right].keys()
        pairs.append({
            "left": left, "right": right,
            "shared_exact_bodies": len(region_sets[left] & region_sets[right]),
            "shared_whole_code_images": len(image_ids[left] & image_ids[right]),
            "same_archive_load_slots": len(common_slots),
            "byte_identical_load_slots": sum(physical[left][key] == physical[right][key] for key in common_slots),
            "whole_code_image_sets_equal": int(image_ids[left] == image_ids[right]),
            "physical_code_load_maps_equal": int(physical[left] == physical[right]),
        })
    write_csv(output / "region-pairs.csv", list(pairs[0]), pairs)
    registered = {site.body for site in aligned if site.evidence == "local_registered"}
    matching = {site.body for site in aligned if site.status == "matching_c"}
    report["totals"] = {
        "raw_eligible_sites": sum(len(sites) for sites in original.values()),
        "raw_exact_bodies": len({site.body for sites in original.values() for site in sites}),
        "aligned_sites": len(aligned), "exact_bodies": len(groups),
        "registered_exact_bodies": len(registered), "candidate_only_exact_bodies": len(groups.keys() - registered),
        "matching_c_exact_bodies": len(matching),
        "nontrivial_sites": sum(site.size > 16 for site in aligned),
        "nontrivial_exact_bodies": sum(members[0].size > 16 for members in groups.values()),
        "nontrivial_c_donor_sites": len(reuse), "distinct_c_donor_bodies": len({site.body for site, _ in reuse}),
        "jump_target_mask_clusters": len(jump_groups), "instruction_shape_clusters": len(shape_groups),
        "candidate_comparisons_replaced_or_excluded": len(adjustments),
        "unresolved_sites_excluded": unresolved,
    }
    report["report_files"] = {path.name: sha256_file(path) for path in sorted(output.glob("*.csv"))}
    report["complete"] = True
    (output / "summary.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    return report


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--census", default="notes/overlays/function-inventory")
    parser.add_argument("--output", default="tmp/overlay-function-sharing")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        report = analyze(root, resolve_within(root, args.census, must_exist=True),
                         resolve_within(root, args.output))
        print(json.dumps(report["totals"], indent=2))
        return 0
    except (SharingError, WorkspaceError, OSError, ValueError, KeyError, csv.Error) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
