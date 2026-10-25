"""Run the actual Cursebreaker handler/helper with guarded ILP32 state and stubs."""

import os
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
HEADERS = r"""
#include "src/types.h"
#define DUEL_FIELD_GRID_2D
#include "src/game/duel_side_state.h"
#include "src/game/duel_grid.h"
#include "src/game/duel_card.h"
#include "src/game/duel_action_lock.h"
#include "src/game/duel_effect_request.h"
#include "src/game/sound.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct {
    u8 before[16];
    DuelCardRecord cards[DUEL_CARD_RECORD_COUNT];
    u8 gap_0[16];
    u8 grid[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
    u8 gap_1[16];
    DuelFieldPosition positions[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
    u8 gap_2[16];
    DuelEffectRequest request;
    u8 gap_3[16];
    s16 state_words[2];
    u16 timer;
    volatile u8 side;
    u16 flags;
    u8 after[16];
} GuardedState;
extern GuardedState state;
#define gDuel_aCardRecords state.cards
#define gDuel_abFieldGridCardRecordIndices state.grid
#define gDuel_aFieldGridGroundPositions state.positions
#define gDuel_sEffectWorkCounter state.state_words[1]
#define gDuel_wStepCounter state.timer
#define gDuel_bActiveSide state.side
#define gDuel_wCardEffectFlags state.flags
"""
WITNESS = HEADERS + r"""
GuardedState state;
static GuardedState expected;
static const u32 addresses[4] = {0x80013800, 0x80100000, 0x00103800, 0x10003800};
static u32 allocations, sounds, events, error, selected, old_stat, cases;
void DuelEffect_ApplyCursebreaker(void);
static void put16(void *p, u32 v) { ((u8 *)p)[0] = v; ((u8 *)p)[1] = v >> 8; }
static void put32(void *p, u32 v)
{
    ((u8 *)p)[0] = v; ((u8 *)p)[1] = v >> 8;
    ((u8 *)p)[2] = v >> 16; ((u8 *)p)[3] = v >> 24;
}
DuelEffectRequest *DuelEffect_AllocateRequest(s32 id)
{
    if (id != 8 || allocations != 0) error = 1;
    allocations++;
    events = events * 10 + 1;
    return &state.request;
}
void SD_SEPlayFull(u32 id)
{
    if (id != 0x14 || allocations != 1 || state.request.phase != 3 ||
        (u16)state.cards[selected].stat_modifier != old_stat) error = 2;
    sounds++;
    events = events * 10 + 2;
}
static int run_case(u32 side, s32 previous, u32 timer, u32 first,
                    u32 value, u32 occupied, u32 stat)
{
    u32 i, s, slot, advance, step, calls;
    for (i = 0; i < sizeof(state); i++) ((u8 *)&state)[i] = i * 37 + value;
    for (s = 0; s < DUEL_SIDE_COUNT; s++) {
        for (slot = 0; slot < DUEL_FIELD_SIDE_GRID_SLOT_COUNT; slot++) {
            state.grid[s][slot] = (slot * 7 + s * 13) % DUEL_CARD_RECORD_COUNT;
            put16(&state.positions[s][slot].x, value + slot * 113 + s * 307);
            put16(&state.positions[s][slot].y, value + slot * 317 + s * 109);
        }
    }
    state.side = side;
    state.flags = ((value ^ 0x4923) & ~0x80) | (first ? 0 : 0x80);
    state.timer = timer;
    state.state_words[1] = previous;
    state.request.buffer = (void *)addresses[value & 3];
    advance = first || timer <= 1 || timer > 0x8000;
    step = first ? 0 : previous + 1;
    calls = advance && step < 5;
    selected = calls ? state.grid[side][10 + step] : 0;
    if (calls) {
        put16(&state.positions[side][10 + step].x, value);
        put16(&state.positions[side][10 + step].y, value ^ 0x8000);
        state.cards[selected].flags =
            (value & ~DUEL_CARD_FLAG_OCCUPIED) | (occupied ? DUEL_CARD_FLAG_OCCUPIED : 0);
        put16(&state.cards[selected].stat_modifier, stat);
    }
    old_stat = (u16)state.cards[selected].stat_modifier;
    for (i = 0; i < sizeof(state); i++) ((u8 *)&expected)[i] = ((u8 *)&state)[i];
    if (first) expected.flags |= 0x80;
    expected.timer = advance ? 16 : timer - 1;
    if (advance) {
        expected.state_words[1] = step;
        if (calls) {
            put16(&expected.request.field_00, value);
            put16(&expected.request.field_02, 0);
            put16(&expected.request.field_04, value ^ 0x8000);
            put32(&expected.request.buffer, addresses[value & 3] + step * 0x3000);
            expected.request.phase = 3;
            if (occupied && (stat & 0x8000)) {
                expected.cards[selected].stat_modifier = 0;
                expected.request.phase = 5;
            }
        } else expected.flags = 0;
    }
    allocations = sounds = events = error = 0;
    DuelEffect_ApplyCursebreaker();
    if (error || allocations != calls || sounds != calls || events != (calls ? 12 : 0))
        return 1;
    for (i = 0; i < sizeof(state); i++)
        if (((u8 *)&state)[i] != ((u8 *)&expected)[i]) return 1;
    cases++;
    return 0;
}
int main(void)
{
    static const u16 stats[] = {0, 1, 0x7FFF, 0x8000, 0xFFFF};
    u32 side, step, value, occupied, i, first;
    for (side = 0; side < 2; side++)
        for (step = 0; step < 5; step++)
            for (value = 0; value < 65536; value++)
                if (run_case(side, step - 1, 1, 0, value, value & 1, value ^ 0x1234))
                    return 1;
    for (value = 0; value < 65536; value++)
        if (run_case(value & 1, value % 6 - 1, value, 0, value, 1, value ^ 0x1234))
            return 1;
    for (side = 0; side < 2; side++)
        for (step = 0; step < 6; step++)
            for (occupied = 0; occupied < 2; occupied++)
                for (i = 0; i < sizeof(stats) / sizeof(stats[0]); i++)
                    if (run_case(side, step - 1, 1, 0, 0x1234, occupied, stats[i]))
                        return 1;
    for (first = 0; first < 2; first++)
        for (side = 0; side < 2; side++)
            for (value = 0; value < 65536; value++)
                if (run_case(side, 3, 16, first, value, value & 1, value ^ 0x1234))
                    return 1;
    return cases == 983160 ? 0 : 2;
}
"""


