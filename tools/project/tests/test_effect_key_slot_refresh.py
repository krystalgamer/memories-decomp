"""ILP32 oracle for the actual key-slot refresher and captured/live boundaries."""

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/model_effect_state.c"
SYMBOL = "Model_RefreshEffectKeySlotValues"
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
HEADERS = r"""
#include "src/types.h"
#include "src/game/model_transfer_state.h"
#include "src/game/model_transfer_flags.h"
#include "src/game/model_copy_slot_position.h"
"""
WITNESS = HEADERS + r"""
typedef struct { u8 before[16]; Key key; u8 after[16]; } Guarded;
typedef char ilp32[sizeof(void *) == 4 && sizeof(Key) == 40 ? 1 : -1];
typedef char key_offset[__builtin_offsetof(Guarded, key) == 16 ? 1 : -1];
typedef char guarded_size[sizeof(Guarded) == 72 ? 1 : -1];
static Guarded blocks[2];
static u8 expected[sizeof(blocks)];
Key *G32 gModel_pCurrentEffectKey;
u8 gModel_bEffectKeyCount;
static Key *expected_current;
typedef struct { s32 slot; u16 *G32 output; } Event;
static Event events[2];
static u32 event_count, seen, mutation_mode, failure, cases;
static const u16 slots[3][4] = {
    {0x8000, 0x7FFF, 0xFFFF, 0x2468},
    {0x0123, 0xFEDC, 0, 0xBEEF},
    {0xAAAA, 0x5555, 0xFFFF, 0x8000}
};

static u32 halfword(const u8 *p) { return p[0] | ((u32)p[1] << 8); }
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
static void mutate(u8 *key)
{
    if (mutation_mode == 1) {
        put16(key + 8, 1);
        put16(key + 14, 3);
    }
    if (mutation_mode == 2) put16(key + 14, 0);
}

void Model_CopySlotPosition(s32 slot, u16 *output)
{
    Event *event;
    u32 word;
    if (seen >= event_count) { failure = 1; return; }
    event = &events[seen++];
    if (slot != event->slot || output != event->output ||
        slot < 0 || slot >= 3) { failure = 2; return; }
    for (word = 0; word < 4; word++)
        put16((u8 *)output + word * 2, slots[slot][word]);
    if (seen == 1 && mutation_mode) {
        mutate((u8 *)&blocks[0].key);
        gModel_pCurrentEffectKey =
            mutation_mode == 1 ? &blocks[1].key : 0;
    }
}

static s32 run(u32 selection, u32 first, u32 second, u32 mutation)
{
    u8 *actual = (u8 *)blocks;
    u8 *key = (u8 *)&blocks[0].key;
    u8 *other = (u8 *)&blocks[1].key;
    u8 *predicted = expected + 16;
    Key *input;
    u8 count = selection == 3 ? 7 : 0;
    u32 i, channel, word;
    cases++;
    for (i = 0; i < sizeof(blocks); i++)
        actual[i] = (u8)(i * 37 + cases * 13);
    put16(key, 0); put16(key + 6, first);
    put16(key + 8, 2); put16(key + 14, second);
    put16(other, 1); put16(other + 6, 2);
    put16(other + 8, 2); put16(other + 14, 3);
    for (i = 0; i < sizeof(blocks); i++) expected[i] = actual[i];
    input = selection == 0 || selection == 3 ? &blocks[0].key : 0;
    gModel_pCurrentEffectKey = selection == 0 ? &blocks[1].key :
        (selection == 1 ? &blocks[0].key : 0);
    expected_current = gModel_pCurrentEffectKey;
    gModel_bEffectKeyCount = count;
    event_count = seen = failure = 0;
    mutation_mode = mutation;
    if (selection != 2) {
        for (channel = 0; channel < 2; channel++) {
            u32 kind = halfword(predicted + channel * 8 + 6);
            if (kind == 2 || kind == 3) {
                u32 slot = halfword(predicted + channel * 8);
                Event *event = &events[event_count++];
                if (slot >= 3) return 3;
                event->slot = slot;
                event->output = (u16 *)(key + 16 + channel * 8);
                for (word = 0; word < 4; word++)
                    put16(predicted + 16 + channel * 8 + word * 2,
                          slots[slot][word]);
                if (event_count == 1 && mutation_mode) {
                    mutate(predicted);
                    expected_current =
                        mutation_mode == 1 ? &blocks[1].key : 0;
                }
            }
        }
    }
    Model_RefreshEffectKeySlotValues(input);
    if (failure || seen != event_count ||
        gModel_pCurrentEffectKey != expected_current ||
        gModel_bEffectKeyCount != count) return 4;
    for (i = 0; i < sizeof(blocks); i++)
        if (actual[i] != expected[i]) return 5;
    return 0;
}

static s32 report(void)
{
    char text[24] = "cases=", digits[10];
    u32 value = cases, n = 0, length = 6;
    s32 written;
    do { digits[n++] = '0' + value % 10; value /= 10; } while (value);
    while (n) text[length++] = digits[--n];
    text[length++] = '\n';
    __asm__ volatile("int $0x80" : "=a"(written)
                     : "0"(4), "b"(1), "c"(text), "d"(length)
                     : "memory", "cc");
    return written == (s32)length ? 0 : 6;
}

int main(void)
{
    static const u32 special[6][4] = {
        {0, 2, 3, 0}, {1, 3, 2, 0}, {2, 2, 3, 0},
        {3, 3, 2, 0}, {1, 2, 1, 1}, {0, 2, 3, 2}
    };
    u32 i, channel, kind;
    s32 result = 0;
    for (i = 0; i < 6; i++) {
        result = run(special[i][0], special[i][1],
                     special[i][2], special[i][3]);
        if (result) goto done;
    }
    for (channel = 0; channel < 2; channel++) {
        for (kind = 0; kind < 65536; kind++) {
            result = run(0, channel == 0 ? kind : 1,
                         channel == 1 ? kind : 1, 0);
            if (result) goto done;
        }
    }
done:
    if (report() != 0) return 6;
    return result;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine() in ("x86_64", "i386", "i686")
    and shutil.which("cc"),
    "requires a Linux x86 C compiler and ILP32 execution",
)
class EffectKeySlotRefreshTests(unittest.TestCase):
    def run_witness(self, optimization, mutation=None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        text = SOURCE.read_text()
        body = re.search(
            r"(?ms)^void " + SYMBOL + r"\(Key \*key\)\s*\{.*?^\}", text
        )
        macro = re.search(r"(?m)^#define KEY_BYTES\(key\).*$", text)
        self.assertIsNotNone(body)
        self.assertIsNotNone(macro)
        code = body.group(0)
        if mutation is not None:
            replacements = {
                "short-stride": (
                    "i * sizeof(u16[4])", "i * sizeof(u16[3])"
                ),
                "ignore-explicit-key": (
                    "u8 *data = KEY_BYTES(key);",
                    "u8 *data = KEY_BYTES(gModel_pCurrentEffectKey);",
                ),
            }
            before, after = replacements[mutation]
            self.assertEqual(code.count(before), 1)
            code = code.replace(before, after)
        with tempfile.TemporaryDirectory(
            prefix="key-slot-refresh-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = dict(os.environ, TMPDIR=str(directory))
            source = directory / "game.c"
            source.write_text(HEADERS + macro.group(0) + "\n" + code + "\n")
            witness = directory / "witness.c"
            witness.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            binary = directory / "witness"
            result = subprocess.run(
                [
                    shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                    "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                    "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                    "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                    str(source), str(witness), str(start), "-o", str(binary),
                ],
                cwd=ROOT, env=environment, text=True,
                capture_output=True, timeout=60,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run(
                [str(binary)], cwd=ROOT, env=environment,
                text=True, capture_output=True, timeout=60,
            )

    def test_all_kind_words_key_selection_and_live_records(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(result.stdout, "cases=131078\n")

    def test_narrow_resolved_stride_is_rejected(self):
        result = self.run_witness("-O2", "short-stride")
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(result.stdout, "cases=1\n")

    def test_ignoring_the_explicit_key_is_rejected(self):
        result = self.run_witness("-O2", "ignore-explicit-key")
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(result.stdout, "cases=1\n")


if __name__ == "__main__":
    unittest.main()
