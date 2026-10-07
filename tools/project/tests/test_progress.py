from __future__ import annotations

import json
from pathlib import Path
import sys
import unittest
from unittest import mock

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from function_inventory import Function, InventoryError, load_inventory
import progress


class ProgressInventoryTests(unittest.TestCase):
    def test_spanish_resident_inventory_agrees_with_matching_manifest(self) -> None:
        config = "config/sles_03951"
        functions = load_inventory(REPOSITORY / config / "functions.csv")
        progress.validate_regional_inventory(
            REPOSITORY, functions, progress.load_image_map(REPOSITORY, config),
            config=config, region_name="Spanish",
        )
        self.assertTrue(any(function.status == "matching_c" for function in functions))
        self.assertTrue(any(function.status == "handwritten_asm" for function in functions))
        self.assertTrue(any(function.status == "sdk_asm" for function in functions))

    def test_french_overlay_counts_follow_matching_manifests(self) -> None:
        overlays = progress.load_french_overlay_inventories(REPOSITORY)
        _, modules = progress.load_overlay_manifest(REPOSITORY, "france")
        self.assertEqual(
            set(overlays),
            {module["name"].removeprefix("french_") for module in modules},
        )
        self.assertEqual(len(overlays), 3594)
        self.assertEqual(sum(row["function_count"] == 0 for row in overlays.values()), 1742)
        self.assertEqual(sum(row["function_count"] for row in overlays.values()), 4201)
        self.assertEqual(sum(row["matching_c_function_count"] for row in overlays.values()), 3546)
        self.assertEqual(sum(row["matching_c_bytes"] for row in overlays.values()), 3968636)
        for name, counts in overlays.items():
            path = REPOSITORY / "config/sles_03948/overlays" / f"{name}_matching_c.json"
            functions = json.loads(path.read_text())["functions"]
            self.assertEqual(counts["matching_c_function_count"], len(functions))
            self.assertEqual(
                counts["matching_c_bytes"],
                sum(int(function["size"], 0) for function in functions),
            )

    def test_french_overlay_rejects_inventory_manifest_disagreement(self) -> None:
        with (
            mock.patch.object(progress, "load_matching_ranges", return_value=[]),
            self.assertRaisesRegex(
                progress.ProgressError, "does not agree with the matching manifest"
            ),
        ):
            progress.load_french_overlay_inventories(REPOSITORY)

    def test_spanish_overlay_counts_follow_matching_manifests(self) -> None:
        overlays = progress.load_spanish_overlay_inventories(REPOSITORY)
        _, modules = progress.load_overlay_manifest(REPOSITORY, "spain")
        self.assertEqual(len(overlays), 3596)
        self.assertEqual(sum(row["function_count"] == 0 for row in overlays.values()), 3187)
        self.assertEqual(sum(row["function_count"] for row in overlays.values()), 2631)
        self.assertEqual(sum(row["matching_c_function_count"] for row in overlays.values()), 1971)
        self.assertEqual(sum(row["matching_c_bytes"] for row in overlays.values()), 2760288)
        self.assertEqual(
            set(overlays),
            {module["name"].removeprefix("spanish_") for module in modules},
        )
        for name, counts in overlays.items():
            path = REPOSITORY / "config/sles_03951/overlays" / f"{name}_matching_c.json"
            functions = json.loads(path.read_text())["functions"]
            self.assertEqual(counts["matching_c_function_count"], len(functions))
            self.assertEqual(
                counts["matching_c_bytes"],
                sum(int(function["size"], 0) for function in functions),
            )

    def test_sdk_fragments_are_coalesced_to_authoritative_extent(self) -> None:
        inventory = [
            Function(0x1000, 0x30, "SdkEntry", "sdk_asm"),
        ]
        generated = [
            Function(0x1000, 0x10, "SdkEntry", "unmatched_asm"),
            Function(0x1010, 0x20, "local_helper", "unmatched_asm"),
        ]

        self.assertEqual(
            progress.coalesce_sdk_fragments(generated, inventory),
            [Function(0x1000, 0x30, "SdkEntry", "unmatched_asm")],
        )

    def test_game_fragments_remain_visible_to_validation(self) -> None:
        inventory = [
            Function(0x1000, 0x30, "GameEntry", "unmatched_asm"),
        ]
        generated = [
            Function(0x1000, 0x10, "GameEntry", "unmatched_asm"),
            Function(0x1010, 0x20, "local_helper", "unmatched_asm"),
        ]

        with self.assertRaisesRegex(
            progress.ProgressError,
            "inventory does not match",
        ):
            progress.validate_inventory(generated, inventory)

    def test_japanese_overlay_counts_follow_matching_manifests(self) -> None:
        overlays = progress.load_japanese_overlay_inventories(REPOSITORY)
        _, modules = progress.load_overlay_manifest(REPOSITORY, "japan")
        self.assertEqual(
            set(overlays),
            {module["name"].removeprefix("japanese_") for module in modules},
        )
        for name, counts in overlays.items():
            path = (
                REPOSITORY / "config/slpm_86398/overlays"
                / f"{name}_matching_c.json"
            )
            functions = json.loads(path.read_text(encoding="utf-8"))["functions"]
            inventory = load_inventory(
                path.with_name(f"{name}_functions.csv")
            )
            self.assertEqual(counts["function_count"], len(inventory))
            self.assertEqual(
                counts["function_bytes"], sum(function.size for function in inventory)
            )
            self.assertEqual(counts["matching_c_function_count"], len(functions))
            self.assertEqual(
                counts["matching_c_bytes"],
                sum(int(function["size"], 0) for function in functions),
            )
        self.assertEqual(overlays["password"]["function_count"], 27)
        self.assertEqual(overlays["password"]["matching_c_function_count"], 27)
        self.assertEqual(
            overlays["password"]["matching_c_bytes"],
            overlays["password"]["function_bytes"],
        )

    def test_japanese_resident_classifies_embedded_sdk_separately(self) -> None:
        inventory = load_inventory(
            REPOSITORY / "config/slpm_86398/functions.csv"
        )
        embedded = [
            function for function in inventory if function.address == 0x8005BD40
        ]
        self.assertEqual(
            [(function.size, function.status, function.module) for function in embedded],
            [(0x10, "sdk_asm", "psyq/sdk")],
        )
        self.assertTrue(
            any(function.status == "handwritten_asm" for function in inventory)
        )

    def test_european_overlay_counts_follow_matching_manifests(self) -> None:
        overlays = progress.load_european_overlay_inventories(REPOSITORY)
        _, modules = progress.load_overlay_manifest(REPOSITORY, "europe")
        self.assertEqual(
            set(overlays),
            {module["name"].removeprefix("european_") for module in modules},
        )
        self.assertIn("password_a", overlays)
        self.assertIn("password_b", overlays)
        for name, counts in overlays.items():
            with self.subTest(overlay=name):
                path = (
                    REPOSITORY / "config/sles_03947/overlays"
                    / f"{name}_matching_c.json"
                )
                functions = json.loads(path.read_text(encoding="utf-8"))["functions"]
                inventory = load_inventory(path.with_name(f"{name}_functions.csv"))
                self.assertEqual(counts["function_count"], len(inventory))
                self.assertEqual(
                    counts["function_bytes"],
                    sum(function.size for function in inventory),
                )
                self.assertEqual(counts["matching_c_function_count"], len(functions))
                self.assertEqual(
                    counts["matching_c_bytes"],
                    sum(int(function["size"], 0) for function in functions),
                )

    def test_european_overlay_rejects_inventory_manifest_disagreement(self) -> None:
        with (
            mock.patch.object(progress, "load_matching_ranges", return_value=[]),
            self.assertRaisesRegex(
                progress.ProgressError, "does not agree with the matching manifest"
            ),
        ):
            progress.load_european_overlay_inventories(REPOSITORY)

    def test_european_overlay_rejects_wrong_region_layout(self) -> None:
        module = {
            "name": "european_free_duel",
            "layout": "config/slpm_86398/overlays/free_duel.yaml",
        }
        with (
            mock.patch.object(
                progress, "load_overlay_manifest", return_value=(2048, [module])
            ),
            self.assertRaisesRegex(
                progress.ProgressError, "invalid European overlay layout"
            ),
        ):
            progress.load_european_overlay_inventories(REPOSITORY)

    def test_japanese_overlay_rejects_overlapping_matches(self) -> None:
        module = {
            "name": "japanese_free_duel",
            "layout": "config/slpm_86398/overlays/free_duel.yaml",
            "load_address": "0x80168000",
            "sector_count": 5,
        }
        manifest = {
            "schema": 1,
            "functions": [
                {"address": "0x80168004", "size": "0x20"},
                {"address": "0x80168014", "size": "0x20"},
            ],
        }
        with (
            mock.patch.object(
                progress, "load_overlay_manifest", return_value=(2048, [module])
            ),
            mock.patch.object(progress.json, "load", return_value=manifest),
            self.assertRaisesRegex(InventoryError, "overlapping"),
        ):
            progress.load_japanese_overlay_inventories(REPOSITORY)

    def test_japanese_overlay_rejects_missing_manifest_match(self) -> None:
        functions = [
            Function(0x1000, 0x10, "first", "matching_c", "overlay/example"),
        ]
        with self.assertRaisesRegex(
            progress.ProgressError, "does not agree with the matching manifest"
        ):
            progress.validate_overlay_inventory(
                functions,
                [(0x1000, 0x10), (0x1010, 0x10)],
                start=0x1000, end=0x2000, name="example",
            )

    def test_incomplete_sdk_fragments_remain_visible_to_validation(self) -> None:
        inventory = [
            Function(0x1000, 0x30, "SdkEntry", "sdk_asm"),
        ]
        generated = [
            Function(0x1000, 0x10, "SdkEntry", "unmatched_asm"),
            Function(0x1020, 0x10, "local_helper", "unmatched_asm"),
        ]

        with self.assertRaisesRegex(
            progress.ProgressError,
            "inventory does not match",
        ):
            progress.validate_inventory(generated, inventory)


