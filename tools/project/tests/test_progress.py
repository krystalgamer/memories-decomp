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

    def test_regional_progress_includes_japanese_target(self) -> None:
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
        )

        self.assertIn("North American (`SLUS-01411`)", rendered)
        self.assertIn("Japanese (`SLPM-86398`)", rendered)
        self.assertIn("`japanese-hash`", rendered)
        self.assertIn("**4 / 6 (66.67%)**", rendered)
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

if __name__ == "__main__":
    unittest.main()
