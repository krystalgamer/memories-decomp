from __future__ import annotations

import contextlib
import io
import json
from pathlib import Path
import sys
import unittest
from unittest import mock

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import overlay_extract
import progress
from function_inventory import load_inventory


class GermanOverlayTests(unittest.TestCase):
    def test_report_json_and_renderer_receive_full_german_metrics(self) -> None:
        metrics = {
            "target": "SLES-03949",
            "target_sha256": "german-hash",
            **progress.summarize_functions(
                load_inventory(REPOSITORY / "config/sles_03949/functions.csv"),
                progress.load_text_size(REPOSITORY, "config/sles_03949"),
            ),
            "overlays": progress.load_german_overlay_inventories(REPOSITORY),
        }
        calculations = (
            "calculate", "calculate_japanese", "calculate_european",
            "calculate_spanish", "calculate_italian", "calculate_german",
        )
        with (
            mock.patch.multiple(
                progress,
                **{name: mock.Mock(return_value=metrics) for name in calculations},
            ),
            mock.patch.object(progress, "parse_arguments", return_value=mock.Mock(check=True)),
            mock.patch.object(progress, "atomic_write_json") as write,
            mock.patch.object(progress, "sync_readme", return_value="current") as sync,
            contextlib.redirect_stdout(io.StringIO()),
        ):
            self.assertEqual(progress.main(), 0)
        self.assertEqual(write.call_args.args[1]["german"], metrics)
        self.assertEqual(sync.call_args.args[-1], metrics)
        self.assertEqual(metrics["matching_c_function_count"], 1140)
        self.assertEqual(len(metrics["overlays"]), 6)

    def test_accepted_resident_inventory_is_complete_and_unchanged(self) -> None:
        spanish = load_inventory(REPOSITORY / "config/sles_03951/functions.csv")
        german = load_inventory(REPOSITORY / "config/sles_03949/functions.csv")
        self.assertEqual(
            [(f.address, f.size, f.status, f.module) for f in german],
            [(f.address, f.size, f.status, f.module) for f in spanish],
        )
        metrics = progress.summarize_functions(
            german, progress.load_text_size(REPOSITORY, "config/sles_03949")
        )
        self.assertEqual(metrics["matching_c_function_count"], 1140)
        self.assertEqual(metrics["matching_c_bytes"], 357700)
        self.assertEqual(metrics["assembly_function_count"], 0)
        self.assertEqual(metrics["handwritten_function_count"], 61)
        self.assertEqual(metrics["sdk_function_count"], 623)
        progress.validate_regional_inventory(
            REPOSITORY, german,
            progress.load_image_map(REPOSITORY, "config/sles_03949"),
            config="config/sles_03949", region_name="German",
        )

    def test_all_instances_reuse_verified_sources(self) -> None:
        counts = progress.load_german_overlay_inventories(REPOSITORY)
        self.assertEqual(len(counts), 6)
        self.assertEqual(sum(row["matching_c_function_count"] for row in counts.values()), 124)
        self.assertEqual(sum(row["matching_c_bytes"] for row in counts.values()), 55852)
        _, spanish = overlay_extract.load_manifest(REPOSITORY, "spain")
        sector_size, german = overlay_extract.load_manifest(REPOSITORY, "germany")
        self.assertEqual(sector_size, 2048)
        self.assertEqual(len(german), 6)
        for original, current in zip(spanish, german):
            for field in ("sector_offset", "sector_count", "load_address", "sha256"):
                self.assertEqual(current[field], original[field])
            self.assertTrue(current["archive"].startswith("game/germany/"))
            name = current["name"].removeprefix("german_")
            old = REPOSITORY / f"config/sles_03951/overlays/{name}_matching_c.json"
            new = REPOSITORY / f"config/sles_03949/overlays/{name}_matching_c.json"
            self.assertEqual(json.loads(new.read_text()), json.loads(old.read_text()))

    def test_distinct_german_archive_hash_is_not_copied_from_spanish(self) -> None:
        _, modules = overlay_extract.load_manifest(REPOSITORY, "germany")
        wa = [module for module in modules if module["archive"].endswith("/WA_MRG.MRG")]
        self.assertEqual(len(wa), 5)
        for module in wa:
            self.assertEqual(
                module["archive_sha256"],
                "fbe294274a0c88fd70f1ea94a85ef6b2a6b5e4e9b5fd0c98eabdf687614d6fc6",
            )

    def test_inventory_disagreement_is_rejected(self) -> None:
        with (
            mock.patch.object(progress, "load_matching_ranges", return_value=[]),
            self.assertRaisesRegex(
                progress.ProgressError, "does not agree with the matching manifest"
            ),
        ):
            progress.load_german_overlay_inventories(REPOSITORY)

    def test_ci_does_not_depend_on_unmerged_resident_build(self) -> None:
        workflow = (REPOSITORY / ".github/workflows/german-overlay-build.yml").read_text()
        self.assertIn("uses: ./.github/actions/retail-inputs", workflow)
        self.assertIn("region: germany", workflow)
        self.assertIn('archives-only: "true"', workflow)
        for command in ("german-match-overlays", "german-verify-overlays"):
            self.assertIn(f"make {command}\n", workflow)
        self.assertNotIn("make german-match\n", workflow)
        self.assertNotIn("YGOFM_SLES_03949_URL", workflow)
        self.assertNotIn("YGOFM_ESP_", workflow)


if __name__ == "__main__":
    unittest.main()
