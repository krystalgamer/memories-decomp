from __future__ import annotations

import json
from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from classify_functions import ClassificationError
from function_inventory import Function, InventoryError, parse_generated_function_tree
from overlay_build import clear_generated_assembly
from regional_inventory import (
    instruction_shapes,
    load_matching_ranges,
    matching_functions,
    refresh_inventory,
)


class RegionalInventoryTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="regional-inventory-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.directory = Path(temporary.name)

    def test_manifest_rejects_overlap(self) -> None:
        manifest = self.directory / "matching_c.json"
        manifest.write_text(
            json.dumps(
                {
                    "schema": 1,
                    "functions": [
                        {"address": "0x1010", "size": "0x20"},
                        {"address": "0x1020", "size": "0x10"},
                    ],
                }
            ),
            encoding="utf-8",
        )
        with self.assertRaisesRegex(InventoryError, "overlapping"):
            load_matching_ranges(manifest)

    def test_compiled_name_requires_exact_elf_size(self) -> None:
        with self.assertRaisesRegex(InventoryError, "one exact-size"):
            matching_functions([(0x1000, 0x10)], {(0x1000, 0x14): ["example"]})

    def test_instruction_shape_masks_addresses_but_preserves_registers(self) -> None:
        assembly = self.directory / "asm"
        assembly.mkdir()
        (assembly / "first.s").write_text(
            "nonmatching first, 0x8\n"
            "glabel first\n"
            "/* 1000 80001000 3412028E */ lw $v0, 0x1234($s0)\n"
            "/* 1004 80001004 ABCD0208 */ j target\n",
            encoding="utf-8",
        )
        self.assertEqual(
            instruction_shapes(assembly, {"first"}),
            {"first": (0x8E020000, 0x08000000)},
        )

    def test_data_only_overlay_tail_is_not_an_invented_function(self) -> None:
        assembly = self.directory / "asm"
        assembly.mkdir()
        (assembly / "tail.s").write_text(
            "nonmatching gLocationPrev\n"
            "dlabel gLocationPrev\n"
            "/* 1618 80169618 00000000 */ .word 0\n"
            "enddlabel gLocationPrev\n",
            encoding="utf-8",
        )
        self.assertEqual(
            parse_generated_function_tree(assembly, require_functions=False), []
        )

    def test_overlay_split_removes_stale_raw_assembly(self) -> None:
        assembly = self.directory / "asm"
        assembly.mkdir()
        stale = assembly / "obsolete.s"
        stale.write_text("old function", encoding="utf-8")
        clear_generated_assembly(REPOSITORY, self.directory)
        self.assertFalse(stale.exists())
        self.assertFalse(assembly.exists())

    def test_refresh_preserves_documented_handwritten_classification(self) -> None:
        regions = [
            {
                "name": "game", "start": 0x1000, "end": 0x1020,
                "module": "game", "status": None,
            },
        ]
        generated = [Function(0x1000, 0x10, "first", "unmatched_asm")]
        matching = [Function(0x1010, 0x10, "second", "matching_c")]
        existing = [
            Function(0x1000, 0x10, "first", "handwritten_asm", "game", "Reviewed."),
            Function(0x1010, 0x10, "second", "matching_c", "game"),
        ]
        self.assertEqual(
            refresh_inventory(generated, matching, existing, regions=regions),
            existing,
        )

    def test_new_exact_match_replaces_stale_assembly_status_and_note(self) -> None:
        existing = [
            Function(
                0x1000, 4, "first", "unmatched_asm", "overlay/example",
                "Still unmatched.",
            )
        ]
        result = refresh_inventory(
            [], [Function(0x1000, 4, "First_Matched", "matching_c")],
            existing, module="overlay/example",
        )
        self.assertEqual(
            result, [Function(0x1000, 4, "First_Matched", "matching_c", "overlay/example")]
        )

    def test_refresh_rejects_function_crossing_ownership_region(self) -> None:
        regions = [
            {
                "name": "game", "start": 0x1000, "end": 0x1010,
                "module": "game", "status": None,
            },
            {
                "name": "sdk", "start": 0x1010, "end": 0x1020,
                "module": "psyq/sdk", "status": "sdk_asm",
            },
        ]
        with self.assertRaisesRegex(ClassificationError, "crosses region game"):
            refresh_inventory(
                [Function(0x1000, 0x18, "first", "unmatched_asm")],
                [], [], regions=regions,
            )

    def test_verified_handwritten_reference_rejects_changed_shape(self) -> None:
        regions = [
            {
                "name": "game", "start": 0x1000, "end": 0x1004,
                "module": "game", "status": None,
            },
        ]
        function = Function(0x1000, 4, "first", "unmatched_asm")
        reference = [
            Function(0x2000, 4, "first", "handwritten_asm", "game")
        ]
        with self.assertRaisesRegex(InventoryError, "instruction shapes differ"):
            refresh_inventory(
                [function], [], [],
                regions=regions,
                handwritten_reference=reference,
                handwritten_shapes={"first": (0x8E020000,)},
                generated_shapes={"first": (0x8E030000,)},
            )
        result = refresh_inventory(
            [function], [], [],
            regions=regions,
            handwritten_reference=reference,
            handwritten_shapes={"first": (0x8E020000,)},
            generated_shapes={"first": (0x8E020000,)},
        )
        self.assertEqual(result[0].status, "handwritten_asm")
        self.assertIn("0x00002000", result[0].notes)


if __name__ == "__main__":
    unittest.main()
