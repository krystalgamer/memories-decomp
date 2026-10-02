from pathlib import Path
from collections import Counter
import csv
import hashlib
import json
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import overlay_function_inventory as inventory


def words(*values):
    return struct.pack(f"<{len(values)}I", *values)


class OverlayFunctionInventoryTests(unittest.TestCase):
    def test_model_domain_and_six_disjoint_loader_phases(self):
        records = inventory.model_records()
        self.assertEqual(len(records), 621)
        self.assertEqual(records[:2], [0, 1])
        self.assertEqual(records[-1], 721)
        self.assertNotIn(300, records)
        self.assertNotIn(650, records)
        self.assertNotIn(720, records)
        self.assertEqual(records[319], 369)
        spans = [(offset * 2048, (offset + count) * 2048)
                 for _, offset, count, _ in inventory.MODEL_PHASES]
        self.assertEqual(inventory.complement(276 * 2048, spans),
                         [(0, 180 * 2048), (224 * 2048, 276 * 2048)])
        self.assertEqual(len(records) * len(spans), 3726)

    def test_complement_merges_duplicates_and_overlaps(self):
        self.assertEqual(inventory.complement(100, [(10, 30), (10, 30), (20, 50), (70, 100)]),
                         [(0, 10), (50, 70)])
        self.assertEqual(inventory.complement(4, []), [(0, 4)])
        for spans in ([(-1, 3)], [(2, 2)], [(0, 5)]):
            with self.assertRaises(inventory.CensusError):
                inventory.complement(4, spans)

    def test_return_delay_slot_is_part_of_extent(self):
        data = words(0x27BDFFF8, 0x03E00008, 0x27BD0008, 0xFFFFFFFF)
        result = inventory.walk_function(data, 0x8013B000, 0)
        self.assertEqual(result["visited"], {0, 4, 8})
        self.assertEqual(result["extent"], 12)
        self.assertTrue(result["closed"])

    def test_conditional_flow_retains_both_return_paths(self):
        data = words(0x10800003, 0, 0x03E00008, 0, 0x03E00008, 0)
        result = inventory.walk_function(data, 0x8013B000, 0)
        self.assertEqual(result["visited"], set(range(0, 24, 4)))
        self.assertEqual(result["returns"], 2)
        self.assertTrue(result["closed"])

    def test_unconditional_branch_does_not_invent_fallthrough(self):
        data = words(0x10000003, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0x03E00008, 0)
        result = inventory.walk_function(data, 0x8013B000, 0)
        self.assertEqual(result["visited"], {0, 4, 16, 20})
        self.assertFalse(result["closed"])

    def test_indirect_jump_is_unresolved_not_a_function_end(self):
        result = inventory.walk_function(words(0x01000008, 0), 0x8013B000, 0)
        self.assertEqual(result["problems"], {"indirect_jump@0x0", "no_reached_return"})
        self.assertFalse(result["closed"])

    def test_indirect_call_keeps_continuation(self):
        result = inventory.walk_function(words(0x0320F809, 0, 0x03E00008, 0), 0x8013B000, 0)
        self.assertEqual(result["indirect_calls"], 1)
        self.assertEqual(result["visited"], {0, 4, 8, 12})

    def test_invalid_and_nested_delay_instructions_are_reported(self):
        for data in (words(0xFFFFFFFF), words(0x03E00008),
                     words(0x03E00008, 0x03E00008)):
            result = inventory.walk_function(data, 0x8013B000, 0)
            self.assertTrue(result["problems"])
            self.assertFalse(result["closed"])

    def test_geometry_instructions_use_ps1_decoder(self):
        result = inventory.walk_function(words(0x4A180001, 0x03E00008, 0), 0x8013B000, 0)
        self.assertTrue(result["closed"])

    def test_newer_mips_integer_instructions_are_not_ps1_code(self):
        for word in (0x67BDFFF8, 0x0002103C, 0x50200001):
            result = inventory.walk_function(words(word, 0, 0x03E00008, 0), 0x8013B000, 0)
            self.assertIn("unsupported_instruction@0x0", result["problems"])
            self.assertFalse(result["closed"])

    def test_registered_extent_is_not_replaced_by_first_return(self):
        known = {0: {"size": 16, "name": "Known", "status": "unmatched_asm"}}
        rows = inventory.discover_functions(words(0x03E00008, 0, 0x03E00008, 0),
                                            0x8013B000, known, set(), set())
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0]["size"], 16)
        self.assertEqual(rows[0]["classification"], "registered_boundary")
        self.assertEqual(rows[0]["observed_extent"], 8)

    def test_candidates_do_not_claim_registered_size_or_c_status(self):
        rows = inventory.discover_functions(words(0x27BDFFF8, 0x03E00008, 0x27BD0008),
                                            0x8013B000, {}, set(), set())
        self.assertEqual(rows[0]["classification"], "candidate_cfg_closed")
        self.assertEqual(rows[0]["size"], "")
        self.assertEqual(rows[0]["reference_status"], "")

    def test_local_call_adds_leaf_without_stack_prologue(self):
        base = 0x8013B000
        call = 0x0C000000 | ((base + 16) >> 2 & 0x3FFFFFF)
        rows = inventory.discover_functions(words(call, 0, 0x03E00008, 0, 0x03E00008, 0),
                                            base, {}, {0}, set())
        self.assertEqual([row["offset"] for row in rows], ["0x0", "0x10"])
        self.assertIn("cfg_local_call", rows[1]["seed_evidence"])

    def test_prologue_scan_requires_alignment_and_frame_alignment(self):
        data = words(0x27BDFFF8, 0x27BDFFFF, 0x27BD0008)
        self.assertEqual(list(inventory.prologue_offsets(data)), [0])
        self.assertEqual(list(inventory.prologue_offsets(b"x" + words(0x27BDFFF8))), [])

    def test_regional_extra_loads_include_boot_special_and_all_duel_copies(self):
        for region in inventory.OVERLAY_MANIFESTS:
            pointers = dict.fromkeys(inventory.LOAD_POINTER_ADDRESSES, 0x80180000)
            pointers[0x800101DC] = 0x80154000 if region == "japan" else 0x80146000
            rows = list(inventory.extra_loads(region, pointers))
            self.assertEqual(len(rows), 20 if region in ("usa", "japan") else 29)
            self.assertEqual(sum(row[4].startswith("duel_terrain_") for row in rows), 7)
            self.assertEqual(sum(row[4].startswith("special_battle_") for row in rows), 2)
            self.assertEqual(sum(row[4].startswith("auxiliary_model_data_") for row in rows), 7)
            self.assertEqual(rows[0][1], 5827 if region == "usa" else 5810 if region == "japan" else 9509)
            menus = [row[1] for row in rows if row[4].startswith("main_menu_language_")]
            self.assertEqual(menus, [98] if region in ("usa", "japan") else [98, 234, 370, 506, 642])
            duel = [row[3] for row in rows if row[4].startswith("duel_terrain_")]
            self.assertEqual(duel, [pointers[0x800101DC]] * 7)
            names = [row[1] for row in rows if row[4] == "name_entry"]
            self.assertEqual(names, [7968 if region == "usa" else 7979 if region == "japan" else 9374])
            options = [row[1] for row in rows if row[4].startswith("options_language_")]
            self.assertEqual(options, [] if region in ("usa", "japan") else [10170, 10211, 10252, 10293, 10334])

    def test_load_pointers_are_read_from_each_actual_executable(self):
        data = bytearray(0x1000)
        data[:8] = b"PS-X EXE"
        struct.pack_into("<I", data, 0x18, 0x80010000)
        for address in inventory.LOAD_POINTER_ADDRESSES:
            struct.pack_into("<I", data, address - 0x80010000 + 0x800, 0x80180000)
        struct.pack_into("<I", data, 0x9DC, 0x80154000)
        self.assertEqual(inventory.load_pointers(data)[0x800101DC], 0x80154000)
        struct.pack_into("<I", data, 0x9DC, 0)
        with self.assertRaises(inventory.CensusError):
            inventory.load_pointers(data)


