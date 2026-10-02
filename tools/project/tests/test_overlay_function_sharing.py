from dataclasses import replace
import csv
import hashlib
import json
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import overlay_function_sharing as sharing


def site(region="a", image="same-image-and-base", offset=4, size=16, body="body",
         evidence="local_registered", status="matching_c", name="Known"):
    return sharing.Site(region, image, offset, size, body, evidence, status, name)


class OverlayFunctionSharingTests(unittest.TestCase):
    def test_peer_boundary_does_not_promote_target_c_status(self):
        original = {"a": [site()], "b": [site("b", size=8, body="prefix", evidence="closed_candidate", status="")]}
        aligned, adjustments = sharing.align_boundaries(original, {"a": {"same-image-and-base"},
                                                                   "b": {"same-image-and-base"}})
        target, = [row for row in aligned if row.region == "b"]
        self.assertEqual((target.size, target.body, target.evidence, target.status), (16, "body", "peer_registered", ""))
        self.assertEqual(len(adjustments), 1)

    def test_unresolved_or_missing_seed_can_borrow_proven_image_boundary(self):
        aligned, _ = sharing.align_boundaries({"a": [site()], "b": []},
                                              {"a": {"same-image-and-base"}, "b": {"same-image-and-base"}})
        self.assertEqual(len(aligned), 2)
        self.assertEqual(aligned[1].evidence, "peer_registered")

    def test_different_payload_or_load_base_prevents_boundary_sharing(self):
        aligned, _ = sharing.align_boundaries({"a": [site()], "b": []},
                                              {"a": {"same-image-and-base"}, "b": {"different-image-or-base"}})
        self.assertEqual(len(aligned), 1)

    def test_different_names_and_statuses_do_not_change_boundaries(self):
        original = {"a": [site()], "b": [site("b", status="unmatched_asm", name="OtherName")]}
        aligned, adjustments = sharing.align_boundaries(original, {"a": {"same-image-and-base"},
                                                                   "b": {"same-image-and-base"}})
        self.assertEqual(aligned[1].status, "unmatched_asm")
        self.assertEqual(aligned[1].name, "OtherName")
        self.assertEqual(adjustments, [])

    def test_contradictory_registered_sizes_or_bytes_fail(self):
        for other in (site("b", size=20), site("b", body="different")):
            with self.assertRaises(sharing.SharingError):
                sharing.align_boundaries({"a": [site()], "b": [other]},
                                         {"a": {"same-image-and-base"}, "b": {"same-image-and-base"}})

    def test_overlapping_registered_boundaries_fail(self):
        with self.assertRaises(sharing.SharingError):
            sharing.align_boundaries({"a": [site()], "b": [site("b", offset=8)]},
                                     {"a": {"same-image-and-base"}, "b": {"same-image-and-base"}})

    def test_candidate_overlap_is_retained_in_adjustment_report(self):
        inside = site("b", offset=8, size=8, body="inner", evidence="closed_candidate", status="")
        outside = replace(inside, offset=24, body="outside")
        aligned, adjustments = sharing.align_boundaries({"a": [site()], "b": [inside, outside]},
                                                        {"a": {"same-image-and-base"}, "b": {"same-image-and-base"}})
        self.assertEqual(len(aligned), 3)
        self.assertEqual(adjustments[0]["offset"], "0x8")
        self.assertEqual(adjustments[0]["registered_overlap_offsets"], "0x4")
        self.assertEqual(aligned[-1], outside)

    def test_jump_mask_preserves_other_words_but_can_hide_callee_changes(self):
        def words(call, value=7):
            return struct.pack("<4I", call, 0, 0x24020000 | value, 0x03E00008)
        self.assertEqual(sharing.fingerprints(words(0x0C012345))[0],
                         sharing.fingerprints(words(0x0C054321))[0])
        self.assertNotEqual(sharing.fingerprints(words(0x0C012345))[0],
                            sharing.fingerprints(words(0x0C012345, 8))[0])
        self.assertNotEqual(sharing.fingerprints(words(0x08012345))[0],
                            sharing.fingerprints(words(0x0C012345))[0])
        with self.assertRaises(sharing.SharingError):
            sharing.fingerprints(b"x")

    def test_reuse_requires_an_actual_c_donor_and_nontrivial_size(self):
        donor = site(size=20)
        target = replace(donor, region="b", status="", evidence="peer_registered")
        self.assertEqual(sharing.reuse_sites([donor, target]), [(target, donor)])
        self.assertEqual(sharing.reuse_sites([target]), [])
        self.assertEqual(sharing.reuse_sites([site(), replace(site(), region="b", status="")]), [])

    def test_equal_hashes_with_different_sizes_fail(self):
        with self.assertRaises(sharing.SharingError):
            sharing.group_bodies([site(), site(size=20)])

    def test_duplicate_local_sites_fail(self):
        with self.assertRaises(sharing.SharingError):
            sharing.align_boundaries({"a": [site(), site()]}, {"a": {"same-image-and-base"}})


class OverlaySharingSnapshotTests(unittest.TestCase):
    directory = ROOT / "notes/overlays/function-sharing"

    def test_complete_snapshot_and_csv_fingerprints(self):
        summary = json.loads((self.directory / "summary.json").read_text())
        self.assertTrue(summary["complete"])
        self.assertEqual(len(summary["regions"]), 7)
        self.assertEqual(len(summary["report_files"]), 6)
        for name, digest in summary["report_files"].items():
            self.assertEqual(hashlib.sha256((self.directory / name).read_bytes()).hexdigest(), digest)
        totals = summary["totals"]
        self.assertEqual((totals["aligned_sites"], totals["exact_bodies"]), (39389, 6044))
        self.assertEqual(totals["registered_exact_bodies"] + totals["candidate_only_exact_bodies"], 6044)
        self.assertEqual((totals["jump_target_mask_clusters"], totals["instruction_shape_clusters"]), (2484, 1402))
        self.assertEqual(totals["nontrivial_c_donor_sites"], 2889)

    def test_localized_pal_images_and_load_maps_are_identical(self):
        localized = {"france", "spain", "germany", "italy"}
        count = 0
        with (self.directory / "region-pairs.csv").open(newline="") as handle:
            for row in csv.DictReader(handle):
                if {row["left"], row["right"]} <= localized:
                    count += 1
                    self.assertEqual(row["shared_exact_bodies"], "1555")
                    self.assertEqual(row["shared_whole_code_images"], "3587")
                    self.assertEqual(row["byte_identical_load_slots"], "3752")
                    self.assertEqual(row["whole_code_image_sets_equal"], "1")
                    self.assertEqual(row["physical_code_load_maps_equal"], "1")
        self.assertEqual(count, 6)

    def test_donor_targets_keep_local_status_and_byte_identity(self):
        with (self.directory / "bodies.csv").open(newline="") as handle:
            donors = {row["body_sha256"]: row for row in csv.DictReader(handle)
                      if int(row["matching_c_sites"]) > 0}
        count = 0
        with (self.directory / "c-donor-sites.csv").open(newline="") as handle:
            for row in csv.DictReader(handle):
                count += 1
                self.assertGreater(int(row["bytes"]), 16)
                self.assertNotEqual(row["local_status"], "matching_c")
                if row["boundary_evidence"] == "peer_registered":
                    self.assertEqual(row["local_status"], "")
                self.assertEqual(row["bytes"], donors[row["body_sha256"]]["bytes"])
                self.assertTrue(row["donor_name"])
        self.assertEqual(count, 2889)


if __name__ == "__main__":
    unittest.main()
