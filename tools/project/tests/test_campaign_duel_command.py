"""Campaign command witnesses: 16,384 cases per ILP32 optimization level.

Compile the actual command and word reader as separate translation units.
A linker wrapper observes the call boundary, then either returns the real
reader's halfword or an opaque int and changes the opponent before bank lookup.
Only stream 0 (DuelEffectChannel.text_00, as used by text_box_build_step.c) is
selected; this does not test or promise malformed-index safety.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/text_start_campaign_duel.c"
HELPER = REPOSITORY / "src/game/func_80036D3C.c"
CASE_COUNT = 256 * 4 * 8 * 2
START = """
.text
.globl _start
_start:
    call main
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

# Preserve the retail adjacency, including untouched bytes around every store.
GLOBAL_OFFSETS = {
    "D_8009B26C": 0x00C,
    "D_8009B360": 0x100,
    "gDuel_bOpponentID": 0x101,
    "gDuel_bTerrain": 0x104,
    "D_8009B368": 0x108,
    "D_8009B369": 0x109,
    "D_8009B36A": 0x10A,
    "D_8009B370": 0x110,
    "D_8009B372": 0x112,
    "D_8009B374": 0x114,
}

WITNESS = r"""
#include "src/types.h"
#include "src/game/text_start_campaign_duel.h"
#include "src/game/func_80036D3C.h"
#include "src/game/ai_opponent_data.h"

u8 global_bytes[0x140] __attribute__((aligned(4)));
static u8 before_call[0x140], after_call[0x140];
typedef struct {
    u8 before[16];
    DuelEffectChannel channel;
    u8 after[16];
} GuardedChannel;
static GuardedChannel object, expected_before, expected_after;
static u8 stream[48], saved_stream[48];
static u8 *command;
static s32 fixture_error, calls, opaque_mode, word_index;
static u32 serial;
static const u16 words[] = {
    0, 1, 0x7FFF, 0x8000, 0x00FF, 0xFF00, 0xFFFF, 0x7280
};
static const s32 opaque_returns[] = {
    0x10000, 0x12340001, -32769, 0x76548000,
    (-2147483647 - 1) + 255, 0x2345FF00, -1, -36224
};

static void fill(void *out, u32 size, u32 seed)
{
    volatile u8 *p = out;
    u32 i;
    for (i = 0; i < size; i++) p[i] = seed + i * 37;
}

static void copy(void *out, const void *in, u32 size)
{
    volatile u8 *p = out;
    const volatile u8 *q = in;
    while (size--) *p++ = *q++;
}

static s32 equal(const void *left, const void *right, u32 size)
{
    const volatile u8 *p = left, *q = right;
    while (size--) if (*p++ != *q++) return 0;
    return 1;
}

static void halfword(u8 *out, u16 value)
{
    out[0] = value;
    out[1] = value >> 8;
}

static u16 bank(u8 opponent)
{
    switch (opponent) {
    case 9: case 10: case 11: case 12:
    case 13: case 14: case 15: case 16: return 0x7280;
    case 17: return 0x7290;
    case 38: return 0x72B0;
    default: return 0x72A0;
    }
}

int __real_func_80036D3C(DuelEffectChannel *channel);

int __wrap_func_80036D3C(DuelEffectChannel *channel)
{
    s32 result;
    calls++;
    if (channel != &object.channel || calls != 1) {
        fixture_error = 10;
        return 0;
    }
    /* This includes the reset, four unsigned stores, and absence of premature
       BGM/bank/mode publications. Compare before invoking the real reader. */
    if (!equal(global_bytes, before_call, sizeof(global_bytes))) {
        fixture_error = 11;
        return 0;
    }
    if (!equal(&object, &expected_before, sizeof(object))) {
        fixture_error = 12;
        return 0;
    }
    if (!equal(stream, saved_stream, sizeof(stream))) {
        fixture_error = 13;
        return 0;
    }
    result = __real_func_80036D3C(channel);
    if (result != words[word_index] ||
        !equal(&object, &expected_after, sizeof(object)) ||
        !equal(stream, saved_stream, sizeof(stream))) fixture_error = 14;
    if (opaque_mode) {
        gDuel_bOpponentID = (s8)(command[0] ^ 0x80);
        return opaque_returns[word_index];
    }
    return result;
}

static void setup(u32 value, u32 rotation)
{
    static const u8 offsets[] = {0, 53, 127, 193};
    u8 opponent;
    u32 i;
    fixture_error = calls = 0;
    fill(global_bytes, sizeof(global_bytes), serial + 0x63);
    fill(&object, sizeof(object), serial + 0xA7);
    fill(stream, sizeof(stream), serial + 0xD1);
    command = stream + 16 + (serial & 3);
    for (i = 0; i < 4; i++)
        command[i] = value + offsets[(i + rotation) & 3];
    halfword(command + 4, words[word_index]);
    object.channel.text_00 = command;
    object.channel.stream_58 = 0;
    copy(saved_stream, stream, sizeof(stream));
    copy(&expected_before, &object, sizeof(object));
    expected_before.channel.text_00 = command + 4;
    copy(&expected_after, &object, sizeof(object));
    expected_after.channel.text_00 = command + 6;

    copy(before_call, global_bytes, sizeof(global_bytes));
    before_call[0x100] = 0xFF;
    before_call[0x101] = command[0];
    halfword(before_call + 0x110, command[1]);
    halfword(before_call + 0x112, command[2]);
    before_call[0x104] = command[3];
    copy(after_call, before_call, sizeof(after_call));
    opponent = opaque_mode ? command[0] ^ 0x80 : command[0];
    after_call[0x101] = opponent;
    halfword(after_call + 0x10A,
             opaque_mode ? (u16)opaque_returns[word_index] : words[word_index]);
    halfword(after_call + 0x114, bank(opponent));
    after_call[0x108] = 2;
    after_call[0x109] = 0;
    after_call[0x00C] = 3;
}

s32 main(void)
{
    u32 value, rotation, cases = 0;
    if (sizeof(void *) != 4 || sizeof(int) != 4 ||
        sizeof(DuelEffectChannel) != 0x64) return 90;
    for (opaque_mode = 0; opaque_mode < 2; opaque_mode++)
    for (word_index = 0; word_index < 8; word_index++)
    for (rotation = 0; rotation < 4; rotation++)
    for (value = 0; value < 256; value++) {
        serial = cases;
        setup(value, rotation);
        Text_StartCampaignDuel(&object.channel);
        if (fixture_error) return fixture_error;
        if (calls != 1) return 15;
        /* Bank-specific failure lets the safe mutation prove an exact
           semantic rejection rather than accepting a signal or any error. */
        if (!equal(global_bytes + 0x114, after_call + 0x114, 2)) return 20;
        if (!equal(global_bytes, after_call, sizeof(global_bytes))) return 21;
        if (!equal(&object, &expected_after, sizeof(object))) return 22;
        if (!equal(stream, saved_stream, sizeof(stream))) return 23;
        cases++;
    }
    return cases == EXPECTED_CASE_COUNT ? 0 : 91;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class CampaignDuelCommandTests(unittest.TestCase):
    def run_witness(self, optimization: str, mutation: bool = False):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(
            prefix="campaign-duel-command-", dir=REPOSITORY / "tmp"
        ) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation:
                text = SOURCE.read_text()
                old = "(u32)(x - 9) < 8"
                self.assertEqual(text.count(old), 1)
                # ID 17 is subsequently overridden, so this upper-bound error
                # would be masked. Move the lower bound to wrongly admit 8.
                text = text.replace(old, "(u32)(x - 8) < 9")
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda m: '#include "' + str((SOURCE.parent / m[1]).resolve()) + '"',
                    text,
                    flags=re.MULTILINE,
                )
                source = directory / "wrong-bank-boundary.c"
                source.write_text(text)
            witness = directory / "witness.c"
            witness.write_text(f"#define EXPECTED_CASE_COUNT {CASE_COUNT}\n" + WITNESS)
            startup = directory / "start.S"
            startup.write_text(START)
            binary = directory / "witness"
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            aliases = [
                f"-Wl,--defsym,{name}=global_bytes+{offset}"
                for name, offset in GLOBAL_OFFSETS.items()
            ]
            compiled = subprocess.run(
                [
                    "cc", "-m32", optimization, "-std=c99", "-ffreestanding",
                    "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
                    "-fno-stack-protector", "-nostdlib", "-I", str(REPOSITORY),
                    str(source), str(HELPER), str(witness), str(startup),
                    "-Wl,--wrap=func_80036D3C", *aliases, "-o", str(binary),
                ],
                capture_output=True, text=True, timeout=60, env=environment,
            )
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            return subprocess.run(
                [str(binary)], capture_output=True, text=True, timeout=60,
                env=environment,
            )

    def test_actual_command_matches_witness_at_o0_and_o2(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization, cases=CASE_COUNT):
                result = self.run_witness(optimization)
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_wrong_bank_boundary_is_rejected(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization, mutation=True)
                self.assertEqual(result.returncode, 20, result.stderr)