class ProgressRenderingTests(unittest.TestCase):
    def test_matching_denominator_excludes_handwritten_assembly(self) -> None:
        rendered = progress.render_readme_progress(
            {
                "game_function_count": 12,
                "game_function_bytes": 0x180,
                "decompilation_target_function_count": 10,
                "decompilation_target_function_bytes": 0x100,
                "matching_c_function_count": 9,
                "matching_c_bytes": 0xE0,
                "assembly_function_count": 1,
                "assembly_function_bytes": 0x20,
                "handwritten_function_count": 2,
                "handwritten_function_bytes": 0x80,
                "sdk_function_count": 3,
                "sdk_function_bytes": 0x60,
                "function_count": 15,
                "unassigned_text_bytes": 0,
                "overlays": {},
            }
        )

        self.assertIn(
            "Game C-decompilation targets matched | "
            "**9 / 10 (90.00%)**",
            rendered,
        )
        self.assertIn(
            "Game C-decompilation target bytes matched | "
            "**224 (`0xE0`) / 256 (`0x100`) (87.50%)**",
            rendered,
        )
        self.assertIn("Total game-owned functions | 12", rendered)
        self.assertNotIn("**9 / 12", rendered)

    def test_regional_progress_includes_japanese_and_european_targets(self) -> None:
        rendered = progress.render_regional_progress(
            {
                "target_sha256": "north-american-hash",
                "game_function_count": 12,
                "decompilation_target_function_count": 10,
                "decompilation_target_function_bytes": 0x100,
                "matching_c_function_count": 9,
                "matching_c_bytes": 0xE0,
                "assembly_function_count": 1,
                "assembly_function_bytes": 0x20,
                "handwritten_function_count": 2,
                "handwritten_function_bytes": 0x80,
                "sdk_function_count": 3,
                "sdk_function_bytes": 0x60,
                "function_count": 15,
                "unassigned_text_bytes": 0,
                "overlays": {},
            },
            {
                "target_sha256": "japanese-hash",
                "text_bytes": 0x400,
                "function_count": 9,
                "game_function_count": 7,
                "decompilation_target_function_count": 6,
                "decompilation_target_function_bytes": 0x240,
                "handwritten_function_count": 1,
                "handwritten_function_bytes": 0x40,
                "assembly_function_count": 2,
                "assembly_function_bytes": 0x140,
                "sdk_function_count": 2,
                "sdk_function_bytes": 0x80,
                "matching_c_function_count": 4,
                "matching_c_bytes": 0x100,
                "unassigned_text_bytes": 0x100,
                "overlays": {
                    "free_duel": {
                        "function_count": 3,
                        "function_bytes": 0x40,
                        "matching_c_function_count": 2,
                        "matching_c_bytes": 0x30,
                    }
                },
            },
            {
                "target_sha256": "european-hash",
                "text_bytes": 0x500,
                "function_count": 11,
                "game_function_count": 8,
                "decompilation_target_function_count": 7,
                "decompilation_target_function_bytes": 0x300,
                "handwritten_function_count": 1,
                "handwritten_function_bytes": 0x40,
                "assembly_function_count": 3,
                "assembly_function_bytes": 0x180,
                "sdk_function_count": 3,
                "sdk_function_bytes": 0x100,
                "matching_c_function_count": 4,
                "matching_c_bytes": 0x180,
                "unassigned_text_bytes": 0xC0,
                "overlays": {
                    "password_a": {
                        "function_count": 27,
                        "function_bytes": 0x200,
                        "matching_c_function_count": 26,
                        "matching_c_bytes": 0x180,
                    },
                    "password_b": {
                        "function_count": 27,
                        "function_bytes": 0x200,
                        "matching_c_function_count": 26,
                        "matching_c_bytes": 0x180,
                    },
                },
            },
            {
                "target_sha256": "spanish-hash",
                **progress.summarize_functions(
                    [
                        Function(0x1000, 0x10, "Matched", "matching_c", "game"),
                        Function(0x1010, 0x10, "Pending", "unmatched_asm", "game"),
                        Function(0x1020, 0x10, "Handwritten", "handwritten_asm", "game"),
                        Function(0x1030, 0x10, "Sdk", "sdk_asm", "psyq/sdk"),
                    ],
                    0x40,
                ),
                "overlays": {
                    "free_duel": {
                        "function_count": 9,
                        "function_bytes": 4252,
                        "matching_c_function_count": 9,
                        "matching_c_bytes": 4252,
                    },
                },
            },
            progress.load_french_overlay_inventories(REPOSITORY),
            {
                "target_sha256": "italian-hash",
                **progress.summarize_functions(
                    load_inventory(REPOSITORY / "config/sles_03950/functions.csv"),
                    progress.load_text_size(REPOSITORY, "config/sles_03950"),
                ),
                "overlays": progress.load_italian_overlay_inventories(REPOSITORY),
            },
            {
                "target_sha256": "german-hash",
                **progress.summarize_functions(
                    load_inventory(REPOSITORY / "config/sles_03949/functions.csv"),
                    progress.load_text_size(REPOSITORY, "config/sles_03949"),
                ),
                "overlays": progress.load_german_overlay_inventories(REPOSITORY),
            },
        )

        self.assertIn("North American (`SLUS-01411`)", rendered)
        self.assertIn("Japanese (`SLPM-86398`)", rendered)
        self.assertIn("European (`SLES-03947`)", rendered)
        self.assertIn("French (`SLES-03948`)", rendered)
        italian = rendered.split("### Italian (`SLES-03950`)", 1)[1]
        self.assertIn("`italian-hash`", italian)
        self.assertIn("**1,140 / 1,140 (100.00%)**", italian)
        self.assertIn("`config/sles_03950/functions.csv`", italian)
        self.assertIn("| `main_menu` | 31 / 31 (100.00%) |", italian)
        german = rendered.split("### German (`SLES-03949`)", 1)[1]
        self.assertIn("`german-hash`", german)
        self.assertIn("**1,140 / 1,140 (100.00%)**", german)
        self.assertIn("`config/sles_03949/functions.csv`", german)
        self.assertIn("| `password_b` | 27 / 27 (100.00%) |", german)
        self.assertIn("`config/sles_03949/overlays/*_functions.csv`", german)
        french = rendered.split("### French (`SLES-03948`)", 1)[1]
        self.assertIn("Resident progress is not included here", french)
        self.assertIn("27 / 27 (100.00%)", french)
        self.assertIn("`japanese-hash`", rendered)
        self.assertIn("`european-hash`", rendered)
        self.assertIn("**4 / 6 (66.67%)**", rendered)
        self.assertIn("**4 / 7 (57.14%)**", rendered)
        self.assertIn(
            "**256 (`0x100`) / 576 (`0x240`) (44.44%)**", rendered
        )
        self.assertIn(
            "Preserved Psy-Q CRT/SDK assembly | "
            "2 functions, 128 (`0x80`)",
            rendered,
        )
        self.assertIn("Runtime overlay modules:", rendered)
        self.assertIn(
            "| `free_duel` | 2 / 3 (66.67%) | "
            "48 (`0x30`) / 64 (`0x40`) (75.00%) |",
            rendered,
        )
        self.assertNotIn("full function inventories not yet tracked", rendered)
        european = rendered.split("### European (`SLES-03947`)", 1)[1]
        self.assertIn("Runtime overlay modules:", european)
        for name in ("password_a", "password_b"):
            self.assertIn(
                f"| `{name}` | 26 / 27 (96.30%) | "
                "384 (`0x180`) / 512 (`0x200`) (75.00%) |",
                european,
            )
        self.assertIn("`config/sles_03947/overlays/*_functions.csv`", european)
        spanish = rendered.split("### Spanish (`SLES-03951`)", 1)[1]
        self.assertIn("`spanish-hash`", spanish)
        self.assertIn("**1 / 2 (50.00%)**", spanish)
        self.assertIn("`config/sles_03951/functions.csv`", spanish)
        self.assertNotIn("Resident matching is not yet configured", spanish)
        self.assertIn("`config/sles_03951/overlays/*_functions.csv`", spanish)
        self.assertIn("| `free_duel` | 9 / 9 (100.00%) |", spanish)

if __name__ == "__main__":
    unittest.main()
