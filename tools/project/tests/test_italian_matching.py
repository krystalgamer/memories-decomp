from __future__ import annotations

import json
from pathlib import Path
import sys
import unittest
from unittest import mock

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from build_italian_baseline import ITALIAN_BUILD
from function_inventory import load_inventory
import overlay_extract
import progress


class ItalianMatchingTests(unittest.TestCase):
    def test_complete_inventory_preserves_spanish_classification(self) -> None:
        spanish = load_inventory(REPOSITORY / "config/sles_03951/functions.csv")
        italian = load_inventory(REPOSITORY / "config/sles_03950/functions.csv")
        self.assertEqual(len(italian), 1824)
        self.assertEqual(
            [(f.address, f.size, f.status, f.module) for f in italian],
            [(f.address, f.size, f.status, f.module) for f in spanish],
        )
        counts = progress.summarize_functions(
            italian, progress.load_text_size(REPOSITORY, "config/sles_03950")
        )
        self.assertEqual(counts["matching_c_function_count"], 1140)
        self.assertEqual(counts["matching_c_bytes"], 357700)
        self.assertEqual(counts["assembly_function_count"], 0)
        self.assertEqual(counts["handwritten_function_count"], 61)
        self.assertEqual(counts["sdk_function_count"], 623)
        progress.validate_regional_inventory(
            REPOSITORY, italian,
            progress.load_image_map(REPOSITORY, "config/sles_03950"),
            config="config/sles_03950", region_name="Italian",
        )

    def test_only_three_complete_source_groups_change(self) -> None:
        spanish = json.loads(
            (REPOSITORY / "config/sles_03951/matching_c.json").read_text()
        )["functions"]
        italian = json.loads(
            (REPOSITORY / "config/sles_03950/matching_c.json").read_text()
        )["functions"]
        self.assertEqual(len(italian), len(spanish))
        changed = []
        for original, current in zip(spanish, italian):
            self.assertEqual(
                {k: v for k, v in original.items() if k != "source"},
                {k: v for k, v in current.items() if k != "source"},
            )
            if original["source"] != current["source"]:
                self.assertEqual(
                    current["source"],
                    original["source"].replace("/spanish/", "/italian/"),
                )
                changed.append(current)
        self.assertEqual(len(changed), 7)
        self.assertEqual(sum(int(row["size"], 0) for row in changed), 2464)
        self.assertEqual(
            {Path(row["source"]).stem for row in changed},
            {"main_init", "debug_menu_sound_entry", "main_run_boot_sequence"},
        )
        for source in {row["source"] for row in changed}:
            original = (REPOSITORY / source.replace("/italian/", "/spanish/")).read_text()
            current = (REPOSITORY / source).read_text()
            self.assertEqual(
                current, original.replace("BUILD_LANGUAGE_INDEX 4", "BUILD_LANGUAGE_INDEX 3")
            )
            self.assertIn('#include "../../types.h"', current)
            self.assertNotIn("extern ", current)
            self.assertNotIn("asm", current)

    def test_inventoried_overlay_instances_reuse_verified_sources(self) -> None:
        counts = progress.load_italian_overlay_inventories(REPOSITORY)
        self.assertEqual(len(counts), 3584)
        self.assertEqual(sum(row["matching_c_function_count"] for row in counts.values()), 256)
        self.assertEqual(sum(row["matching_c_bytes"] for row in counts.values()), 142412)
        _, spanish = overlay_extract.load_manifest(REPOSITORY, "spain")
        _, french = overlay_extract.load_manifest(REPOSITORY, "france")
        sector_size, italian = overlay_extract.load_manifest(REPOSITORY, "italy")
        self.assertEqual(sector_size, 2048)
        self.assertEqual(len(italian), len(counts))
        italian = [m for m in italian if not m["name"].startswith("italian_model_image_")]
        self.assertEqual(len(italian), 10)
        spanish_by_name = {
            module["name"].removeprefix("spanish_"): module for module in spanish
        }
        french_by_name = {
            module["name"].removeprefix("french_"): module for module in french
        }
        for current in italian:
            name = current["name"].removeprefix("italian_")
            localized_menu = name.startswith("main_menu_language_")
            donors = french_by_name if localized_menu else spanish_by_name
            config = "sles_03948" if localized_menu else "sles_03951"
            self.assertIn(name, donors)
            original = donors[name]
            for field in ("sector_offset", "sector_count", "load_address", "sha256"):
                self.assertEqual(current[field], original[field])
            self.assertTrue(current["archive"].startswith("game/italy/"))
            original_manifest = REPOSITORY / f"config/{config}/overlays/{name}_matching_c.json"
            current_manifest = REPOSITORY / f"config/sles_03950/overlays/{name}_matching_c.json"
            self.assertEqual(
                json.loads(current_manifest.read_text()),
                json.loads(original_manifest.read_text()),
            )

    def test_overlay_inventory_disagreement_is_rejected(self) -> None:
        with (
            mock.patch.object(progress, "load_matching_ranges", return_value=[]),
            self.assertRaisesRegex(
                progress.ProgressError, "does not agree with the matching manifest"
            ),
        ):
            progress.load_italian_overlay_inventories(REPOSITORY)

    def test_build_and_ci_use_independent_target(self) -> None:
        self.assertEqual(ITALIAN_BUILD.matching_config, "config/sles_03950/matching_c.json")
        self.assertEqual(ITALIAN_BUILD.output_name, "SLES_039.50")
        self.assertEqual(ITALIAN_BUILD.expected_size, 0x1D0800)
        workflow = (REPOSITORY / ".github/workflows/italian-overlay-build.yml").read_text()
        self.assertIn("uses: ./.github/actions/retail-inputs", workflow)
        self.assertIn("region: italy", workflow)
        self.assertNotIn("archives-only:", workflow)
        for command in ("verify-italian-inputs", "italian-match", "italian-match-overlays"):
            self.assertIn(f"make {command}\n", workflow)
        self.assertNotIn("game/spain", workflow)
        self.assertNotIn("YGOFM_ESP_", workflow)


if __name__ == "__main__":
    unittest.main()
