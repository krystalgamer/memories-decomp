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
        self.assertEqual(len(matched), 47)
        self.assertEqual(sum(int(row["size"], 0) for row in matched), 10084)
        self.assertEqual(sum(row["status"] == "unmatched_asm" for row in rows), 38)
        deferred = next(row for row in rows if row["address"] == "0x8014FABC")
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

    def test_small_helper_groups_have_exact_extents_and_definition_order(self) -> None:
        manifest = json.loads(
            (ROOT / "config/sles_03948/overlays/duel_effects_matching_c.json").read_text()
        )
        for name, start, end, count in (
            ("color_test", 0x8014D378, 0x8014D3E8, 2),
            ("rect_vertices", 0x8014F490, 0x8014F524, 1),
            ("quad_helpers", 0x8014FE00, 0x8014FF40, 2),
            ("projected_wrappers", 0x80151218, 0x801513F4, 2),
            ("matrix_setup", 0x801513F4, 0x801514BC, 1),
            ("matrix_helpers", 0x801514BC, 0x80151558, 2),
            ("packet_helpers", 0x80152EC4, 0x80153200, 4),
            ("color_transition", 0x80153F28, 0x80154084, 3),
            ("screen_draw", 0x801556F4, 0x801558F4, 3),
            ("layered_drawing", 0x801558F4, 0x80155F94, 3),
            ("drawing_tail", 0x80155F94, 0x80156448, 3),
            ("number_helpers", 0x80156AD4, 0x80156C40, 2),
            ("textured_quads", 0x80156C40, 0x80156E58, 2),
            ("primitive_draw", 0x80156E58, 0x801570B0, 2),
        ):
            with self.subTest(source=name):
                source = f"src/overlays/duel_effects/{name}.c"
                functions = [row for row in manifest["functions"] if row["source"] == source]
                names = []
                cursor = start
                for row in functions:
                    self.assertEqual(int(row["address"], 0), cursor)
                    names.append(f"func_{cursor:08X}")
                    cursor += int(row["size"], 0)
                self.assertEqual(len(functions), count)
                self.assertEqual(cursor, end)
                text = (ROOT / source).read_text()
                self.assertEqual(
                    re.findall(r"^(?:void|s32|u16) (func_[0-9A-F]+)\(", text, re.MULTILINE), names
                )
                self.assertNotRegex(text, r"\b(?:extern|asm|__asm__)\b")

    def test_spanish_drawing_groups_are_reused_without_regional_variants(self) -> None:
        manifests = [
            json.loads((ROOT / f"config/{region}/overlays/duel_effects_matching_c.json").read_text())
            for region in ("sles_03948", "sles_03951")
        ]
        for name, count in (("number_helpers", 2), ("primitive_draw", 2),
                            ("textured_quads", 2), ("packet_helpers", 4),
                            ("color_transition", 3),
                            ("screen_draw", 3), ("layered_drawing", 3),
                            ("drawing_tail", 3)):
            source = f"src/overlays/duel_effects/{name}.c"
            groups = [[row for row in manifest["functions"] if row["source"] == source]
                      for manifest in manifests]
            self.assertEqual(len(groups[0]), count)
            self.assertEqual(groups[0], groups[1])

    def test_color_word_binding_preserves_resident_ownership(self) -> None:
        region = ROOT / "config/sles_03948"
        binding = "D_8009B300 = 0x8009C688;"
        for path in ("link_symbols.ld", "overlays/duel_effects_linker_symbols.txt",
                     "overlays/duel_effects_symbols.txt"):
            with self.subTest(path=path):
                self.assertIn(binding, (region / path).read_text())
        self.assertIn(
            '#include "../../game/sorted_entry.h"',
            (ROOT / "src/overlays/duel_effects/color_helpers.h").read_text(),
        )

    def test_packet_declarations_have_one_shared_owner(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        headers = {name: (directory / name).read_text()
                   for name in ("packet_helpers.h", "drawing_helpers.h", "utility_helpers.h")}
        for symbol in ("D_8015B7F8", "D_8015B800", "func_80152EC4",
                       "func_80152F9C", "func_801530B0", "func_801531C4"):
            with self.subTest(symbol=symbol):
                owner = "packet_helpers.h" if symbol == "func_801531C4" else "drawing_helpers.h"
                for name, text in headers.items():
                    self.assertEqual(len(re.findall(rf"\b{symbol}\b", text)),
                                     1 if name == owner else 0)


if __name__ == "__main__":
    unittest.main()
