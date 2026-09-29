import csv
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelBankDispatchTests(unittest.TestCase):
    def test_all_cases_and_two_argument_special_callee_remain_explicit(self) -> None:
        source = (DIRECTORY / "dispatch.c").read_text()
        header = (DIRECTORY / "dispatch.h").read_text()
        self.assertEqual(
            [int(value) for value in re.findall(r"if \(effect == (\d+)\)", source)],
            list(range(25)),
        )
        self.assertNotIn("else if", source)
        self.assertIn("if (phase >= 0)", source)
        self.assertIn("i < 21", source)
        self.assertIn(
            "func_8014F564(D_8015B748.pairs[i], &D_8015A1E4[i], D_8015A430[i]);",
            source,
        )
        special = source.split("if (effect == 9) {", 1)[1].split("if (effect == 10)", 1)[0]
        self.assertLess(
            special.index("D_8015B7F4 = (GsOT *)context->field_08;"),
            special.index("D_8015B7F4 = (GsOT *)context->field_0C;"),
        )
        self.assertIn("func_801593A8(buffer, phase);", special)
        self.assertIn("void func_801593A8(void *work, s32 phase);", header)
        phase_case = source.split("if (effect == 20) {", 1)[1].split("if (effect == 21)", 1)[0]
        self.assertIn("func_80151558(buffer, 1);", phase_case)
        self.assertIn("func_80151558(buffer, phase);", phase_case)

    def test_dispatch_reuses_the_resident_request_record(self) -> None:
        header = (DIRECTORY / "dispatch.h").read_text()
        source = (DIRECTORY / "dispatch.c").read_text()
        self.assertIn('#include "../../game/duel_effect_request.h"', header)
        self.assertIn("DuelEffectRequest *context", header)
        self.assertIn("DuelEffectRequest *context", source)
        self.assertNotIn("DuelEffectDispatchContext", header + source)
        self.assertIn("u16 flags = context->field_10;", source)
        self.assertIn(
            "setVector(&D_8015B7F8, context->field_00, context->field_02, context->field_04);",
            source,
        )
        self.assertEqual(source.count("context->field_12"), 2)

    def test_callees_and_data_keep_real_overlay_owners(self) -> None:
        for region in ("sles_03951", "sles_03948"):
            with self.subTest(region=region):
                self._assert_real_owners(region)

    def _assert_real_owners(self, region: str) -> None:
        directory = ROOT / f"config/{region}/overlays"
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        source = (DIRECTORY / "dispatch.c").read_text()
        with (directory / "duel_effects_functions.csv").open() as handle:
            inventory = {row["name"]: row for row in csv.DictReader(handle)}
        callees = set(re.findall(r"\b(func_[0-9A-F]+)\(", source)) - {"func_80146258"}
        self.assertEqual(len(callees), 25)
        for name in callees:
            self.assertNotIn(name + " =", aliases)
            self.assertIn(inventory[name]["status"], ("matching_c", "unmatched_asm"))
        for address, size in (
            (0x8015A1E4, 0x24C), (0x8015A430, 0x2A),
            (0x8015B748, 0x54), (0x8015B7F4, 4),
            (0x8015B7F8, 8), (0x8015B800, 2),
        ):
            name = f"D_{address:X}"
            self.assertNotIn(name + " =", aliases)
            self.assertIn(f"{name} = 0x{address:X}; // size:0x{size:X}", symbols)


if __name__ == "__main__":
    unittest.main()
