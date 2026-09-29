import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/overlays/duel_effects/effect_23.c"


class DuelEffect23Tests(unittest.TestCase):
    def test_spanish_registration_reuses_the_complete_french_source(self) -> None:
        entries = []
        for region in ("sles_03948", "sles_03951"):
            manifest = json.loads(
                (ROOT / f"config/{region}/overlays/duel_effects_matching_c.json").read_text()
            )
            entries.append([row for row in manifest["functions"] if row["address"] == "0x80152048"])
        self.assertEqual(entries[0], entries[1])
        self.assertEqual(entries[1], [{
            "address": "0x80152048", "size": "0xE7C",
            "source": "src/overlays/duel_effects/effect_23.c",
            "profile": "gcc_2_8_1_g0_split",
        }])
        source = SOURCE.read_text()
        self.assertIn('#include "../../types.h"', source)
        self.assertNotIn("extern ", source)
        self.assertNotIn("asm", source)

    def test_card_rotation_preserves_the_repeated_x_byte(self) -> None:
        source = SOURCE.read_text()
        for field in ("field_20", "field_21", "field_22"):
            self.assertIn(
                f"->field_20.b.{field} += work->card_rotations[i].vx;", source
            )
        self.assertEqual(source.count("+= work->card_rotations[i].vx;"), 3)
        self.assertIn("(s16)((DisplayObject *)D_8015B7A0[i])->field_30.h.field_30", source)
        self.assertIn("->attribute |= 0x50000000;", source)
        self.assertIn("->flags &= ~0x40;", source)

    def test_five_slots_and_three_particles_keep_texture_frames(self) -> None:
        source = SOURCE.read_text()
        header = SOURCE.with_suffix(".h").read_text()
        self.assertIn("SVECTOR particles[5][3];", header)
        self.assertIn("SVECTOR velocities[5][3];", header)
        for expression in (
            "work->ages[i] < 24 && work->states[i] >= 2",
            "D_8015B748.pairs[19][0]", "D_8015B748.pairs[19][1]",
            "D_8015B748.pairs[20][0]", "D_8015B748.pairs[20][1]",
            "((work->ages[i] / 3) % 4) * 32",
            "(work->ages[i] / 12) * 32 + 192",
            "(work->ages[i] / 12) * 32 + 223",
            "addVector(&work->particles[i][j], &work->velocities[i][j]);",
        ):
            self.assertIn(expression, source)

    def test_sweep_and_both_completion_paths_remain(self) -> None:
        source = SOURCE.read_text()
        for expression in (
            "Duel_CollectFieldRowCardObjects(D_8015B7A0, 1);",
            "work->position.vy < -104", "work->rotation.vx < 320",
            "work->rotation.vx > -320", "SD_SEPlayFull(36);",
            "work->position.vx >= i * 70 - 44",
            "work->position.vx <= 236 - i * 70",
            "if (work->count != 0)",
            "D_8015B7A0[work->count - 1]",
            "->field_0C & 0xFFFFFF",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)

    def test_initial_vector_has_exact_real_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        self.assertIn("D_801461A8 = 0x801461A8; // size:0x10", symbols)
        self.assertNotIn("D_801461A8 =", aliases)
        self.assertEqual(0x801461A8 + 16, 0x801461B8)


if __name__ == "__main__":
    unittest.main()
