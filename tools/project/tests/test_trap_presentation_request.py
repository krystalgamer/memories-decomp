"""Guard the actual request/copy/removal fragment, not all four presentation modes."""

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
FIXTURE = r"""
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct {
    u8 before[16];
    DuelEffectRequest request;
    u8 gap_0[16];
    DuelCardRecord records[DUEL_CARD_RECORD_COUNT];
    u8 gap_1[16];
    DisplayObject object;
    u8 gap_2[16];
    u16 mode;
    u8 after[16];
} GuardedState;
extern GuardedState state;
#define gDuel_aCardRecords state.records
#define gDuel_wTrapPresentationMode state.mode
"""
WITNESS = r"""
GuardedState state, expected;
static u32 value, index, mode, events, error, cases;
void copy_trap_request(DisplayObject *p);
static void put16(void *p, u32 v) { ((u8 *)p)[0] = v; ((u8 *)p)[1] = v >> 8; }
static int copied(void)
{
    return (u16)state.request.field_00 == value &&
           (u16)state.request.field_02 == (value ^ 0x8000) &&
           (u16)state.request.field_04 == (value ^ 0xFFFF);
}
DuelEffectRequest *DuelEffect_CreateRequest(s32 id)
{
    if (id != 8 || events != 0) error = 1;
    events = events * 10 + 1;
    return &state.request;
}
void DuelCard_RemoveFromField(DuelCardRecord *record)
{
    if (events != 1 || record != &state.records[index] || !copied() ||
        state.mode != mode) error = 2;
    events = events * 10 + 2;
    record->card_id = 0;
}
void SD_SEPlayFull(u32 id)
{
    if (id != 0x17 || events != 12 || !copied() || state.mode != mode ||
        state.records[index].card_id != 0) error = 3;
    events = events * 10 + 3;
}
int main(void)
{
    u32 i;
    for (index = 0; index < DUEL_CARD_RECORD_COUNT; index++) {
        for (value = 0; value < 65536; value++) {
            for (i = 0; i < sizeof(state); i++) ((u8 *)&state)[i] = i * 37 + value;
            state.mode = mode = (value & 0xFFF0) | 1;
            state.object.field_6A = index;
            state.object.field_30.h.field_30 = value;
            state.object.field_30.h.field_32 = value ^ 0x8000;
            put16(&state.object.field_34.h.field_34, value ^ 0xFFFF);
            for (i = 0; i < sizeof(state); i++) ((u8 *)&expected)[i] = ((u8 *)&state)[i];
            put16(&expected.request.field_00, value);
            put16(&expected.request.field_02, value ^ 0x8000);
            put16(&expected.request.field_04, value ^ 0xFFFF);
            expected.records[index].card_id = 0;
            expected.mode = 2;
            events = error = 0;
            copy_trap_request(&state.object);
            if (error || events != 123) return 1;
            for (i = 0; i < sizeof(state); i++)
                if (((u8 *)&state)[i] != ((u8 *)&expected)[i]) return 1;
            cases++;
        }
    }
    return cases == 1966080 ? 0 : 2;
}
"""


class TrapPresentationRequestTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_copy_removal_fragment_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_battle_stats.c").read_text()
        # The unit opens with the battle arithmetic; keep only its include
        # block and the trap section's own preamble.
        includes = re.match(r"(?:#[^\n]*\n|[ \t]*\n)*", source).group()
        trap_start = source.index("#define DUEL_ATTACK_TRAP_SCRATCH_VIEW")
        prefix = includes + source[trap_start:source.index("u8 gDuel_abTrapAttackThresholds[")]
        match = re.search(
            r"(?s)e = DuelEffect_CreateRequest\(8\);.*?gDuel_wTrapPresentationMode = 2;", source,
        )
        self.assertIsNotNone(match)
        fragment = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wide-third-payload"),
            ("-O2", "removal-before-copy"), ("-O2", "mode-before-sound"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = fragment
                if mutation == "wide-third-payload":
                    code = code.replace(
                        "*(d = (u16 *)&e->field_04) = *q34;",
                        "*(u32 *)&e->field_04 = *q34;",
                    )
                elif mutation == "removal-before-copy":
                    statement = "DuelCard_RemoveFromField(&gDuel_aCardRecords[p->field_6A]);"
                    code = code.replace(statement, "")
                    code = code.replace("*(d =", statement + "\n    *(d =")
                elif mutation == "mode-before-sound":
                    code = code.replace("SD_SEPlayFull(0x17);", "")
                    code = code.replace("gDuel_wTrapPresentationMode = 2;", "gDuel_wTrapPresentationMode = 2;\n    SD_SEPlayFull(0x17);")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragment)
                with tempfile.TemporaryDirectory(prefix="trap-request-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        prefix + FIXTURE + "\nvoid copy_trap_request(DisplayObject *p) {\n"
                        "DuelEffectRequest *e; u16 *q34, *d;\n" + code + "\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(prefix + FIXTURE + WITNESS)
                    start = directory / "start.S"
                    start.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT / "src/game"),
                         str(operations), str(witness), str(start), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
