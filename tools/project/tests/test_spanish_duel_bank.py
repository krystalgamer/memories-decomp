import csv
import json
from pathlib import Path
import re
import sys
import unittest


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import progress


class SpanishDuelBankTests(unittest.TestCase):
    def test_all_seven_terrain_copies_are_registered(self) -> None:
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        bank = next(m for m in manifest["modules"] if m["name"] == "spanish_duel_effects")
        self.assertEqual(
            [bank["sector_offset"], *bank["duplicate_sector_offsets"]],
            [7193 + terrain * 240 for terrain in range(7)],
        )
        self.assertEqual(bank["sector_count"], 44)
        self.assertEqual(bank["load_address"], "0x80146000")
        self.assertEqual(bank["archive"], "game/spain/DATA/WA_MRG.MRG")
        self.assertEqual(
            bank["archive_sha256"],
            "00d79fecc9aadd11914dc9d4b79d24619f4ced86049670a3fc23f417ef6ecd8f",
        )
        self.assertEqual(
            bank["sha256"],
            "a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3",
        )

    def test_full_inventory_preserves_unmatched_boundaries(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 85)
        cursor = 0x80146258
        for row in rows:
            self.assertEqual(int(row["address"], 0), cursor)
            cursor += int(row["size"], 0)
        self.assertEqual(cursor, 0x8015A1E4)
        matched = [r for r in rows if r["status"] == "matching_c"]
        self.assertEqual(len(matched), 21)
        self.assertEqual(sum(int(r["size"], 0) for r in matched), 4084)
        self.assertEqual(sum(r["status"] == "unmatched_asm" for r in rows), 64)
        self.assertFalse(any(r["status"] in ("handwritten_asm", "sdk_asm") for r in rows))
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        self.assertEqual(
            {(r["address"], r["size"]) for r in matched},
            {(r["address"], r["size"]) for r in manifest["functions"]},
        )
        french = ROOT / "config/sles_03948/overlays/duel_effects_matching_c.json"
        accepted = {
            row["address"]: row for row in json.loads(french.read_text())["functions"]
        }
        original_sources = {
            f"src/overlays/duel_effects/{name}.c"
            for name in ("utility_helpers", "texture_words", "vector_init")
        }
        for row in manifest["functions"]:
            if row["source"] in original_sources:
                self.assertEqual(row, accepted[row["address"]])

    def test_reporting_does_not_hide_the_new_unmatched_bank(self) -> None:
        modules = progress.load_spanish_overlay_inventories(ROOT)
        self.assertEqual(len(modules), 7)
        self.assertEqual(modules["duel_effects"]["function_count"], 85)
        self.assertEqual(modules["duel_effects"]["matching_c_function_count"], 21)
        self.assertEqual(sum(m["function_count"] for m in modules.values()), 209)
        self.assertEqual(sum(m["matching_c_function_count"] for m in modules.values()), 145)
        self.assertEqual(sum(m["matching_c_bytes"] for m in modules.values()), 59936)

    def test_new_groups_preserve_exact_extents_and_definition_order(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        expected = {
            "number_helpers": [(0x80156AD4, 0x6C), (0x80156B40, 0x100)],
            "primitive_draw": [(0x80156E58, 0x14C), (0x80156FA4, 0x10C)],
        }
        for unit, extents in expected.items():
            source = f"src/overlays/duel_effects/{unit}.c"
            functions = [row for row in manifest["functions"] if row["source"] == source]
            self.assertEqual(
                [(int(row["address"], 0), int(row["size"], 0)) for row in functions],
                extents,
            )
            self.assertEqual({row["profile"] for row in functions}, {"gcc_2_8_1_g0_split"})
            definitions = re.findall(
                r"^(?:u16|void) (func_[0-9A-F]+)\(", (ROOT / source).read_text(), re.M
            )
            self.assertEqual(definitions, [f"func_{address:X}" for address, _ in extents])
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        for deferred in ("0x8014F490",):
            self.assertEqual(rows[deferred]["status"], "unmatched_asm")

    def test_drawing_bindings_reuse_resident_owners_without_data_aliases(self) -> None:
        directory = ROOT / "config/sles_03951"
        aliases = (directory / "overlays/duel_effects_linker_symbols.txt").read_text()
        resident = (directory / "symbols.txt").read_text() + (
            directory / "link_symbols.ld"
        ).read_text()
        for name, address in (
            ("RotAverage3", "0x800879D8"), ("RotAverage4", "0x80087A38"),
            ("GsSortPoly", "0x800842A8"), ("func_8005B260", "0x8004D5B8"),
        ):
            binding = f"{name} = {address};"
            self.assertIn(binding, aliases)
            self.assertIn(binding, resident)
        self.assertNotIn("D_8015B7F4 =", aliases)


if __name__ == "__main__":
    unittest.main()
