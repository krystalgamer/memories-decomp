from collections import Counter
import csv
import hashlib
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import model_overlay_inventory as inventory


def domain():
    rows = []
    for record, model in enumerate(inventory.model_records()):
        for stage, offset, count, _ in inventory.MODEL_PHASES:
            rows.append({
                "loader_evidence": "model_record", "model": str(model),
                "stage": str(stage), "sector_offset": str(record * 276 + offset),
                "sector_count": str(count),
            })
        for slot in (0, 1):
            rows.append({"loader_evidence": f"model_bulk_data_slot{slot}", "model": str(model)})
    rows.extend({"loader_evidence": f"special_battle_slot{slot}"} for slot in (0, 1))
    rows.append({"loader_evidence": "model_intro_credits"})
    rows.extend({"loader_evidence": f"auxiliary_model_data_{index}"} for index in range(7))
    return rows


class ModelOverlayInventoryTests(unittest.TestCase):
    def test_regular_domain_has_six_images_not_one_per_card(self):
        rows = domain()
        inventory.validate_domain(rows)
        counts = Counter(inventory.family(row) for row in rows)
        self.assertEqual(counts, {
            "primary": 1242, "variant": 2484, "special_battle": 2,
            "intro_credits": 1, "bulk_data": 1242, "auxiliary_data": 7,
        })

    def test_missing_duplicate_and_mislocated_images_are_rejected(self):
        for change in ("missing", "duplicate", "sector", "size", "special", "auxiliary",
                       "duplicate-special", "duplicate-auxiliary", "duplicate-bulk"):
            rows = domain()
            if change == "missing":
                rows.pop(0)
            elif change == "duplicate":
                rows.append(rows[0])
            elif change == "sector":
                rows[0]["sector_offset"] = "181"
            elif change == "size":
                rows[0]["sector_count"] = "9"
            elif change == "special":
                rows = [row for row in rows if row["loader_evidence"] != "special_battle_slot1"]
            elif change == "duplicate-special":
                next(row for row in rows if row["loader_evidence"] == "special_battle_slot1")[
                    "loader_evidence"] = "special_battle_slot0"
            elif change == "duplicate-auxiliary":
                rows[-1]["loader_evidence"] = rows[-2]["loader_evidence"]
            elif change == "duplicate-bulk":
                rows[7] = rows[6].copy()
            else:
                rows.pop()
            with self.subTest(change=change), self.assertRaises(inventory.CensusError):
                inventory.validate_domain(rows)

    def test_unrelated_overlays_are_not_model_images(self):
        self.assertIsNone(inventory.family({"loader_evidence": "main_menu_language_0"}))
        self.assertEqual(inventory.family({
            "loader_evidence": "french_exodia_slot0;special_battle_slot0",
        }), "special_battle")

    def test_byte_identity_leads_do_not_promote_candidate_sites(self):
        rows = []
        for image, status, classification, closed in (
            ("a", "matching_c", "registered_boundary", "1"),
            ("b", "", "candidate_cfg_closed", "1"),
            ("c", "", "unresolved_candidate", "0"),
        ):
            rows.append({
                "image_id": image, "reference_status": status,
                "classification": classification, "cfg_closed": closed,
                "body_sha256": "same", "size": "12" if status else "",
                "observed_extent": "12",
            })
        result, = inventory.body_groups(rows, Counter(a=2, b=3, c=4))
        self.assertEqual(result["unique_image_sites"], 2)
        self.assertEqual(result["physical_site_occurrences"], 5)
        self.assertEqual(result["known_matching_c_sites"], 1)
        self.assertEqual(result["unregistered_candidate_sites"], 1)
        self.assertEqual(result["classification"], "has_matching_c_byte_identity")
        self.assertEqual(rows[1]["reference_status"], "")

    def test_candidate_only_group_is_explicitly_unproven(self):
        result, = inventory.body_groups([{
            "image_id": "a", "reference_status": "", "classification": "candidate_cfg_closed",
            "cfg_closed": "1", "body_sha256": "unproven", "size": "", "observed_extent": "8",
        }], Counter(a=1))
        self.assertEqual(result["classification"], "candidate_only_not_proven_function")
        self.assertEqual(result["known_matching_c_sites"], 0)


class ModelInventorySnapshotTests(unittest.TestCase):
    directory = ROOT / "notes/overlays/model-inventory"

    def test_fixed_denominator_and_artifact_integrity(self):
        for region in inventory.OVERLAY_MANIFESTS:
            with self.subTest(region=region):
                directory = self.directory / region
                summary = json.loads((directory / "summary.json").read_text())
                self.assertEqual(summary["region"], region)
                self.assertEqual(summary["code_image_instances"], 3729)
                self.assertEqual(summary["data_load_instances"], 1249)
                self.assertEqual(summary["configured_physical_images"]
                                 + summary["unconfigured_physical_images"], 3729)
                for name, digest in summary["report_files"].items():
                    self.assertEqual(hashlib.sha256((directory / name).read_bytes()).hexdigest(), digest)
                with (directory / "images.csv").open(newline="") as handle:
                    images = list(csv.DictReader(handle))
                with (directory / "data-loads.csv").open(newline="") as handle:
                    data = list(csv.DictReader(handle))
                inventory.validate_domain(images + data)
                self.assertEqual(len(images), 3729)
                self.assertEqual(len({(row["archive"], row["sector_offset"], row["sector_count"],
                                       row["load_address"]) for row in images}), 3729)
                self.assertEqual(len({row["image_id"] for row in images}), summary["unique_loaded_payloads"])
                self.assertEqual(sum(int(row["unassigned_bytes"]) for row in images),
                                 summary["physical_unassigned_bytes"])
                self.assertEqual(sum(bool(row["configured_modules"]) for row in images),
                                 summary["configured_physical_images"])

    def test_cross_release_leads_keep_local_status_and_true_donors(self):
        directory = self.directory / "cross-release"
        summary = json.loads((directory / "summary.json").read_text())
        self.assertEqual(summary["regions"], sorted(inventory.OVERLAY_MANIFESTS))
        self.assertEqual(summary["physical_code_images"], 26103)
        donors = set()
        for region, digest in summary["regional_summary_sha256"].items():
            self.assertEqual(hashlib.sha256((self.directory / region / "summary.json").read_bytes()).hexdigest(),
                             digest)
            with (self.directory / region / "function-sites.csv").open(newline="") as handle:
                for row in csv.DictReader(handle):
                    if row["reference_status"] == "matching_c":
                        donors.add((region, row["image_id"], int(row["offset"], 0), row["body_sha256"]))
        for name, digest in summary["report_files"].items():
            self.assertEqual(hashlib.sha256((directory / name).read_bytes()).hexdigest(), digest)
        counts = Counter()
        with (directory / "peer-c-leads.csv").open(newline="") as handle:
            for row in csv.DictReader(handle):
                self.assertNotEqual(row["region"], row["donor_region"])
                self.assertNotEqual(row["local_status"], "matching_c")
                self.assertIn((row["donor_region"], row["donor_image_id"],
                               int(row["donor_offset"], 0), row["body_sha256"]), donors)
                counts[row["region"]] += 1
        self.assertEqual(dict(counts), summary["peer_c_lead_sites_by_region"])


if __name__ == "__main__":
    unittest.main()
