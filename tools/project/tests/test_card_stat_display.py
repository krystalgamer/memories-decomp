"""ILP32 witnesses for the card-type icon constructor, not allocator safety.

The real translation unit is linked separately from a byte-layout/table oracle.
Only valid card IDs and successful allocations are exercised. Freestanding x86
startup permits these tests without installing a 32-bit host libc.
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
SOURCE = REPOSITORY / "src/game/duel_card_type_icon.c"
CASE_COUNT = 111744
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

WITNESS = r"""
#include "src/types.h"
#include "src/game/duel_card_stat_display.h"
#include "src/game/display_object_config.h"
#include "src/game/display_object_core.h"
#include "src/game/display_object_helpers.h"
#include "src/game/duel_card.h"

enum { CARD_COUNT = 722, GUARD = 32, OBJECT_SIZE = 0x70 };
typedef struct {
    u8 before[GUARD];
    DisplayObject object;
    u8 after[GUARD];
} GuardedObject;
static GuardedObject actual, decoy;
static u8 expected[5][sizeof(GuardedObject)], saved_decoy[sizeof(GuardedObject)];
static u8 data[64], saved_data[64];
s32 gDuel_adwCardStats[CARD_COUNT];
static s32 saved_stats[CARD_COUNT];
static u32 serial, final_stats_word, texture, bit_pattern, helper_mode;
static s32 card_index, x, y, arg3, arg4, allocation_index, mutate_stats;
static s32 stage, error;

/* Indexed by the five type bits, independent of the implementation's switch. */
static const u8 texture_offsets[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 16, 32, 48, 16, 0, 0, 0, 0, 0, 0, 0, 0
};
static const u32 boundary_words[8] = {
    0, 1, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000, 0x00008000,
    0xFFFF7FFF, 0x1234ABCD
};
static const u16 texture_words[8] = {
    0, 0x260, 0x7FFF, 0x8000, 0xFFCF, 0xFFD0, 0xFFF0, 0xFFFF
};

static void copy_bytes(void *out, const void *in, u32 size)
{
    volatile u8 *dst = out;
    const volatile u8 *src = in;
    while (size--) *dst++ = *src++;
}

static s32 equal_bytes(const void *left, const void *right, u32 size)
{
    const volatile u8 *a = left, *b = right;
    while (size--) if (*a++ != *b++) return 0;
    return 1;
}

static void put16(u8 *p, u32 value)
{
    p[0] = value;
    p[1] = value >> 8;
}

static void put32(u8 *p, u32 value)
{
    put16(p, value);
    put16(p + 2, value >> 16);
}

static u32 get16(const u8 *p)
{
    return (u32)p[0] | ((u32)p[1] << 8);
}

static u32 get32(const u8 *p)
{
    return get16(p) | (get16(p + 2) << 16);
}

static void fail(s32 code)
{
    if (!error) error = code;
}

static void check_stage(s32 wanted, s32 snapshot)
{
    if (stage != wanted ||
        !equal_bytes(&actual, expected[snapshot], sizeof(actual)))
        fail(10 + wanted);
    stage++;
}

void func_800313E8(DisplayObject *object)
{
    (void)object;
    fail(30); /* Construction installs the callback but must not execute it. */
}

static void old_callback(u8 *object)
{
    (void)object;
    fail(31);
}

s32 DisplayObject_FindFreeGeneralSlot(void)
{
    check_stage(1, 0);
    return allocation_index;
}

void *DisplayObject_AcquireSlot(s32 index, s32 key)
{
    check_stage(2, 0);
    if (index != allocation_index || key != 1) fail(12);
    return &actual.object;
}

DisplayObjectConfigView *DisplayObject_ConfigureScreenSprite(
    DisplayObjectConfigView *object, s32 px, s32 py, s32 height,
    s32 width, s32 field_5C, s32 field_5D, s32 field_66,
    s32 field_40, s32 field_42)
{
    check_stage(3, 0);
    if ((void *)object != &actual.object || px != x || py != y ||
        height != 16 || width != 16 || field_5C != 0 ||
        field_5D != 0xC8 || field_66 != 11 || field_40 != 0x260 ||
        field_42 != 0xFC) fail(13);
    copy_bytes(&actual, expected[1], sizeof(actual));
    if (mutate_stats) gDuel_adwCardStats[card_index - 1] = final_stats_word;
    /* This result is deliberately distinct: the constructor retains allocation. */
    return (DisplayObjectConfigView *)&decoy.object;
}

