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
SOURCE = REPOSITORY / "src/game/duel_load_package_stage.c"

START = r"""
.text
.globl _start
_start:
    andl $-16, %esp
    call main
    movl %eax, %ebx
    movl $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

# The oracle is a table of stage contracts applied to raw descriptor bytes,
# independent of the implementation's switch and flag-expression spelling.
WITNESS = r"""
#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#define D_8009B118_IS_POINTER_IN_DATA
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "src/types.h"
#include "src/game/duel_load_package_stage.h"
#include "src/game/duel_check_ritual.h"
#include "src/game/duel_card_checks.h"
#include "src/game/graphics_frame.h"
#include "src/game/high_memory_addresses.h"
#include "src/unmatched.h"

volatile u32 D_8009B0F4;
static u8 buffer[8192], alternate[8192], stage7[16], stage11[16];
u8 *D_8009B118 = buffer;
u8 *D_800101DC = stage7;
u8 *D_80010000 = stage11;
u16 gDuel_awEquipTable[16], gDuel_aFusionTable[16], gDuel_awRitualData[16];
u8 D_801A8000[16], D_801A9800[16];
RECT D_800E9D70[2];

typedef char pointer_must_be_ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef char rect_must_be_eight_bytes[sizeof(RECT) == 8 ? 1 : -1];

static struct {
    u8 before[16];
    FileTransferDescriptor descriptor;
    u8 after[16];
} actual;
static u8 expected[sizeof(actual)];
static u8 expected_rects[sizeof(D_800E9D70)];
static u8 submitted_rect[sizeof(RECT)];
static RECT *submitted_address;
static u32 *submitted_buffer;
static u32 submitted_flags, calls, mutation_mode, replacement_flags;

static void copy_bytes(void *destination, const void *source, u32 size)
{
    u8 *d = destination;
    const u8 *s = source;
    u32 i;
    for (i = 0; i < size; i++) d[i] = s[i];
}

