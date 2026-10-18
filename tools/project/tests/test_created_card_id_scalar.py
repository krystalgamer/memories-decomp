"""Exercise the actual scalar helper's byte gates and modulo-word flag argument."""

import os
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
FIXTURE = r"""
typedef char ilp32[sizeof(void *) == 4 && sizeof(u32) == 4 ? 1 : -1];
typedef struct {
    u8 before[16];
    u8 side;
    s8 controller;
    s8 opponent;
    u8 after[16];
} GuardedConditions;
extern GuardedConditions guarded;
#define D_8009B1D5 guarded.side
#define D_8009B360 guarded.controller
#define gDuel_bOpponentID (&guarded.opponent)
"""
WITNESS = r"""
GuardedConditions guarded, expected;
static u32 input, calls, error;
void Library_UpdateCardUsedFlag(s32 flag)
{
    u32 i;
    if ((u32)flag != (u32)((u64)input + 0x120)) error = 1;
    for (i = 0; i < sizeof(guarded); i++)
        if (((u8 *)&guarded)[i] != ((u8 *)&expected)[i]) error = 2;
    calls++;
}
static s32 check(u32 wanted)
{
    u32 i;
    for (i = 0; i < sizeof(guarded); i++) ((u8 *)&expected)[i] = ((u8 *)&guarded)[i];
    calls = error = 0;
    Duel_MarkCreatedCardKnown((s32)input);
    if (error || calls != wanted) return 1;
    for (i = 0; i < sizeof(guarded); i++)
        if (((u8 *)&guarded)[i] != ((u8 *)&expected)[i]) return 1;
    return 0;
}
int main(void)
{
    static const u32 high[] = {0, 0x10000, 0x7FFF0000, 0x80000000, 0xFFFF0000};
    u32 side, controller, opponent, i, h, low, cases = 0;
    for (i = 0; i < sizeof(guarded); i++) ((u8 *)&guarded)[i] = i * 37 + 11;
    for (side = 0; side < 256; side++)
        for (controller = 0; controller < 256; controller++)
            for (opponent = 0; opponent < 256; opponent++) {
                guarded.side = side;
                guarded.controller = controller;
                guarded.opponent = opponent;
                input = (side << 24) | (controller << 16) | (opponent << 8) | 0x7F;
                if (check(side == 0 && controller >= 128 && opponent < 128)) return 1;
                cases++;
            }
    guarded.side = 0; guarded.controller = -1; guarded.opponent = 0;
    for (h = 0; h < 5; h++)
        for (low = 0; low < 65536; low++) {
            input = high[h] | low;
            if (check(1)) return 1;
            cases++;
        }
    return cases == 17104896 ? 0 : 2;
}
"""


class CreatedCardIdScalarTests(unittest.TestCase):
    def test_integer_argument_and_original_caller_widths(self):
        source = (ROOT / "src/game/duel_mark_created_card_known.c").read_text()
        header = (ROOT / "src/game/duel_ritual_controller.h").read_text()
        for text in (source, header):
            self.assertIn("Duel_MarkCreatedCardKnown(s32 card_id)", text)
        self.assertIn(
            "Library_UpdateCardUsedFlag((s32)((u32)card_id + CAMPAIGN_FLAG_LIBRARY_CARD_BASE));",
            source,
        )
        self.assertIn("D_8009B1D5 == 0 && D_8009B360 < 0 && gDuel_bOpponentID[0] >= 0", source)
        ritual = (ROOT / "src/game/duel_ritual_effect.c").read_text()
        placement = (ROOT / "src/game/duel_scene_card_placement.c").read_text()
        self.assertIn("Duel_MarkCreatedCardKnown((s32)gDuel_RitualResultCardId);", ritual)
        self.assertIn("Duel_MarkCreatedCardKnown((u32)(D_8009B150 & 0xFFF));", placement)
        self.assertNotIn("(char *)object", source)

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires Linux x86 compiler and ILP32 execution",
    )
    def test_actual_helper_full_byte_gates_and_word_wrap_with_negatives(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_mark_created_card_known.c").read_text()
        start = source.index("void Duel_MarkCreatedCardKnown(")
        prefix, original = source[:start], source[start:]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "narrow-id"),
            ("-O2", "wrong-controller-sign"), ("-O2", "wrong-opponent-sign"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = original
                if mutation == "narrow-id":
                    code = code.replace("(u32)card_id", "(u32)(u16)card_id")
                elif mutation == "wrong-controller-sign":
                    code = code.replace("D_8009B360 < 0", "D_8009B360 >= 0")
                elif mutation == "wrong-opponent-sign":
                    code = code.replace("gDuel_bOpponentID[0] >= 0", "gDuel_bOpponentID[0] < 0")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, original)
                with tempfile.TemporaryDirectory(prefix="created-card-word-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(prefix + FIXTURE + code)
                    witness = directory / "witness.c"
                    witness.write_text(prefix + FIXTURE + WITNESS)
                    startup = directory / "start.S"
                    startup.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT / "src/game"),
                         str(operations), str(witness), str(startup), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
