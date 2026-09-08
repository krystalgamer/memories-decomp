from __future__ import annotations

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
    merge_inventory,
    parse_generated_functions,
)


class InventoryProvenanceTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="inventory-provenance-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.assembly = Path(temporary.name) / "generated.s"

    def parse(self, text: str) -> list[Function]:
        self.assembly.write_text(text, encoding="utf-8")
        return parse_generated_functions(self.assembly)

    def test_generated_handwritten_hint_is_not_source_provenance(self) -> None:
        functions = self.parse(
            "/* Handwritten function */\n"
            "nonmatching func_80010000, 0x4\n"
            "glabel func_80010000\n"
            "/* 800 80010000 00000248 */ mfc2 $v0, $0\n"
            "nonmatching func_80010004, 0x4\n"
            "glabel func_80010004\n"
            "/* 804 80010004 00000000 */ nop\n"
        )
        self.assertEqual(
            functions,
            [
                Function(0x80010000, 4, "func_80010000", "unmatched_asm"),
                Function(0x80010004, 4, "func_80010004", "unmatched_asm"),
            ],
        )

    def test_explicit_classification_survives_inventory_refresh(self) -> None:
        generated = [
            Function(0x80010000, 4, "func_80010000", "unmatched_asm"),
            Function(0x80010004, 4, "func_80010004", "unmatched_asm"),
        ]
        existing = [
            Function(
                0x80010000, 4, "func_80010000", "handwritten_asm", "game",
                "Independently documented source-assembly evidence.",
            ),
            Function(
                0x80010004, 4, "func_80010004", "sdk_asm", "psyq/sdk",
                "Verified SDK ownership.",
            ),
            Function(
                0x80010008, 4, "func_80010008", "matching_c", "game",
                "Matching C is absent from the generated assembly.",
            ),
        ]
        self.assertEqual(merge_inventory(generated, existing), existing)

    def test_sdk_region_classification_remains_separate(self) -> None:
        function = Function(0x80010000, 4, "func_80010000", "unmatched_asm")
        region = {
            "start": 0x80010000,
            "end": 0x80010004,
            "module": "psyq/sdk",
            "status": "sdk_asm",
        }
        classified = classify_function(function, [region])
        self.assertEqual(classified.status, "sdk_asm")
        self.assertEqual(classified.module, "psyq/sdk")

    def test_annotation_does_not_hide_a_missing_instruction(self) -> None:
        with self.assertRaisesRegex(InventoryError, "has no first instruction"):
            self.parse(
                "/* Handwritten function */\n"
                "nonmatching func_80010000, 0x4\n"
                "glabel func_80010000\n"
            )


if __name__ == "__main__":
    unittest.main()
