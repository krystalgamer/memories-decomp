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


if __name__ == "__main__":
    unittest.main()
