import csv
import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class FrenchResidentCoverageTests(unittest.TestCase):
    @staticmethod
    def inventory(region: str) -> list[dict]:
        with (ROOT / "config" / region / "functions.csv").open() as handle:
            return list(csv.DictReader(handle))

    def test_game_intervals_have_complete_classified_coverage(self) -> None:
        rows = self.inventory("sles_03948")
        regions = json.loads(
            (ROOT / "config/sles_03948/function_regions.json").read_text()
        )["regions"]
        for row in rows:
            address, size = int(row["address"], 0), int(row["size"], 0)
            owner = next(
                region for region in regions
                if int(region["start"], 0) <= address < int(region["end"], 0)
            )
            self.assertLessEqual(address + size, int(owner["end"], 0))
            self.assertEqual(row["module"], owner["module"])
            self.assertIn(
                row["status"],
                ("matching_c", "handwritten_asm") if owner["module"] == "game" else ("sdk_asm",),
            )
        for region in regions:
            if region["module"] != "game":
                continue
            cursor, end = int(region["start"], 0), int(region["end"], 0)
            for row in rows:
                address = int(row["address"], 0)
                if int(region["start"], 0) <= address < end:
                    self.assertEqual(address, cursor)
                    cursor += int(row["size"], 0)
            self.assertEqual(cursor, end)

    def test_only_verified_handwritten_extents_are_excluded_from_game_c(self) -> None:
        spanish = {row["address"]: row for row in self.inventory("sles_03951")}
        handwritten = [
            row for row in self.inventory("sles_03948")
            if row["module"] == "game" and row["status"] == "handwritten_asm"
        ]
        self.assertEqual(len(handwritten), 61)
        for row in handwritten:
            with self.subTest(address=row["address"]):
                reference = spanish[row["address"]]
                self.assertEqual(
                    (row["size"], row["status"], row["module"]),
                    (reference["size"], reference["status"], reference["module"]),
                )
                self.assertIn("Byte-identical", row["notes"])

    def test_every_eligible_function_is_in_the_matching_manifest(self) -> None:
        rows = self.inventory("sles_03948")
        manifest = json.loads(
            (ROOT / "config/sles_03948/matching_c.json").read_text()
        )["functions"]
        eligible = [
            row for row in rows
            if row["module"] == "game" and row["status"] != "handwritten_asm"
        ]
        self.assertEqual(len(eligible), 1140)
        self.assertEqual(sum(int(row["size"], 0) for row in eligible), 357700)
        self.assertEqual(
            {(row["address"], row["size"]) for row in eligible},
            {(row["address"], row["size"]) for row in manifest},
        )


if __name__ == "__main__":
    unittest.main()