class OverlayInventorySnapshotTests(unittest.TestCase):
    directory = ROOT / "notes/overlays/function-inventory"

    def summary(self):
        return json.loads((self.directory / "summary.json").read_text())

    def test_complete_seven_release_snapshot_and_artifact_hashes(self):
        summary = self.summary()
        self.assertTrue(summary["complete"])
        self.assertEqual(summary["requested_regions"], sorted(inventory.OVERLAY_MANIFESTS))
        self.assertEqual(sorted(summary["regions"]), summary["requested_regions"])
        expected = {f"{region}-{kind}.csv" for region in summary["regions"]
                    for kind in ("images", "functions", "coverage", "data-hints", "unmapped", "unmapped-hints")}
        self.assertEqual(set(summary["report_files"]), expected)
        for name, digest in summary["report_files"].items():
            self.assertEqual(hashlib.sha256((self.directory / name).read_bytes()).hexdigest(), digest)
        for row in summary["regions"].values():
            self.assertEqual(len(row["inputs"]), 4)
            self.assertEqual(row["model_records"], 621)
            self.assertEqual(row["model_phase_instances"], 3726)
            self.assertEqual(row["model_bulk_data_load_instances"], 1242)
            self.assertEqual(row["code_load_instances"] + row["data_load_instances"], row["physical_images"])
        self.assertEqual(summary["regions"]["japan"]["load_pointers"]["0x800101DC"], "0x80154000")

    def test_function_candidates_never_become_registered_c_coverage(self):
        for region, expected in self.summary()["regions"].items():
            counts = Counter()
            with (self.directory / f"{region}-functions.csv").open(newline="") as handle:
                for row in csv.DictReader(handle):
                    counts[row["classification"]] += 1
                    self.assertEqual(row["load_kind"], "code_load")
                    if row["classification"] == "registered_boundary":
                        self.assertGreater(int(row["size"]), 0)
                        self.assertTrue(row["reference_status"])
                    else:
                        self.assertEqual(row["size"], "")
                        self.assertEqual(row["reference_status"], "")
                    if row["classification"] == "candidate_cfg_closed":
                        self.assertEqual(row["cfg_closed"], "1")
                        self.assertEqual(int(row["observed_extent"]), int(row["reachable_bytes"]))
                        self.assertEqual(row["unresolved"], "")
                    self.assertEqual(int(row["offset"], 0) % 4, 0)
            self.assertEqual(dict(counts), expected["unique_image_function_sites"])

    def test_every_code_image_has_explicit_complete_byte_accounting(self):
        for region, expected in self.summary()["regions"].items():
            code_images = {}
            physical = set()
            phases = set()
            with (self.directory / f"{region}-images.csv").open(newline="") as handle:
                for row in csv.DictReader(handle):
                    key = (row["archive"], row["sector_offset"], row["sector_count"], row["load_address"])
                    self.assertNotIn(key, physical)
                    physical.add(key)
                    if row["load_kind"] == "code_load":
                        code_images[row["image_id"]] = int(row["sector_count"]) * 2048
                    if row["model"] and row["stage"] != "0":
                        phases.add((int(row["model"]), int(row["stage"])))
            self.assertEqual(len(physical), expected["physical_images"])
            self.assertEqual(phases, {(model, stage) for model in inventory.model_records()
                                     for stage, *_ in inventory.MODEL_PHASES})
            covered = set()
            with (self.directory / f"{region}-coverage.csv").open(newline="") as handle:
                for row in csv.DictReader(handle):
                    identity = row["image_id"]
                    self.assertNotIn(identity, covered)
                    covered.add(identity)
                    self.assertEqual(int(row["image_bytes"]), code_images[identity])
                    values = [int(row[key]) for key in ("registered_function_bytes",
                                                        "additional_candidate_span_bytes", "unassigned_bytes")]
                    self.assertTrue(all(value >= 0 for value in values))
                    self.assertEqual(sum(values), code_images[identity])
                    ranges = [tuple(int(value, 0) for value in item.split(":"))
                              for item in row["unassigned_ranges"].split(";") if item]
                    self.assertEqual(sum(end - begin for begin, end in ranges), values[2])
                    for hint in row["unassigned_return_hints"].split(";"):
                        if hint:
                            self.assertTrue(any(begin <= int(hint, 0) < end for begin, end in ranges))
            self.assertEqual(covered, set(code_images))


if __name__ == "__main__":
    unittest.main()
