from __future__ import annotations

from pathlib import Path
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from function_inventory import Function
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

if __name__ == "__main__":
    unittest.main()
