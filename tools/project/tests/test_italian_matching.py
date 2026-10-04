from __future__ import annotations

import csv
import hashlib
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

    def test_all_overlay_instances_reuse_verified_sources(self) -> None:
        counts = progress.load_italian_overlay_inventories(REPOSITORY)
        self.assertEqual(len(counts), 8)
        self.assertEqual(sum(row["matching_c_function_count"] for row in counts.values()), 132)
        self.assertEqual(sum(row["matching_c_bytes"] for row in counts.values()), 69292)
        _, spanish = overlay_extract.load_manifest(REPOSITORY, "spain")
        sector_size, italian = overlay_extract.load_manifest(REPOSITORY, "italy")
        self.assertEqual(sector_size, 2048)
        self.assertEqual(len(italian), len(counts))
        spanish_by_name = {
            module["name"].removeprefix("spanish_"): module for module in spanish
        }
        for current in italian:
            name = current["name"].removeprefix("italian_")
            self.assertIn(name, spanish_by_name)
            original = spanish_by_name[name]
            for field in ("sector_offset", "sector_count", "load_address", "sha256"):
                self.assertEqual(current[field], original[field])
            self.assertTrue(current["archive"].startswith("game/italy/"))
            original_manifest = REPOSITORY / f"config/sles_03951/overlays/{name}_matching_c.json"
            current_manifest = REPOSITORY / f"config/sles_03950/overlays/{name}_matching_c.json"
            current_mapping = json.loads(current_manifest.read_text())
            if name.startswith("model_variant_174_"):
                self.assertEqual(len(current_mapping["functions"]), 4)
                self.assertEqual(
                    [row["source"] for row in current_mapping["functions"]],
                    [
                        "src/overlays/french_model_variant/variant450_ribbons"
                        + ("_slot1.c" if name.endswith("slot1") else ".c"),
                        "src/overlays/french_model_variant/variant450_bands"
                        + ("_slot1.c" if name.endswith("slot1") else ".c"),
                        "src/overlays/spanish_model_variant/variant450_quads"
                        + ("_slot1.c" if name.endswith("slot1") else ".c"),
                        "src/overlays/spanish_model_variant/variant450_lines"
                        + ("_slot1.c" if name.endswith("slot1") else ".c"),
                    ],
                )
            else:
                self.assertEqual(
                    current_mapping,
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

    def test_model450_instances_and_terminal_attempts(self) -> None:
        with (
            REPOSITORY / "notes/overlays/italian-model-variant450-instances.csv"
        ).open() as handle:
            instances = list(csv.DictReader(handle))
        self.assertEqual(
            [(row["module"], row["sector_offset"], row["sha256"]) for row in instances],
            [
                (
                    "italian_model_variant_174_stage9_slot0",
                    "48224",
                    "b2c0697e759ecfe1e5d746ca7ad4bc29bcd3719c05893056acb877274dc6eac8",
                ),
                (
                    "italian_model_variant_174_stage10_slot1",
                    "48234",
                    "770befcc07901cfa9da9573421c5713062488ea83f6df218a25fe302d5c091b7",
                ),
            ],
        )
        with (
            REPOSITORY / "notes/overlays/italian-model-variant450-attempts.csv"
        ).open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 8)
        sources = {
            ("0xDD8", "0"): "src/overlays/french_model_variant/variant450_ribbons.c",
            ("0xDD8", "1"): "src/overlays/french_model_variant/variant450_ribbons_slot1.c",
            ("0x1794", "0"): "src/overlays/french_model_variant/variant450_bands.c",
            ("0x1794", "1"): "src/overlays/french_model_variant/variant450_bands_slot1.c",
            ("0x27C0", "0"): "src/overlays/spanish_model_variant/variant450_quads.c",
            ("0x27C0", "1"): "src/overlays/spanish_model_variant/variant450_quads_slot1.c",
            ("0x2D88", "0"): "src/overlays/spanish_model_variant/variant450_lines.c",
            ("0x2D88", "1"): "src/overlays/spanish_model_variant/variant450_lines_slot1.c",
        }
        for row in attempts:
            source = REPOSITORY / sources[(row["function_offset"], row["slot"])]
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["profile"], row["result"], row["different_words"]),
                             ("gcc_2_8_1_g0_split", "matched", "0"))

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