void func_80042918(DisplayObject *object)
{
    check_stage(4, 2);
    if (object != &actual.object) fail(14);
    copy_bytes(&actual, expected[3], sizeof(actual));
}

s32 func_800428EC(u8 *object, s8 value)
{
    check_stage(5, 3);
    if ((void *)object != &actual.object || value != 10) fail(15);
    copy_bytes(&actual, expected[4], sizeof(actual));
    return 0x13579BDF;
}

static void setup(u32 kind, u32 sign, u32 pattern)
{
    u32 i;
    u8 *p;
    for (i = 0; i < sizeof(actual); i++)
        ((u8 *)&actual)[i] = (u8)(i * 73 + serial * 17 + 91);
    for (i = 0; i < sizeof(decoy); i++)
        ((u8 *)&decoy)[i] = (u8)(i * 37 + serial * 3);
    for (i = 0; i < sizeof(data); i++) data[i] = (u8)(i * 11 + serial);
    copy_bytes(saved_decoy, &decoy, sizeof(decoy));
    copy_bytes(saved_data, data, sizeof(data));
    final_stats_word = (kind << 26) | (sign << 31) |
                       ((serial * 0x19275U + 0x2A95681U) & 0x03FFFFFF);
    for (i = 0; i < CARD_COUNT; i++)
        gDuel_adwCardStats[i] = final_stats_word ^ 0x54000000U ^ (i * 7919U);
    gDuel_adwCardStats[card_index - 1] =
        mutate_stats ? final_stats_word ^ 0x04000000U : final_stats_word;
    copy_bytes(saved_stats, gDuel_adwCardStats, sizeof(saved_stats));
    saved_stats[card_index - 1] = final_stats_word;
    x = boundary_words[pattern];
    y = boundary_words[(pattern + 3) % 8];
    arg3 = boundary_words[(pattern + 5) % 8];
    arg4 = boundary_words[(pattern + 7) % 8];
    allocation_index = serial % 96;
    actual.object.update = old_callback;
    p = (u8 *)&actual.object;
    put32(p + 0x54, (u32)data);
    put32(p + 4, (serial * 0x19283U & ~0x08000000U) |
                  ((bit_pattern & 2) ? 0x08000000U : 0));
    put16(p + 8, (serial * 137U & 0xFFF7U) | ((bit_pattern & 1) ? 8 : 0));
    copy_bytes(expected[0], &actual, sizeof(actual));

    copy_bytes(expected[1], expected[0], sizeof(actual));
    p = expected[1] + GUARD;
    put16(p + 0x18, 8);
    put16(p + 0x1A, 8);
    put16(p + 0x30, x);
    put16(p + 0x32, y);
    put16(p + 0x3C, 16);
    put16(p + 0x3E, 16);
    put16(p + 0x40, texture);
    put16(p + 0x42, 0xFC);
    put16(p + 0x48, 8);
    put16(p + 0x4A, 8);
    p[0x5C] = 0;
    p[0x5D] = 0xC8;
    p[0x66] = 11;
    if (helper_mode & 1) {
        put32(p + 4, get32(p + 4) ^ 0xA8001234U);
        put16(p + 8, get16(p + 8) ^ 0x5AA8);
    }

    copy_bytes(expected[2], expected[1], sizeof(actual));
    p = expected[2] + GUARD;
    put16(p + 0x40, texture + texture_offsets[kind]);

    copy_bytes(expected[3], expected[2], sizeof(actual));
    p = expected[3] + GUARD;
    p[0x17] = 1;
    put16(p + 0x14, 0x7193);
    if (helper_mode & 2) {
        put32(p + 4, get32(p + 4) ^ 0x48008765U);
        put16(p + 8, get16(p + 8) ^ 0x9638);
        put16(p + 0x40, get16(p + 0x40) + 0xF123);
    }

    copy_bytes(expected[4], expected[3], sizeof(actual));
    p = expected[4] + GUARD;
    p[0x16] = 10;
    if (helper_mode & 1) {
        put32(p + 4, get32(p + 4) ^ 0x1800FF00U);
        put16(p + 8, get16(p + 8) ^ 0xC3C8);
        put16(p + 0x40, get16(p + 0x40) + 0x4312);
    }
    /* Deliberately nonzero values require all final stores after both helpers. */
    put32(p + 0x44, 0xA1B2C3D4);
    put32(p + 0x18, 0xDEADCAFE);
    put32(p + 0x24, (u32)old_callback);
    stage = 1;
    error = 0;
}

