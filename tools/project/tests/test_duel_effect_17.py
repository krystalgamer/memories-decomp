import csv
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect17Tests(unittest.TestCase):
    def test_halfword_brightness_keeps_explicit_low_byte_packing(self) -> None:
        declaration = "void func_8015405C(u16 high, u16 middle, u16 low)"
        self.assertIn(declaration + ";", (DIRECTORY / "color_helpers.h").read_text())
        source = (DIRECTORY / "color_transition.c").read_text()
        self.assertIn(declaration, source)
        self.assertIn("((u8)high << 16) | ((u8)middle << 8) | (u8)low", source)
        self.assertIn(
            "func_8015405C(work->brightness, work->brightness, work->brightness);",
            (DIRECTORY / "effect_17.c").read_text(),
        )

    def test_collector_and_pool_lifetimes_remain_explicit(self) -> None:
        source = (DIRECTORY / "effect_17.c").read_text()
        header = (DIRECTORY / "effect_17.h").read_text()
        for declaration in (
            "SVECTOR card_particles[20][16];", "SVECTOR particles[64];",
            "SVECTOR paths[48][8];", "CVECTOR path_colors[48];",
            "extern u32 D_8015B7A0[21];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "Duel_CollectMatchingFieldCardObjects(D_8015B7A0, -1);",
            "Duel_CollectMatchingFieldCardObjects(D_8015B7A0, 0);",
            "if (work->card_count && work->stage < 2)",
            "work->card_particles[work->completed - 1]",
            "work->card_ages[i] > 16", "work->path_ages[i] = 7;",
            "work->active_paths = 48;", "work->active_particles = 64;",
        ):
            self.assertIn(expression, source)
        self.assertEqual(
            source.count("(s32)((u32)((rand() - rand()) % 4096) << 1)"), 3
        )

    def test_resident_bindings_and_curve_fallback_keep_real_owners(self) -> None:
        region = ROOT / "config/sles_03951"
        with (region / "functions.csv").open() as handle:
            resident = {row["address"]: row for row in csv.DictReader(handle)}
        aliases = (region / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, address in (
            ("Duel_CollectMatchingFieldCardObjects", "0x8002CB88"),
            ("Model_GetLightSourceMatrix", "0x8005C328"),
        ):
            self.assertEqual(resident[address]["name"], name)
            self.assertEqual(resident[address]["status"], "matching_c")
            self.assertIn(f"{name} = {address};", aliases)
        self.assertEqual(resident["0x80089928"]["status"], "sdk_asm")
        with (region / "overlays/duel_effects_functions.csv").open() as handle:
            curve = next(row for row in csv.DictReader(handle) if row["address"] == "0x8014FABC")
        self.assertEqual((curve["status"], curve["size"]), ("unmatched_asm", "0x344"))
        for name in ("D_80146034", "D_8015A60C", "D_8015B7A0"):
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
