import csv
import hashlib
import json
from pathlib import Path
import re
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_extract import OverlayError, read_module


class DuplicateOverlayTests(unittest.TestCase):
    def setUp(self) -> None:
        temporary = tempfile.TemporaryDirectory(prefix="duplicate-overlay-", dir=ROOT / "tmp")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.archive = self.root / "archive.bin"
        self.archive.write_bytes(b"----abcd----abcd----abcd")
        self.module = {
            "name": "example",
            "archive": "archive.bin",
            "archive_sha256": hashlib.sha256(self.archive.read_bytes()).hexdigest(),
            "sector_offset": 1,
            "sector_count": 1,
            "duplicate_sector_offsets": [3, 5],
            "sha256": hashlib.sha256(b"abcd").hexdigest(),
            "output": "tmp/module.bin",
        }

    def test_all_copies_are_checked_without_changing_primary_output(self) -> None:
        output, payload = read_module(self.root, 4, self.module)
        self.assertEqual(output, self.root / "tmp/module.bin")
        self.assertEqual(payload, b"abcd")

    def test_existing_single_instance_manifest_still_works(self) -> None:
        del self.module["duplicate_sector_offsets"]
        self.assertEqual(read_module(self.root, 4, self.module)[1], b"abcd")

    def test_changed_last_copy_is_not_hidden_by_correct_primary(self) -> None:
        self.archive.write_bytes(b"----abcd----abcd----abce")
        self.module["archive_sha256"] = hashlib.sha256(self.archive.read_bytes()).hexdigest()
        with self.assertRaisesRegex(OverlayError, "sector 5 differs"):
            read_module(self.root, 4, self.module)

    def test_invalid_duplicate_locations_are_rejected(self) -> None:
        for offsets in (None, "3", [True], [-1], [1.5], [1], [3, 3], [6]):
            with self.subTest(offsets=offsets):
                self.module["duplicate_sector_offsets"] = offsets
                with self.assertRaises(OverlayError):
                    read_module(self.root, 4, self.module)


class FrenchDuelBankTests(unittest.TestCase):
    def test_loader_locations_cover_every_terrain_copy(self) -> None:
        modules = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())["modules"]
        bank = next(module for module in modules if module["name"] == "french_duel_effects")
        phase_order = [64, 5, 4, 5, 32, 1, 2, 32]
        expected = [0x1B88 + sum(phase_order) + terrain * 0xF0 for terrain in range(7)]
        self.assertEqual([bank["sector_offset"], *bank["duplicate_sector_offsets"]], expected)
        self.assertEqual(bank["sector_count"], 44)
        self.assertEqual(bank["load_address"], "0x80146000")
        self.assertEqual(
            bank["sha256"],
            "a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3",
        )

    def test_inventory_keeps_complete_text_and_unmatched_boundaries(self) -> None:
        directory = ROOT / "config/sles_03948/overlays"
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        cursor = 0x80146258
        for row in rows:
            self.assertEqual(int(row["address"], 0), cursor)
            cursor += int(row["size"], 0)
        self.assertEqual(cursor, 0x8015A1E4)
        self.assertEqual(len(rows), 85)
        matched = [row for row in rows if row["status"] == "matching_c"]
        self.assertEqual(len(matched), 15)
        self.assertEqual(sum(int(row["size"], 0) for row in matched), 2584)
        self.assertEqual(sum(row["status"] == "unmatched_asm" for row in rows), 70)
        deferred = next(row for row in rows if row["address"] == "0x8014F490")
        self.assertEqual(deferred["status"], "unmatched_asm")
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        self.assertEqual(
            {(row["address"], row["size"]) for row in matched},
            {(row["address"], row["size"]) for row in manifest["functions"]},
        )
        self.assertEqual({row["profile"] for row in manifest["functions"]}, {"gcc_2_8_1_g0_split"})

    def test_vector_group_preserves_complete_definition_order(self) -> None:
        directory = ROOT / "config/sles_03948/overlays"
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        source = "src/overlays/duel_effects/vector_init.c"
        functions = [row for row in manifest["functions"] if row["source"] == source]
        cursor = 0x8014F608
        names = []
        for row in functions:
            self.assertEqual(int(row["address"], 0), cursor)
            names.append(f"func_{cursor:08X}")
            cursor += int(row["size"], 0)
        self.assertEqual(len(functions), 5)
        self.assertEqual(cursor, 0x8014FABC)
        text = (ROOT / source).read_text()
        self.assertEqual(re.findall(r"^void (func_[0-9A-F]+)\(", text, re.MULTILINE), names)
        self.assertIn('#include "utility_helpers.h"', text)
        self.assertNotRegex(text, r"\b(?:extern|asm|__asm__)\b")


if __name__ == "__main__":
    unittest.main()