static s32 equal_bytes(const void *first, const void *second, u32 size)
{
    const u8 *a = first, *b = second;
    u32 i;
    for (i = 0; i < size; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

static void put16(u8 *bytes, u32 offset, u32 value)
{
    bytes[offset] = value;
    bytes[offset + 1] = value >> 8;
}

static void put32(u8 *bytes, u32 offset, u32 value)
{
    put16(bytes, offset, value);
    put16(bytes, offset + 2, value >> 16);
}

int LoadImage2(RECT *rect, u32 *pixels)
{
    calls++;
    submitted_address = rect;
    submitted_buffer = pixels;
    submitted_flags = D_8009B0F4;
    copy_bytes(submitted_rect, rect, sizeof(RECT));
    if (mutation_mode & 1) D_8009B0F4 = replacement_flags;
    if (mutation_mode & 2) D_8009B118 = alternate;
    return -123;
}

enum {
    BUFFER, EQUIP, FUSION, RITUAL, STAGE7, ARENA8, ARENA9, STAGE11
};

static const struct {
    u32 sectors;
    u16 counter, field_32;
    u8 destination, image, submits;
} contracts[13] = {
    {64, 0x300, 0x100, BUFFER, 1, 0},
    {4,  0,     0,     BUFFER, 0, 0},
    {5,  0,     0,     EQUIP,  0, 1},
    {32, 0,     0,     FUSION, 0, 0},
    {1,  0,     0,     RITUAL, 0, 0},
    {2,  0,     0,     BUFFER, 0, 0},
    {32, 0x200, 0x100, BUFFER, 1, 1},
    {44, 0,     0,     STAGE7, 0, 0},
    {3,  0,     0,     ARENA8, 0, 0},
    {3,  0,     0,     ARENA9, 0, 0},
    {8,  0x340, 0,     BUFFER, 1, 0},
    {5,  0,     0,     STAGE11,0, 0},
    {32, 0x280, 0x100, BUFFER, 1, 0}
};

static s32 check_case(s32 stage, u32 initial_flags, u32 mode)
{
    u32 i, destination, final_flags = initial_flags;
    u32 valid = (u32)stage < 13;
    u32 submits = valid && contracts[stage].submits;
    u8 *bytes = (u8 *)&actual;
    u8 *descriptor = expected + 16;
    u8 *final_buffer = buffer;
    u32 destinations[8];

    for (i = 0; i < sizeof(actual); i++)
        bytes[i] = (i * 37 + (initial_flags >> 16) + (u32)stage * 11) ^ 0xA5;
    copy_bytes(expected, &actual, sizeof(actual));
    for (i = 0; i < sizeof(D_800E9D70); i++)
        ((u8 *)D_800E9D70)[i] = 0xB7 ^ (i * 23 + initial_flags);
    copy_bytes(expected_rects, D_800E9D70, sizeof(D_800E9D70));
    D_8009B0F4 = initial_flags;
    D_8009B118 = buffer;
    calls = 0;
    submitted_address = 0;
    submitted_buffer = 0;
    submitted_flags = 0;
    mutation_mode = mode;
    replacement_flags = initial_flags ^ 0xFEDCBA98u;

    if (submits) {
        put16(expected_rects, 0, stage == 2 ? 256 : 0);
        put16(expected_rects, 2, 240);
        put16(expected_rects, 4, 256);
        put16(expected_rects, 6, stage == 2 ? 16 : 8);
        if (mode & 1) final_flags = replacement_flags;
        if (mode & 2) final_buffer = alternate;
    }
    destinations[BUFFER] = (u32)final_buffer;
    destinations[EQUIP] = (u32)gDuel_awEquipTable;
    destinations[FUSION] = (u32)gDuel_aFusionTable;
    destinations[RITUAL] = (u32)gDuel_awRitualData;
    destinations[STAGE7] = (u32)stage7;
    destinations[ARENA8] = (u32)D_801A8000;
    destinations[ARENA9] = (u32)D_801A9800;
    destinations[STAGE11] = (u32)stage11;
    if (valid) {
        destination = destinations[contracts[stage].destination];
        put32(descriptor, 0x08, destination);
        put32(descriptor, 0x0C, destination + (contracts[stage].image ? 2048 : 0));
        put32(descriptor, 0x1C, contracts[stage].sectors * 2048);
        descriptor[0x46] = contracts[stage].image ? 2 : 1;
        /* Bits 16, 17 and 21 are the only state bits the contract changes. */
        final_flags &= ~((1u << 16) | (1u << 17) | (1u << 21));
        if (contracts[stage].image) {
            final_flags |= 1u << 16;
            put16(descriptor, 0x04, 64);
            put16(descriptor, 0x06, 16);
            put16(descriptor, 0x30, contracts[stage].counter);
            put16(descriptor, 0x32, contracts[stage].field_32);
        }
    }

    Duel_LoadPackageStage(&actual.descriptor, stage);
    if (!equal_bytes(&actual, expected, sizeof(actual))) return 20;
    if (D_8009B0F4 != final_flags) return 22;
    if (D_8009B118 != final_buffer) return 23;
    if (!equal_bytes(D_800E9D70, expected_rects, sizeof(D_800E9D70))) return 24;
    if (calls != submits) return 25;
    if (submits) {
        if (submitted_address != &D_800E9D70[0]) return 26;
        if (submitted_buffer != (u32 *)buffer) return 27;
        if (!equal_bytes(submitted_rect, expected_rects, sizeof(RECT))) return 28;
        if (submitted_flags != initial_flags) return 29;
    }
    if (D_800101DC != stage7 || D_80010000 != stage11) return 30;
    return 0;
}

s32 main(void)
{
    static const s32 stages[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        -1, -2, (-2147483647 - 1), 13, 14, 2147483647
    };
    u32 high, index, mode, flags, cases = 0;
    s32 result;
    volatile u32 plain_address = (u32)&D_8009B0F4;
    volatile u32 alias_address = (u32)&D_8009B0F4_abs;
    if (plain_address != alias_address) return 90;
    for (high = 0; high < 65536; high++) {
        flags = (high << 16) | (((high * 40503u) ^ 0xA5D3u) & 0xFFFFu) | 1u;
        for (index = 0; index < sizeof(stages) / sizeof(stages[0]); index++) {
            for (mode = 0; mode < ((stages[index] == 2 || stages[index] == 6) ? 4u : 1u); mode++) {
                result = check_case(stages[index], flags, mode);
                if (result) return result;
                cases++;
            }
        }
    }
    if (cases != 1638400) return 91;
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class DuelPackageStageTests(unittest.TestCase):
    def run_witness(self, optimization: str, mutation: bool = False):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(
            prefix="duel-package-stage-", dir=REPOSITORY / "tmp"
        ) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation:
                text = SOURCE.read_text()
                old = "do { enabled = 0x10000; } while (0);"
                self.assertEqual(text.count(old), 1)
                text = text.replace(old, "do { enabled = 0x20000; } while (0);", 1)
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "' + str((SOURCE.parent / match[1]).resolve()) + '"',
                    text,
                    flags=re.MULTILINE,
                )
                source = directory / "wrong-stage10-mask.c"
                source.write_text(text)
            witness = directory / "witness.c"
            witness.write_text(WITNESS)
            startup = directory / "start.S"
            startup.write_text(START)
            # Old GCC accepts the unrelated incomplete extern array in
            # graphics_frame.h. Complete its real type before that declaration
            # for modern GCC, without replacing any game declarations/layouts.
            layout = (REPOSITORY / "src/game/graphics_frame_buffer.h").read_text()
            definition = re.search(r"struct GraphicsFrameBuffer \{.*?\n\};", layout, re.DOTALL)
            self.assertIsNotNone(definition)
            compatibility = directory / "host-types.h"
            compatibility.write_text(
                '#ifndef __ASSEMBLER__\n#include "src/types.h"\n'
                '#include "src/psyq/libgte.h"\n#include "src/psyq/libgpu.h"\n'
                '#include "src/psyq/libgs.h"\n'
                + definition[0] + "\n#endif\n"
            )
            binary = directory / "witness"
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            # Separate translation units preserve external-call reload behavior;
            # only the test startup is x86 assembly, never the game function.
            compiled = subprocess.run(
                [
                    "cc", "-m32", optimization, "-std=gnu99", "-nostdlib",
                    "-fno-pie", "-no-pie", "-fno-stack-protector", "-ffreestanding",
                    "-fno-builtin", "-fno-strict-aliasing", "-I", str(REPOSITORY),
                    "-include", str(compatibility),
                    str(source), str(witness), str(startup),
                    "-Wl,--defsym,D_8009B0F4_abs=D_8009B0F4", "-o", str(binary),
                ],
                capture_output=True, text=True, timeout=60, env=environment,
            )
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            return subprocess.run(
                [str(binary)], capture_output=True, text=True, timeout=60, env=environment
            )

    def test_all_stages_and_65536_flag_halfwords_at_o0_and_o2(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization)
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_stage10_wrong_enabled_mask_is_rejected_without_crashing(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization, mutation=True)
                self.assertEqual(result.returncode, 22, result.stderr)
