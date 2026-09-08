from __future__ import annotations

import argparse
from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from classify_functions import classify_function
from function_inventory import (
    Function,
    InventoryError,
    STATUSES,
    merge_inventory,
    parse_generated_functions,
    write_inventory,
)
from progress import render_readme_progress
from select_candidates import ATTEMPT_FIELDS, select


class FunctionInventoryTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        self.temporary = tempfile.TemporaryDirectory(
            prefix="function-inventory-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(self.temporary.cleanup)
        self.assembly = Path(self.temporary.name) / "functions.s"

    def parse(self, source: str) -> list[Function]:
        self.assembly.write_text(source, encoding="utf-8")
        return parse_generated_functions(self.assembly)

    def test_disassembler_hint_does_not_establish_source_provenance(self) -> None:
        for hint in ("", "/* Handwritten function */\n"):
            with self.subTest(hint=hint):
                functions = self.parse(
                    hint
                    + "nonmatching Example, 0x8\n"
                    + "/* 000800 80010000 0100184A */ rtps"
                    + " /* handwritten instruction */\n"
                    + "/* 000804 80010004 00000000 */ nop\n"
                )
                self.assertEqual(
                    functions,
                    [Function(0x80010000, 8, "Example", "unmatched_asm")],
                )

    def test_hinted_and_ordinary_functions_are_both_unclassified(self) -> None:
        functions = self.parse(
            "/* Handwritten function */\n"
            "nonmatching First, 0x4\n"
            "/* 000800 80010000 00000000 */ nop\n"
            "endnonmatching First\n"
            "nonmatching Second, 0x4\n"
            "/* 000804 80010004 00000000 */ nop\n"
        )
        self.assertEqual(
            functions,
            [
                Function(0x80010000, 4, "First", "unmatched_asm"),
                Function(0x80010004, 4, "Second", "unmatched_asm"),
            ],
        )

    def test_regeneration_preserves_every_reviewed_status_and_note(self) -> None:
        generated = self.parse(
            "/* Handwritten function */\n"
            "nonmatching Renamed, 0x4\n"
            "/* 000800 80010000 00000000 */ nop\n"
        )
        for status in sorted(STATUSES):
            with self.subTest(status=status):
                previous = Function(
                    0x80010000, 4, "OldName", status, "game", "Reviewed evidence"
                )
                self.assertEqual(
                    merge_inventory(generated, [previous]),
                    [
                        Function(
                            0x80010000, 4, "Renamed", status,
                            "game", "Reviewed evidence",
                        )
                    ],
                )

    def test_region_classification_still_identifies_sdk_assembly(self) -> None:
        function = self.parse(
            "/* Handwritten function */\n"
            "nonmatching Example, 0x4\n"
            "/* 000800 80010000 00000000 */ nop\n"
        )[0]
        region = [{
            "start": 0x80010000, "end": 0x80010004,
            "module": "psyq/sdk", "status": "sdk_asm",
        }]
        self.assertEqual(
            classify_function(function, region),
            Function(0x80010000, 4, "Example", "sdk_asm", "psyq/sdk"),
        )

    def test_game_region_keeps_reviewed_unmatched_classification(self) -> None:
        function = Function(
            0x80010000, 4, "Example", "unmatched_asm", "game",
            "GTE macro coverage is not handwritten-source evidence",
        )
        region = [{
            "start": 0x80010000, "end": 0x80010004,
            "module": "game", "status": None,
        }]
        self.assertEqual(classify_function(function, region), function)

    def test_only_unmatched_game_functions_enter_the_candidate_queue(self) -> None:
        root = Path(self.temporary.name)
        config = root / "config/slus_01411"
        write_inventory(config / "functions.csv", [
            Function(0x80010000, 0x20, "GteExample", "unmatched_asm", "game"),
            Function(0x80010020, 4, "Handwritten", "handwritten_asm", "game"),
            Function(0x80010024, 4, "SdkExample", "sdk_asm", "psyq/sdk"),
            Function(0x80010028, 8, "Ordinary", "unmatched_asm", "game"),
            Function(0x80010030, 4, "Matched", "matching_c", "game"),
        ])
        (config / "attempts.csv").write_text(
            ",".join(ATTEMPT_FIELDS) + "\n", encoding="utf-8"
        )
        args = argparse.Namespace(
            start=0, end=0x1_0000_0000, limit=30, include_partial=False,
        )
        self.assertEqual(select(root, args), [
            {"address": "0x80010028", "size": "0x8", "attempt_count": 0},
            {"address": "0x80010000", "size": "0x20", "attempt_count": 0},
        ])
        (config / "attempts.csv").write_text(
            ",".join(ATTEMPT_FIELDS) + "\n"
            "0x80010000,1,gcc-2.8.1-psx,profile=gcc_2_8_1_g0,"
            "nonmatch,Prior source did not match\n",
            encoding="utf-8",
        )
        self.assertEqual(select(root, args), [
            {"address": "0x80010028", "size": "0x8", "attempt_count": 0},
        ])
        args.include_partial = True
        self.assertEqual(select(root, args), [
            {"address": "0x80010028", "size": "0x8", "attempt_count": 0},
            {"address": "0x80010000", "size": "0x20", "attempt_count": 1},
        ])

    def test_progress_does_not_claim_unmatched_source_origin(self) -> None:
        rendered = render_readme_progress({
            "game_function_count": 3, "game_function_bytes": 24,
            "matching_c_function_count": 1, "matching_c_bytes": 8,
            "assembly_function_count": 2, "assembly_function_bytes": 16,
            "handwritten_function_count": 0, "handwritten_function_bytes": 0,
            "sdk_function_count": 0, "sdk_function_bytes": 0,
            "function_count": 3, "unassigned_text_bytes": 0,
        })
        self.assertIn("| Unmatched game assembly | 2 functions, 16 (`0x10`) |", rendered)
        self.assertIn("| Evidence-backed handwritten game assembly |", rendered)
        self.assertNotIn("compiler-generated", rendered)

    def test_missing_first_instruction_is_still_rejected(self) -> None:
        for tail in ("", "nonmatching Second, 0x4\n"):
            with self.subTest(tail=tail):
                with self.assertRaisesRegex(
                    InventoryError, "function First has no first instruction"
                ):
                    self.parse(
                        "/* Handwritten function */\n"
                        "nonmatching First, 0x4\n" + tail
                    )

    def test_hint_alone_is_not_a_function(self) -> None:
        self.assembly.write_text(
            "/* Handwritten function */\n", encoding="utf-8"
        )
        self.assertEqual(
            parse_generated_functions(self.assembly, require_functions=False), []
        )
        with self.assertRaisesRegex(InventoryError, "no generated functions"):
            parse_generated_functions(self.assembly)


if __name__ == "__main__":
    unittest.main()
