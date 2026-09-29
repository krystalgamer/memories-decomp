from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect15Tests(unittest.TestCase):
    def test_lifecycle_preserves_double_submission_and_delayed_blink_completion(self) -> None:
        source = (DIRECTORY / "effect_15.c").read_text()
        for expression in (
            "if (phase >= 8)", "work->config = &D_8015AC08[phase];",
            "work->config->selector == 999", "work->cross_frame > 180",
            "work->scale > 1024", "work->scale -= 512;",
            "work->state == 2 && work->scale == 1024",
            "if (work->state == 1 && work->timer < 16)",
            "if (!(work->timer % 4))", "} else if (work->timer >= 16)",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("func_80151218(packet, quad, 32, 1);"), 2)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)
        self.assertNotIn("asm", source)

    def test_collection_and_object_coordinates_use_canonical_owners(self) -> None:
        source = (DIRECTORY / "effect_15.c").read_text()
        header = (DIRECTORY / "effect_15.h").read_text()
        self.assertIn("extern u32 D_8015B7A0[21];", header)
        self.assertIn('"../../game/duel_card.h"', header)
        self.assertIn('"../../game/display_object.h"', header)
        for member in ("field_30.h.field_30", "field_30.h.field_32",
                       "field_34.h.field_34"):
            self.assertIn(f"((DisplayObject *)D_8015B7A0[i])->{member}", source)
        self.assertIn("D_8015B748.pairs[13][0]", source)
        self.assertIn("D_8015B748.pairs[13][1]", source)
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        resident = (ROOT / "config/sles_03951/symbols.txt").read_text()
        for binding in (
            "Duel_CollectFieldRowCardObjects = 0x8002CB0C;",
            "Duel_CollectMatchingFieldCardObjects = 0x8002CB88;",
            "Model_GetLightSourceMatrix = 0x8005C328;",
        ):
            self.assertIn(binding, symbols)
            self.assertIn(binding, aliases)
            self.assertTrue(binding in resident)

    def test_tables_and_shared_output_retain_real_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (
            ("D_80146168", "0x10"), ("D_8015AC08", "0x40"),
            ("D_8015B7A0", "0x54"),
        ):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        self.assertIn("DuelEffect15Config D_8015AC08[8];",
                      (DIRECTORY / "effect_15.h").read_text())


if __name__ == "__main__":
    unittest.main()