static s32 run_case(u32 kind, u32 sign, u32 pattern)
{
    DisplayObject *result;
    u8 final[sizeof(actual)];
    u8 *p = final + GUARD;
    setup(kind, sign, pattern);
    copy_bytes(final, expected[4], sizeof(actual));
    put32(p + 0x44, 0);
    put16(p + 0x18, (u32)arg3 & 0xFFFF);
    put16(p + 0x1A, (u32)arg4 & 0xFFFF);
    put32(p + 0x24, (u32)func_800313E8);
    put16(p + 8, get16(p + 8) & 0xFFF7);
    put32(p + 4, get32(p + 4) & 0xF7FFFFFF);
    result = func_80031574(card_index, x, y, arg3, arg4);
    if (error) return error;
    if (stage != 6) return 16;
    if (!equal_bytes(&actual, final, sizeof(actual))) return 20;
    if (result != &actual.object ||
        result->update != (DisplayObjectCallback)func_800313E8) return 21;
    if (!equal_bytes(gDuel_adwCardStats, saved_stats, sizeof(saved_stats))) return 22;
    if (!equal_bytes(data, saved_data, sizeof(data)) ||
        !equal_bytes(&decoy, saved_decoy, sizeof(decoy))) return 23;
    serial++;
    return 0;
}

s32 main(void)
{
    u32 kind, sign, pattern, base;
    s32 result;
    if (sizeof(void *) != 4 || sizeof(DisplayObject) != OBJECT_SIZE ||
        (u32)&actual.object - (u32)&actual != GUARD ||
        sizeof(actual) != GUARD * 2 + OBJECT_SIZE) return 90;
    /* All valid IDs, all types and both sign-bit values: 46,208 cases. */
    for (card_index = 1; card_index <= CARD_COUNT; card_index++)
    for (kind = 0; kind < 32; kind++)
    for (sign = 0; sign < 2; sign++) {
        texture = texture_words[card_index % 8];
        bit_pattern = card_index % 4;
        helper_mode = (card_index / 4) % 4;
        mutate_stats = card_index & 1;
        result = run_case(kind, sign, card_index % 8);
        if (result) return result;
    }
    /* Full boundary/bit/helper cross-product: 65,536 additional cases. */
    for (kind = 0; kind < 32; kind++)
    for (sign = 0; sign < 2; sign++)
    for (pattern = 0; pattern < 8; pattern++)
    for (base = 0; base < 8; base++)
    for (bit_pattern = 0; bit_pattern < 4; bit_pattern++)
    for (helper_mode = 0; helper_mode < 4; helper_mode++) {
        card_index = pattern & 1 ? CARD_COUNT : 1;
        texture = texture_words[base];
        mutate_stats = 1;
        result = run_case(kind, sign, pattern);
        if (result) return result;
    }
    return serial == 111744 ? 0 : 91;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class CardStatDisplayTests(unittest.TestCase):
    def run_witness(self, optimization: str, mutation: bool = False):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(
            prefix="card-stat-display-", dir=REPOSITORY / "tmp"
        ) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation:
                text = SOURCE.read_text()
                text, count = re.subn(
                    r"(case CARD_TYPE_TRAP:\s*"
                    r"\*\(u16 \*\)&object->field_40 \+= )0x20;",
                    r"\g<1>0x10;",
                    text,
                )
                self.assertEqual(count, 1, "negative control must alter only trap")
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "'
                    + str((SOURCE.parent / match[1]).resolve())
                    + '"',
                    text,
                    flags=re.MULTILINE,
                )
                source = directory / "wrong-trap-offset.c"
                source.write_text(text)
            witness = directory / "witness.c"
            witness.write_text(WITNESS)
            startup = directory / "start.S"
            startup.write_text(START)
            binary = directory / "witness"
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            compiled = subprocess.run(
                [
                    "cc", "-m32", optimization, "-std=c99", "-ffreestanding",
                    "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
                    "-fno-stack-protector", "-nostdlib", "-I", str(REPOSITORY),
                    str(source), str(witness), str(startup), "-o", str(binary),
                ],
                capture_output=True, text=True, timeout=60, env=environment,
            )
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            return subprocess.run(
                [str(binary)], capture_output=True, text=True, timeout=60,
                env=environment,
            )

    def test_actual_constructor_matches_table_oracle_at_o0_and_o2(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization, cases=CASE_COUNT):
                result = self.run_witness(optimization)
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_wrong_trap_offset_is_rejected_at_helper_boundary(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization, mutation=True)
                self.assertEqual(result.returncode, 14, result.stderr)
