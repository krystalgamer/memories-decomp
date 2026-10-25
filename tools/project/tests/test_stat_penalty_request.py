"""Guard the actual request setup/penalty fragment and integer address-word math."""

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
#include "src/game/card_constants.h"
#include "src/game/duel_card.h"
#include "src/game/display_object.h"
#include "src/game/duel_effect_request.h"
#include "src/game/duel_action_lock.h"
#include "src/game/sound.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef char buffer_word[(u32)&((DuelEffectRequest *)0)->buffer == 0x14
                         && sizeof(((DuelEffectRequest *)0)->buffer) == 4 ? 1 : -1];
typedef struct { u8 before[16]; DuelEffectRequest request; u8 after[16]; } GuardedRequest;
typedef struct { u8 before[16]; DuelCardRecord record; u8 after[16]; } GuardedRecord;
typedef struct { u8 before[16]; DisplayObject card; u8 after[16]; } GuardedCard;
extern GuardedRequest request_block;
extern GuardedRecord record_block;
extern GuardedCard card_block;
extern u16 gDuel_wStepCounter;
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
"""
WITNESS = HEADERS + r"""
GuardedRequest request_block;
GuardedRecord record_block;
GuardedCard card_block;
u16 gDuel_wStepCounter;
s16 gDuel_wEffectCardID;
static u8 expected_request[sizeof(request_block)], expected_record[sizeof(record_block)];
static u8 expected_card[sizeof(card_block)];
static const u32 addresses[4] = {0x80013800, 0x80100000, 0x00103800, 0x10003800};
static u32 allocations, sounds, error, penalty;
void apply_penalty_fragment(void);
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
static void put32(u8 *p, u32 value)
{
    p[0] = value; p[1] = value >> 8; p[2] = value >> 16; p[3] = value >> 24;
}
DuelEffectRequest *DuelEffect_AllocateRequest(s32 id)
{
    if (id != 0xD) error = 1;
    allocations++;
    return &request_block.request;
}
void SD_SEPlayFull(u32 id)
{
    if (id != 0x21 || allocations != 1 ||
        request_block.request.phase != (penalty == 500 ? 2 : 1) ||
        request_block.request.field_12 != -(s32)penalty) error = 2;
    sounds++;
}
int main(void)
{
    u32 branch, step, value, i, req, rec, cases = 0;
    req = (u8 *)&request_block.request - (u8 *)&request_block;
    rec = (u8 *)&record_block.record - (u8 *)&record_block;
    for (branch = 0; branch < 2; branch++) {
        gDuel_wEffectCardID = branch == 0 ? DUEL_SPELLBINDING_CIRCLE_CARD_ID : 0;
        penalty = branch == 0 ? 500 : 1000;
        for (step = 0; step < DUEL_FIELD_ROW_SIZE; step++) {
            gDuel_wStepCounter = step;
            for (value = 0; value < 65536; value++) {
                for (i = 0; i < sizeof(request_block); i++)
                    ((u8 *)&request_block)[i] = i * 37 + value;
                request_block.request.buffer = (void *)addresses[value & 3];
                for (i = 0; i < sizeof(request_block); i++)
                    expected_request[i] = ((u8 *)&request_block)[i];
                for (i = 0; i < sizeof(record_block); i++)
                    ((u8 *)&record_block)[i] = i * 19 + value;
                record_block.record.object = &card_block.card;
                put16((u8 *)&record_block.record.stat_modifier, value ^ 0x5A5A);
                for (i = 0; i < sizeof(record_block); i++)
                    expected_record[i] = ((u8 *)&record_block)[i];
                for (i = 0; i < sizeof(card_block); i++)
                    ((u8 *)&card_block)[i] = i * 13 + value;
                card_block.card.field_30.h.field_30 = value;
                card_block.card.field_30.h.field_32 = value ^ 0x8000;
                put16((u8 *)&card_block.card.field_34, value ^ 0xFFFF);
                for (i = 0; i < sizeof(card_block); i++)
                    expected_card[i] = ((u8 *)&card_block)[i];
                put16(expected_request + req, value);
                put16(expected_request + req + 2, value ^ 0x8000);
                put16(expected_request + req + 4, value ^ 0xFFFF);
                put32(expected_request + req + 0x14, addresses[value & 3] + (step << 14));
                put16(expected_request + req + 0x1A, branch == 0 ? 2 : 1);
                put16(expected_request + req + 0x12, 0 - penalty);
                put16(expected_record + rec + 0x12, (value ^ 0x5A5A) - penalty);
                allocations = sounds = error = 0;
                apply_penalty_fragment();
                if (error || allocations != 1 || sounds != 1) return 1;
                for (i = 0; i < sizeof(request_block); i++)
                    if (((u8 *)&request_block)[i] != expected_request[i]) return 1;
                for (i = 0; i < sizeof(record_block); i++)
                    if (((u8 *)&record_block)[i] != expected_record[i]) return 1;
                for (i = 0; i < sizeof(card_block); i++)
                    if (((u8 *)&card_block)[i] != expected_card[i]) return 1;
                cases++;
            }
        }
    }
    return cases == 655360 ? 0 : 2;
}
"""


class StatPenaltyRequestTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_setup_fragment_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_card_effects.c").read_text()
        source = source[source.index("void DuelEffect_ApplyStatPenalty("):]
        match = re.search(
            r"(?s)(?P<setup>card = DISPLAY_OBJECT_VIEW\(record->object\);.*?"
            r"SD_SEPlayFull\(0x21\);)", source,
        )
        self.assertIsNotNone(match)
        fragment = match["setup"]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "scaled-word-step"),
            ("-O2", "byte-payload-store"), ("-O2", "wrong-penalty"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = fragment
                if mutation == "scaled-word-step":
                    code = code.replace("gDuel_wStepCounter << 14", "gDuel_wStepCounter << 16")
                elif mutation == "byte-payload-store":
                    code = code.replace(
                        "object->field_00 = (s16)", "*(u8 *)&object->field_00 = (s16)"
                    )
                elif mutation == "wrong-penalty":
                    code = code.replace(
                        "record->stat_modifier - DUEL_STAT_PENALTY_PER_LEVEL;",
                        "record->stat_modifier - 2 * DUEL_STAT_PENALTY_PER_LEVEL;",
                    )
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragment)
                with tempfile.TemporaryDirectory(prefix="stat-request-local-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid apply_penalty_fragment(void) {\n"
                        " DuelCardRecord *record = &record_block.record;\n"
                        " DisplayObject *card; DuelEffectRequest *object;\n" + code + "\n}\n"
                    )
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
