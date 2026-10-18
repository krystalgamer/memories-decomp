"""Guard the constructor's actual default-stat blocks and their argument roles."""

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
HEADERS = r"""
#include "src/types.h"
#include "src/game/duel_card.h"
#include "src/game/duel_effect_resource_record.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct { u8 before[16]; DuelEffectResourceRecord entry; u8 after[16]; } GuardedRecord;
typedef struct { u8 before[16]; s32 stats[3]; u8 after[16]; } GuardedWords;
extern GuardedRecord record_block;
extern GuardedWords word_block;
#define gDuel_adwCardStats word_block.stats
"""
WITNESS = r"""
GuardedRecord record_block, expected;
GuardedWords word_block, expected_words;
static u32 cases;
void apply_stat_overrides(s32 attack, s32 defense);
static void put16(void *p, u32 v) { ((u8 *)p)[0] = v; ((u8 *)p)[1] = v >> 8; }
static int run_case(u32 attack, u32 defense, u32 packed)
{
    u32 i, low_attack, low_defense;
    for (i = 0; i < sizeof(record_block); i++) ((u8 *)&record_block)[i] = i * 37 + attack;
    for (i = 0; i < sizeof(word_block); i++) ((u8 *)&word_block)[i] = i * 19 + defense;
    word_block.stats[0] = 0x12345678;
    word_block.stats[1] = packed;
    word_block.stats[2] = 0x87654321;
    for (i = 0; i < sizeof(record_block); i++)
        ((u8 *)&expected)[i] = ((u8 *)&record_block)[i];
    for (i = 0; i < sizeof(word_block); i++)
        ((u8 *)&expected_words)[i] = ((u8 *)&word_block)[i];
    low_attack = attack & 0xFFFF;
    low_defense = defense & 0xFFFF;
    if (low_attack & 0x8000) low_attack = (packed & 0x1FF) * 10;
    if (low_defense & 0x8000) low_defense = ((packed >> 9) & 0x1FF) * 10;
    put16(&expected.entry.field_32, low_attack);
    put16(&expected.entry.field_34, low_defense);
    apply_stat_overrides((s32)attack, (s32)defense);
    for (i = 0; i < sizeof(record_block); i++)
        if (((u8 *)&record_block)[i] != ((u8 *)&expected)[i]) return 1;
    for (i = 0; i < sizeof(word_block); i++)
        if (((u8 *)&word_block)[i] != ((u8 *)&expected_words)[i]) return 1;
    cases++;
    return 0;
}
int main(void)
{
    static const u32 high[] = {0, 0x7FFF0000, 0x80000000, 0xFFFF0000};
    static const u16 low[] = {0, 1, 0x7FFF, 0x8000, 0xFFFF};
    static const u32 packed[] = {0, 0xFFFFFFFF, 0xFC000000, 0x0003FFFF};
    u32 h, bits, a, d, ah, dh, p, word;
    for (h = 0; h < 4; h++)
        for (bits = 0; bits < 65536; bits++) {
            word = 0xFC000000 | (bits & 0x1FF) | (((bits >> 3) & 0x1FF) << 9);
            if (run_case(high[h] | bits, high[(h + 1) & 3] | (bits ^ 0x8000), word))
                return 1;
        }
    for (ah = 0; ah < 4; ah++)
        for (dh = 0; dh < 4; dh++)
            for (a = 0; a < 5; a++)
                for (d = 0; d < 5; d++)
                    for (p = 0; p < 4; p++)
                        if (run_case(high[ah] | low[a], high[dh] | low[d], packed[p]))
                            return 1;
    return cases == 263744 ? 0 : 2;
}
"""


class DuelResourceStatArgumentTests(unittest.TestCase):
    def test_argument_roles_from_independent_main_game_producers(self):
        source = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        header = (ROOT / "src/game/func_800291E0.h").read_text()
        for text in (source, header):
            self.assertIn("func_800291E0(s32 index, s32 attack, s32 defense)", text)
        self.assertIn("setup = attack;", source)
        self.assertIn("s16 attack_value = setup;", source)
        self.assertIn("s16 defense_value = defense;", source)
        self.assertIn("object->field_30.h.field_30 = DUEL_RESOURCE_DEFAULT_X;", source)
        producer = (ROOT / "src/game/duel_calc_card_stats.c").read_text()
        self.assertIn("return (defense << 16) | attack;", producer)
        for filename, variable in (
            ("duel_scene_battle.c", "result"), ("duel_scene_card_placement.c", "value"),
        ):
            caller = (ROOT / "src/game" / filename).read_text()
            self.assertIn(variable + " = Duel_CalcCardStats(", caller)
            self.assertIn(
                "func_800291E0(0, " + variable + " & 0xFFFF, " + variable + " >> 16)", caller
            )
        base_stats = (ROOT / "src/game/duel_card_record_lifecycle.c").read_text()
        self.assertRegex(
            base_stats,
            r"p->attack\s*=\s*\(gDuel_adwCardStats\[.*?\] & CARD_STAT_VALUE_MASK\)\s*\*",
        )
        self.assertRegex(
            base_stats,
            r"p->defense\s*=\s*\(\(gDuel_adwCardStats\[.*?\] >>\s*"
            r"CARD_STAT_DEFENSE_SHIFT\) & CARD_STAT_VALUE_MASK\)\s*\*",
        )
        renderer = (ROOT / "src/game/func_80028B08.c").read_text()
        for text in (
            "i = rec->field_32 + rec->field_36;",
            "Text_EncodeDecimalDigits(i, 4, buf1);",
            "win->field_30.h.field_32 + FUNC_80028B08_ATK_Y;",
            "PRM->uv.b.lo = buf1[i] * 6 + 0x10;",
            "i = rec->field_34 + rec->field_38;",
            "Text_EncodeDecimalDigits(i, 4, buf2);",
            "win->field_30.h.field_32 + FUNC_80028B08_DEF_Y;",
            "PRM->uv.b.lo = buf2[i] * 6 + 0x10;",
        ):
            self.assertIn(text, renderer)

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_default_stat_blocks_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        match = re.search(
            r"(?s)    \{\n        s16 attack_value = setup;.*?\n"
            r"    \}\n    \{\n        s16 defense_value = defense;.*?\n    \}", source,
        )
        self.assertIsNotNone(match)
        fragment = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "full-word-sign"),
            ("-O2", "swapped-stat-fields"), ("-O2", "wrong-default-scale"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = fragment
                if mutation == "full-word-sign":
                    code = code.replace("if (attack_value < 0)", "if (setup < 0)")
                    code = code.replace("if (defense_value < 0)", "if (defense < 0)")
                elif mutation == "swapped-stat-fields":
                    code = code.replace("field_32", "temporary_stat_field")
                    code = code.replace("field_34", "field_32")
                    code = code.replace("temporary_stat_field", "field_34")
                elif mutation == "wrong-default-scale":
                    code = code.replace("* 10", "* 100")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragment)
                with tempfile.TemporaryDirectory(prefix="resource-stat-args-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid apply_stat_overrides(s32 attack, s32 defense) {\n"
                        "DuelEffectResourceRecord *entry = &record_block.entry;\n"
                        "s32 setup = attack; s32 card_id = 2;\n" + code + "\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + WITNESS)
                    start = directory / "start.S"
                    start.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                         str(operations), str(witness), str(start), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
