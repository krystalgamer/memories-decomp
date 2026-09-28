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
        self.assertEqual(len(matched), 40)
        self.assertEqual(sum(int(r["size"], 0) for r in matched), 7564)
        self.assertEqual(sum(r["status"] == "unmatched_asm" for r in rows), 45)
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
        shared_sources = {
            f"src/overlays/duel_effects/{name}.c"
            for name in (
                "utility_helpers", "texture_words", "vector_init",
                "color_test", "quad_helpers", "matrix_helpers",
            )
        }
        for row in manifest["functions"]:
            if row["source"] in shared_sources:
                self.assertEqual(row, accepted[row["address"]])

    def test_reporting_does_not_hide_the_new_unmatched_bank(self) -> None:
        modules = progress.load_spanish_overlay_inventories(ROOT)
        self.assertEqual(len(modules), 7)
        self.assertEqual(modules["duel_effects"]["function_count"], 85)
        self.assertEqual(modules["duel_effects"]["matching_c_function_count"], 40)
        self.assertEqual(sum(m["function_count"] for m in modules.values()), 209)
        self.assertEqual(sum(m["matching_c_function_count"] for m in modules.values()), 164)
        self.assertEqual(sum(m["matching_c_bytes"] for m in modules.values()), 63416)

    def test_new_groups_preserve_exact_extents_and_definition_order(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        expected = {
            "drawing_tail": [
                (0x80155F94, 0xD0), (0x80156064, 0x108), (0x8015616C, 0x2DC),
            ],
            "color_transition": [
                (0x80153F28, 0x70), (0x80153F98, 0xC4), (0x8015405C, 0x28),
            ],
            "screen_draw": [
                (0x801556F4, 0xA4), (0x80155798, 0xCC), (0x80155864, 0x90),
            ],
            "packet_helpers": [
                (0x80152EC4, 0xD8), (0x80152F9C, 0x114),
                (0x801530B0, 0x114), (0x801531C4, 0x3C),
            ],
            "color_test": [(0x8014D378, 0x34), (0x8014D3AC, 0x3C)],
            "quad_helpers": [(0x8014FE00, 0xD4), (0x8014FED4, 0x6C)],
            "matrix_helpers": [(0x801514BC, 0x3C), (0x801514F8, 0x60)],
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
                r"^(?:s32|u16|void) (func_[0-9A-F]+)\(", (ROOT / source).read_text(), re.M
            )
            self.assertEqual(definitions, [f"func_{address:X}" for address, _ in extents])
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        for deferred in ("0x8014F490", "0x8014FABC"):
            self.assertEqual(rows[deferred]["status"], "unmatched_asm")

    def test_drawing_bindings_reuse_resident_owners_without_data_aliases(self) -> None:
        directory = ROOT / "config/sles_03951"
        aliases = (directory / "overlays/duel_effects_linker_symbols.txt").read_text()
        resident = (directory / "symbols.txt").read_text() + (
            directory / "link_symbols.ld"
        ).read_text()
        for name, address in (
            ("D_8009B300", "0x8009C688"),
            ("ScaleMatrix", "0x800875F8"), ("GsSetLsMatrix", "0x80085558"),
            ("RotAverage3", "0x800879D8"), ("RotAverage4", "0x80087A38"),
            ("GsSortPoly", "0x800842A8"), ("func_8005B260", "0x8004D5B8"),
        ):
            binding = f"{name} = {address};"
            self.assertIn(binding, aliases)
            self.assertIn(binding, resident)
        for name in ("D_8015B7F4", "D_8015B7F8", "D_8015B800"):
            self.assertNotIn(f"{name} =", aliases)

    def test_packet_helpers_preserve_distinct_priority_and_mode_paths(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/packet_helpers.c").read_text()
        self.assertIn("void func_80152EC4(void *primitive, u16 flags)", source)
        bodies = dict(re.findall(
            r"void (func_[0-9A-F]+)\([^\n]+\)\n\{(.*?)\n\}", source, re.S
        ))
        for name in ("func_80152F9C", "func_801530B0"):
            self.assertIn("if (mode == 1)", bodies[name])
            self.assertIn("setSemiTrans(packet, 0);", bodies[name])
            self.assertIn(
                "func_8005B260((u32 *)packet, D_8015B7F4, D_8015B800, 1);",
                bodies[name],
            )
        self.assertIn(
            "GsSortPoly(packet, D_8015B7F4, D_8015B800);",
            bodies["func_80152F9C"],
        )
        self.assertIn(
            "GsSortPoly(packet, D_8015B7F4, (u16)(D_8015B800 + 1));",
            bodies["func_801530B0"],
        )
        self.assertIn(
            "func_8005B260((u32 *)packet, D_8015B7F4, (u16)(D_8015B800 + 1), flags);",
            bodies["func_80152EC4"],
        )

    def test_regional_packet_declarations_share_one_compatible_owner(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        declarations = (
            (directory / "drawing_helpers.h").read_text()
            + (directory / "packet_helpers.h").read_text()
        )
        self.assertEqual(declarations.count("extern SVECTOR D_8015B7F8;"), 1)
        self.assertNotIn("D_8015B7F8[", declarations)
        self.assertEqual(declarations.count("void func_80152EC4("), 1)
        self.assertIn("void func_80152EC4(void *primitive, u16 flags);", declarations)
        for source in ("packet_helpers.c", "sorting_helpers.c"):
            self.assertIn(
                "void func_80152EC4(void *primitive,",
                (directory / source).read_text(),
            )


if __name__ == "__main__":
    unittest.main()
