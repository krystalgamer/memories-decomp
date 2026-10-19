"""Separately compiled ILP32 reward-compaction witnesses, without host libc.

Every occupancy mask is checked under eight ID/eligibility configurations.
An independent output-array filter predicts the entire guarded arena, and a
second sweep distinguishes each valid card's byte from its adjacent bytes.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/duel_reward_setup.c"
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
#include "src/game/duel_reward_setup.h"

enum {
    VALID_CARD_COUNT = 722, SLOT_COUNT = 16, TABLE_OFFSET = 0x56C,
    GUARD_SIZE = 64, ARENA_SIZE = TABLE_OFFSET + SLOT_COUNT * 2 + GUARD_SIZE,
    TOTAL_SIZE = GUARD_SIZE + ARENA_SIZE
};
typedef char require_ilp32[sizeof(void *) == 4 && sizeof(u32) == 4 ? 1 : -1];
typedef char require_halfword[sizeof(s16) == 2 ? 1 : -1];
typedef char require_table_size[DUEL_RECENT_CARD_DROP_COUNT == SLOT_COUNT ? 1 : -1];
typedef char require_distance[DUEL_RECENT_DROPS_CHEST_DISTANCE == TABLE_OFFSET ? 1 : -1];

/* The linker exposes arena at the chest, inside this single guarded object. */
u8 guarded_arena[TOTAL_SIZE] __attribute__((aligned(16)));
extern u8 arena[];
static u8 expected[TOTAL_SIZE];
static s16 input[SLOT_COUNT];
static u32 cases;

static u8 *chest(void)
{
    return guarded_arena + GUARD_SIZE;
}

static void initialize(void)
{
    u32 i;
    for (i = 0; i < TOTAL_SIZE; i++)
        guarded_arena[i] = (u8)(0x39 + i * 37 + (i >> 3));
}

static void set_eligibility(s32 card, s32 keep, u32 salt)
{
    chest()[card - 1] = keep ? (u8)(1 + salt % 255) : 0;
}

static s32 check_case(void)
{
    s16 output[SLOT_COUNT] = {0};
    u32 i, length = 0;
    for (i = 0; i < SLOT_COUNT; i++) {
        u16 value = (u16)input[i];
        chest()[TABLE_OFFSET + i * 2] = (u8)value;
        chest()[TABLE_OFFSET + i * 2 + 1] = (u8)(value >> 8);
    }
    for (i = 0; i < TOTAL_SIZE; i++) expected[i] = guarded_arena[i];

    /* Read only the original input and snapshot, never an in-place cursor. */
    for (i = 0; i < SLOT_COUNT; i++)
        if (input[i] && expected[GUARD_SIZE + input[i] - 1])
            output[length++] = input[i];
    for (i = 0; i < SLOT_COUNT; i++) {
        u16 value = (u16)output[i];
        expected[GUARD_SIZE + TABLE_OFFSET + i * 2] = (u8)value;
        expected[GUARD_SIZE + TABLE_OFFSET + i * 2 + 1] = (u8)(value >> 8);
    }

    func_80032370();
    for (i = GUARD_SIZE + TABLE_OFFSET; i < GUARD_SIZE + TABLE_OFFSET + 32; i++)
        if (guarded_arena[i] != expected[i]) return 21;
    for (i = 0; i < TOTAL_SIZE; i++)
        if (guarded_arena[i] != expected[i]) return 22;
    cases++;
    return 0;
}

static s32 occupancy_sweep(void)
{
    static const s16 distinct[SLOT_COUNT] = {
        1, 722, 2, 721, 3, 720, 17, 255, 256, 257, 511, 512, 513, 100, 400, 600
    };
    u32 mask, profile, slot, eligibility;
    s32 result, card;
    for (mask = 0; mask < 65536; mask++)
    for (profile = 0; profile < 8; profile++) {
        switch (profile & 3) {
        case 0: eligibility = 0xFFFF; break;
        case 1: eligibility = 0; break;
        case 2: eligibility = 0xA55A; break;
        default: eligibility = ((mask * 40503 + 97) ^ (mask >> 3)) & 0xFFFF; break;
        }
        for (slot = 0; slot < SLOT_COUNT; slot++) {
            /* The second family repeats both endpoint IDs and two interiors. */
            card = distinct[profile < 4 ? slot : (slot * 3) % 4];
            input[slot] = mask & (1u << slot) ? card : 0;
            set_eligibility(card, eligibility & (1u << slot), mask + slot * 41);
        }
        result = check_case();
        if (result) return result;
    }
    return 0;
}

static s32 card_index_sweep(void)
{
    s32 card, position, mode, slot, result, keep;
    for (card = 1; card <= VALID_CARD_COUNT; card++)
    for (position = 0; position < SLOT_COUNT; position++)
    for (mode = 0; mode < 4; mode++) {
        keep = mode & 1;
        for (slot = 0; slot < SLOT_COUNT; slot++)
            input[slot] = slot == position || (mode >= 2 && slot % 3 == 0) ? card : 0;
        /* Neighbor bytes have opposite truth values, including outside the
         * chest at IDs 1 and 722; those bytes must also remain untouched. */
        guarded_arena[GUARD_SIZE + card - 2] = keep ? 0 : 0x80;
        guarded_arena[GUARD_SIZE + card] = keep ? 0 : 0xFF;
        set_eligibility(card, keep, card + position * 17);
        result = check_case();
        if (result) return result;
    }
    return 0;
}

s32 main(void)
{
    s32 result;
    volatile u32 linked_arena = (u32)arena;
    volatile u32 linked_table = (u32)gDuel_awRecentCardDrops;
    volatile u32 actual_arena = (u32)(guarded_arena + GUARD_SIZE);
    if (linked_arena != actual_arena || linked_table != actual_arena + TABLE_OFFSET)
        return 90;
    if (linked_arena & 15) return 91;
    initialize();
    result = occupancy_sweep();
    if (result) return result;
    result = card_index_sweep();
    if (result) return result;
    return cases == 65536u * 8 + 722u * 16 * 4 ? 0 : 92;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class RewardCompactionTests(unittest.TestCase):
    def run_witness(self, optimization: str, mutation: bool = False):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(
            prefix="reward-compaction-", dir=REPOSITORY / "tmp"
        ) as temporary:
            directory = Path(temporary)
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            # The unit also holds the reward-table transfer, whose graphics
            # headers are PSX-only; compile the compaction pass on its own
            # against the header that declares it.
            unit = SOURCE.read_text()
            text = ('#include "../types.h"\n#include "duel_reward_setup.h"\n' +
                    unit[unit.index("/*\n * Reward-drop compaction."):])
            source = directory / "reward-compaction.c"
            if mutation:
                self.assertEqual(text.count("*source = 0;"), 1)
                source = directory / "missing-source-clear.c"
                text = text.replace("*source = 0;", "(void)source;")
            source.write_text(text)
            witness = directory / "witness.c"
            witness.write_text(WITNESS)
            startup = directory / "start.S"
            startup.write_text(START)
            flags = [
                "cc", "-m32", optimization, "-std=gnu99", "-ffreestanding",
                "-fno-builtin", "-fno-strict-aliasing", "-fno-pie",
                "-fno-stack-protector", "-I", str(REPOSITORY),
                "-iquote", str(SOURCE.parent),
            ]
            objects = []
            for name, path in (("actual", source), ("oracle", witness), ("start", startup)):
                obj = directory / f"{name}.o"
                compiled = subprocess.run(
                    [*flags, "-c", str(path), "-o", str(obj)],
                    capture_output=True, text=True, timeout=60, env=environment,
                )
                self.assertEqual(compiled.returncode, 0, compiled.stderr)
                objects.append(str(obj))
            binary = directory / "witness"
            linked = subprocess.run(
                [
                    "cc", "-m32", "-no-pie", "-nostdlib", *objects,
                    "-Wl,--defsym,arena=guarded_arena+64",
                    "-Wl,--defsym,gDuel_awRecentCardDrops=arena+1388",
                    "-Wl,--defsym,gLibrary_abCardChest=arena",
                    "-o", str(binary),
                ],
                capture_output=True, text=True, timeout=60, env=environment,
            )
            self.assertEqual(linked.returncode, 0, linked.stderr)
            return subprocess.run(
                [str(binary)], capture_output=True, text=True, timeout=120,
                env=environment,
            )

    def test_actual_compaction_matches_filter_at_o0_and_o2(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization)
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_missing_source_clear_is_rejected(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization, mutation=True)
                self.assertEqual(result.returncode, 21, result.stderr)