def function(source, name):
    start = source.index(name + "(void)")
    start = source.rfind("\n", 0, start) + 1
    return source[start:source.index("\n}\n", start) + 3]


class CursebreakerRequestTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_full_handler_and_real_initializer_with_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        body = function((ROOT / "src/game/duel_field_effects.c").read_text(),
                        "DuelEffect_ApplyCursebreaker")
        initializer = function((ROOT / "src/game/duel_action_lock.c").read_text(),
                               "DuelEffect_MarkInitialized")
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "scaled-word-step"),
            ("-O2", "cleared-positive-modifier"), ("-O2", "late-sound"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = body
                if mutation == "scaled-word-step":
                    code = code.replace("step * 0x3000", "step * 0xC000")
                elif mutation == "cleared-positive-modifier":
                    code = code.replace("record->stat_modifier < 0", "record->stat_modifier != 0")
                elif mutation == "late-sound":
                    code = code.replace("        SD_SEPlayFull(0x14);\n", "")
                    code = code.replace(
                        "        }\n    } else {",
                        "        }\n        SD_SEPlayFull(0x14);\n    } else {",
                    )
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, body)
                with tempfile.TemporaryDirectory(prefix="curse-request-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(HEADERS + initializer + "\n" + code)
                    witness = directory / "witness.c"
                    witness.write_text(WITNESS)
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
