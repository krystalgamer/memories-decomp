import csv
import json
from pathlib import Path
import re
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import register_model_images as registration
from progress import ProgressError, render_overlay_progress, validate_overlay_inventory


class ModelImageRegistrationTests(unittest.TestCase):
    def image(self, sector=0, address="0x8013b000", archive="MODEL.MRG", digest="a"):
        return dict(archive=archive, sector_offset=sector, sector_count=10,
                    load_address=address, sha256=digest)

    def test_grouping_requires_whole_payload_address_archive_and_size(self):
        rows = [self.image(), self.image(10), self.image(20, digest="b"),
                self.image(30, address="0x8017b000"), self.image(40, archive="SU.MRG"),
                {**self.image(50), "sector_count": 2}]
        groups = registration.missing_groups(rows, [])
        self.assertEqual(sorted(map(len, groups)), [1, 1, 1, 1, 2])

    def test_existing_layouts_and_aliases_are_not_replaced(self):
        module = {**self.image(), "duplicate_sector_offsets": [10], "layout": "accepted.yaml"}
        before = json.dumps(module)
        groups = registration.missing_groups([self.image(), self.image(10), self.image(20)], [module])
        self.assertEqual(groups, [[self.image(20)]])
        self.assertEqual(json.dumps(module), before)

    def test_conflicting_registrations_are_rejected(self):
        with self.assertRaisesRegex(registration.CensusError, "duplicate physical"):
            registration.registered_keys([self.image(), self.image()])
        with self.assertRaisesRegex(registration.CensusError, "disagrees"):
            registration.missing_groups([self.image()], [self.image(digest="wrong")])

    def test_manifest_append_preserves_existing_order_and_formatting(self):
        text = '{\n  "modules": [\n    { "z": 1, "a": 2 }\n  ],\n  "schema": 1\n}\n'
        result = registration.append_modules(text, [{"name": "new"}])
        self.assertIn('    { "z": 1, "a": 2 },\n', result)
        self.assertEqual(json.loads(result)["modules"], [{"z": 1, "a": 2}, {"name": "new"}])
        self.assertEqual(registration.append_modules(text, []), text)

    def test_all_seven_domains_have_complete_splat_coverage(self):
        total = 0
        for region in registration.OVERLAY_MANIFESTS:
            with self.subTest(region=region):
                directory = ROOT / "notes/overlays/model-inventory" / region
                with (directory / "images.csv").open(newline="") as handle:
                    images = list(csv.DictReader(handle))
                _, modules = registration.load_manifest(ROOT, region)
                self.assertEqual(len(images), 3729)
                self.assertEqual(registration.missing_groups(images, modules), [])
                mapped = registration.registered_keys(modules)
                for row in images:
                    module = mapped[registration.physical_key(row)]
                    self.assertTrue((ROOT / module["layout"]).is_file())
                    self.assertEqual(module["sha256"], row["sha256"])
                total += len(images)
        self.assertEqual(total, 26103)

    def test_baselines_do_not_promote_functions_or_disassemble_guessed_boundaries(self):
        count = 0
        for region, manifest in registration.OVERLAY_MANIFESTS.items():
            modules = json.loads((ROOT / manifest).read_text())["modules"]
            for module in modules:
                if not module["name"].startswith(registration.PREFIXES[region] + "model_image_"):
                    continue
                path = ROOT / module["layout"]
                if "linker_symbols" in module:
                    # Promoted in place; family-specific regressions verify the owners.
                    self.assertTrue(json.loads(path.with_name(
                        path.stem + "_matching_c.json").read_text())["functions"])
                    continue
                text = path.read_text()
                sha1 = re.search(r"^sha1: ([0-9a-f]{40})$", text, re.MULTILINE)
                self.assertIsNotNone(sha1)
                self.assertEqual(text, registration.layout_text(module, sha1[1]))
                self.assertEqual(
                    path.with_name(path.stem + "_functions.csv").read_text(),
                    ",".join(registration.FIELDS) + "\n",
                )
                self.assertEqual(json.loads(path.with_name(path.stem + "_matching_c.json").read_text()),
                                 {"schema": 1, "functions": []})
                count += 1
        self.assertGreater(count, 22000)

    def test_uninventoried_overlay_progress_has_no_fake_percentage(self):
        validate_overlay_inventory([], [], start=0x8013B000, end=0x80140000, name="raw")
        with self.assertRaisesRegex(ProgressError, "does not agree"):
            validate_overlay_inventory([], [(0x8013B004, 0x8013B00C)],
                                       start=0x8013B000, end=0x80140000, name="raw")
        text = "\n".join(render_overlay_progress({
            "raw": dict(function_count=0, function_bytes=0,
                        matching_c_function_count=0, matching_c_bytes=0),
            "known": dict(function_count=2, function_bytes=8,
                          matching_c_function_count=1, matching_c_bytes=4),
        }))
        self.assertIn("| Uninventoried layouts: 1 | Not inventoried | Unclassified |", text)
        self.assertIn("50.00%", text)
        self.assertNotIn("100.00%", text)

    def test_large_uninventoried_domains_keep_readme_tables_bounded(self):
        overlays = {
            f"raw_{index}": dict(function_count=0, function_bytes=0,
                                matching_c_function_count=0, matching_c_bytes=0)
            for index in range(3729)
        }
        lines = render_overlay_progress(overlays)
        self.assertEqual(len(lines), 6)
        self.assertIn("| Uninventoried layouts: 3,729 | Not inventoried | Unclassified |", lines)
        self.assertEqual(len(overlays), 3729)
