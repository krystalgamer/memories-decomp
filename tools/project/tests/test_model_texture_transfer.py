"""Independent ILP32 runtime oracle for the retail 0x80056D7C stage program."""

from __future__ import annotations

from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/model_texture_transfer.c"
COPIER = REPOSITORY / "src/game/model_word_memory.c"
SCRATCH = REPOSITORY / "tmp/astra/model-texture-transfer-witness"
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

# The oracle uses retail byte offsets and a table, not the implementation's
# structs/switch. Canonical headers still enforce the actual caller's ABI.
WITNESS = r"""
#include "src/types.h"
#include "src/game/file_transfer.h"
#include "src/game/model.h"
#include "src/game/high_memory_addresses.h"
#include "src/psyq/libgte.h"
#include "src/psyq/libgpu.h"

typedef char ilp32[sizeof(void *) == 4 && sizeof(int) == 4 ? 1 : -1];
typedef char descriptor_size[sizeof(FileTransferDescriptor) == 0x48 ? 1 : -1];
typedef char slot_size[sizeof(ModelSlot) == 0xE20 ? 1 : -1];

enum { GUARD = 32, DESC = 0x48 + 64, SLOTS = 3 * 0xE20 + 64,
       RAW = 0x1400 + 64, COPY = 1, IMAGE = 2, HELPER = 3,
       MISMATCH = 71 };
u8 slots[SLOTS] __attribute__((aligned(16)));
u8 raw[RAW] __attribute__((aligned(16)));
__asm__(".globl D_800F2C40\n.set D_800F2C40, slots+32\n"
        ".globl D_801DD000\n.set D_801DD000, raw+32\n");
u8 D_801DD800[0x800], D_801DE000[0x400], D_801A8000[0x1000];
u8 D_800F5694[1];
static u8 destinations[6][32], buffers[2][0x1000];
u8 *D_80010000, *D_80010004, *D_8001000C, *D_80010010;
s32 D_80010008, D_80010014, D_80010018;
u8 *D_8009B118;
volatile u32 D_8009B0F4_abs;
void func_80056D7C(FileTransferDescriptor *, s32);
void __real_func_8005B620(s32 *, const s32 *, u32);

static u8 descriptor[DESC] __attribute__((aligned(16)));
static u8 expected_descriptor[DESC], expected_slots[SLOTS], expected_raw[RAW];
static u32 expected_flags;
static u8 *expected_buffer;
static s32 entry_index, current_stage, mutation;
static u32 cases, image_calls, copy_calls, helper_calls;
static s32 event_count, event_cursor;
typedef struct {
    s32 kind;
    u32 a, b, c;
    s16 rect[4];
    u8 descriptor[DESC], slots[SLOTS], raw[RAW];
    u32 flags;
    u8 *buffer;
} Event;
static Event events[2];

static void bytes(void *out, const void *in, u32 size)
{
    u8 *d = out;
    const u8 *s = in;
    while (size--) *d++ = *s++;
}
static s32 equal(const void *left, const void *right, u32 size)
{
    const u8 *a = left, *b = right;
    while (size--) if (*a++ != *b++) return 0;
    return 1;
}
static void fill(u8 *out, u32 size, u32 seed)
{
    u32 i;
    for (i = 0; i < size; i++) out[i] = (u8)(seed + i * 37 + (i >> 4));
}
static void put16(u8 *p, u32 n)
{
    p[0] = n; p[1] = n >> 8;
}
static void put32(u8 *p, u32 n)
{
    put16(p, n); put16(p + 2, n >> 16);
}
static u32 get16(const u8 *p) { return p[0] | ((u32)p[1] << 8); }
static u32 get32(const u8 *p) { return get16(p) | (get16(p + 2) << 16); }
static void write_text(const char *text, u32 size)
{
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(text), "d"(size)
                     : "memory", "cc");
}
static void number(u32 value)
{
    char out[12], reverse[10];
    u32 n = 0, length = 0;
    do { reverse[n++] = '0' + value % 10; value /= 10; } while (value);
    while (n) out[length++] = reverse[--n];
    out[length++] = '\n';
    write_text(out, length);
}
static void fail(u32 reason) __attribute__((noreturn));
static void fail(u32 reason)
{
    write_text("witness mismatch: ", 18); number(reason);
    write_text("case: ", 6); number(cases);
    write_text("stage: ", 7); number((u32)current_stage);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(MISMATCH) : "memory");
    __builtin_unreachable();
}

/* External calls may change the descriptor, flags, buffers, or either slot.
 * The slot/index/alternate belong to entry, but post-call field reads are live.
 * Copy mutation is deliberately AFTER the real 64-word copy and BEFORE the
 * caller's ordinary seven-word assignment. Helper mutation is after E11=4. */
static void effects(s32 kind, u8 *d, u8 *s, u8 *r, u32 *flags, u8 **buffer)
{
    u8 *slot = s + GUARD + entry_index * 0xE20;
    u8 *other = s + GUARD + (1 - entry_index) * 0xE20;
    u32 i;
    if (kind == HELPER) slot[0xE11] = 4;
    if ((kind != HELPER && !(mutation & 1)) ||
        (kind == HELPER && !(mutation & 2))) return;
    put32(d + GUARD + 0x38, 1 - entry_index);
    put32(d + GUARD + 0x3C, 0x13579);
    put32(d + GUARD + 0x08, 0x2468ACE0);
    d[GUARD + 0x47] ^= 0x69;
    *flags ^= 0xA5235A17;
    *buffer = buffers[1];
    slot[0x23] ^= 0x5A;
    other[0x77] ^= 0xC3;
    if (kind == COPY) {
        u32 block = get32(r + GUARD + 0x108);
        for (i = 0; i < 28; i++) r[GUARD + 0x100 + i] ^= (u8)(0x19 + i);
        /* Both creation and removal of the helper predicate are exercised. */
        put32(r + GUARD + 0x108,
              block ^ 0x20200000);
        put16(slot + 0xDFA, 0xFFFE);
        put16(slot + 0xDFC, 0);
        slot[0xE1D] = !slot[0xE1D];
    }
    if (kind == HELPER) {
        put16(slot + 0xDFA, 0xFFFF);
        put16(slot + 0xDFC, 0xBEEF);
        slot[0xE1D] = !slot[0xE1D];
        slot[0xD08] ^= 0x33;
    }
}

static void expect_event(s32 kind, u32 a, u32 b, u32 c,
                         s32 x, s32 y, s32 w, s32 h)
{
    Event *e;
    if (event_count == 2) fail(1);
    e = &events[event_count++];
    e->kind = kind; e->a = a; e->b = b; e->c = c;
    e->rect[0] = x; e->rect[1] = y; e->rect[2] = w; e->rect[3] = h;
    bytes(e->descriptor, expected_descriptor, DESC);
    bytes(e->slots, expected_slots, SLOTS);
    bytes(e->raw, expected_raw, RAW);
    e->flags = expected_flags; e->buffer = expected_buffer;
}
static void observe(s32 kind, u32 a, u32 b, u32 c, const RECT *rect)
{
    Event *e;
    if (event_cursor >= event_count) fail(2);
    e = &events[event_cursor++];
    if (e->kind != kind || e->a != a || e->b != b || e->c != c) fail(3);
    if (rect && !equal(e->rect, rect, 8)) fail(4);
    if (!equal(e->descriptor, descriptor, DESC)) fail(5);
    if (!equal(e->slots, slots, SLOTS)) fail(6);
    if (!equal(e->raw, raw, RAW)) fail(7);
    if (e->flags != D_8009B0F4_abs || e->buffer != D_8009B118) fail(8);
}
static void actual_effects(s32 kind)
{
    u32 flags = D_8009B0F4_abs;
    effects(kind, descriptor, slots, raw, &flags, &D_8009B118);
    D_8009B0F4_abs = flags;
}
int LoadImage2(RECT *rect, u32 *source)
{
    observe(IMAGE, (u32)source, 0, 0, rect);
    image_calls++;
    actual_effects(IMAGE);
    return 0x76543210;
}
void __wrap_func_8005B620(s32 *destination, const s32 *source, u32 count)
{
    observe(COPY, (u32)destination, (u32)source, count, 0);
    copy_calls++;
    __real_func_8005B620(destination, source, count);
    actual_effects(COPY);
}
void func_80059284(s32 index, s32 value)
{
    observe(HELPER, index, value, 0, 0);
    helper_calls++;
    actual_effects(HELPER);
}
static void expected_effects(s32 kind)
{
    effects(kind, expected_descriptor, expected_slots, expected_raw,
            &expected_flags, &expected_buffer);
}

static void oracle(s32 stage, s32 index, s32 alternate)
{
    static const u32 phases[16] = {
        0x30000, 0x18000, 0x1000, 0x800, 0x8000, 0x800, 0x8000,
        0x5000, 0x5000, 0x5000, 0x5000, 0x1000, 0x1000,
        0x800, 0x19000, 0x800
    };
    u8 *d = expected_descriptor + GUARD;
    u8 *slot = expected_slots + GUARD + index * 0xE20;
    u32 address = 0, flags = 0, i, half;
    s32 skip = 0, image = 0, stream = 0;
    if ((u32)stage > 16) return;
    if (stage == 16) {
        expect_event(COPY, (u32)&D_800F2C40[index] + 0xBF8,
                     (u32)D_801DD000, 64, 0, 0, 0, 0);
        bytes(slot + 0xBF8, expected_raw + GUARD, 256);
        expected_effects(COPY);
        bytes(slot + 0xCF8, expected_raw + GUARD + 256, 28);
        if (get32(slot + 0xD00) & 0x20200000) {
            expect_event(HELPER, index, 4, 0, 0, 0, 0, 0);
            expected_effects(HELPER);
        }
        for (i = 0; i < 2; i++) {
            half = get16(slot + 0xDFA + i * 2);
            if (half != 0xFFFF) put16(slot + 0xCF8 + i * 2, half);
        }
        if (slot[0xE1D]) for (i = 0; i < 3; i++) put32(slot + 0xD08 + i * 4, ~0u);
        slot[0xE14] = 1;
        return;
    }
    if (stage == 0) address = (u32)(index ? D_80010004 : D_80010000);
    if (stage == 1) image = 1;
    if (stage == 2 || stage == 5 || stage == 15) address = (u32)D_801DD000;
    if (stage == 3) {
        expect_event(IMAGE, (u32)D_801DD000, 0, 0, index * 256, 248, 256, 8);
        expected_effects(IMAGE);
        address = (u32)D_801DD000 + 0x1000;
    }
    if (stage == 4 || stage == 6) {
        skip = alternate != (stage == 6);
        if (!skip) {
            expect_event(IMAGE, (u32)(stage == 4 ? D_801DE000 : D_801DD000),
                         0, 0, 512, index + 242, 256, 1);
            expected_effects(IMAGE);
            image = 1;
        }
    }
    if (stage >= 7 && stage <= 10) {
        skip = alternate != (stage >= 9) || index != ((stage - 7) & 1);
        address = (u32)(index ? D_80010018 : D_80010014);
    }
    if (stage == 11 || stage == 12) {
        skip = index != stage - 11;
        address = (u32)(index ? D_80010010 : D_8001000C);
    }
    if (stage == 13 || stage == 14) {
        skip = slot[0xE1D] != 0;
        address = (u32)D_801A8000 + index * 0x800;
        stream = stage == 14 && !skip;
    }
    put32(d + 0x1C, phases[stage]);
    d[0x46] = stream ? 3 : image ? 2 : 1;
    if (skip) flags = (expected_flags & 0xFFFCFFFF) | 0x200000;
    else if (image) flags = (expected_flags & 0xFFDDFFFF) | 0x10000;
    else if (stream) flags = expected_flags;
    else flags = expected_flags & 0xFFDCFFFF;
    expected_flags = flags;
    if (image) {
        put16(d + 0x30, index * 256 + (stage == 1 ? 0 : 192));
        put16(d + 0x32, 256);
        put16(d + 4, 64); put16(d + 6, 16);
    }
    if (stream) put32(d + 0x30, index * 0x19000 + 0xD810);
    if (image || stream) {
        put32(d + 8, (u32)expected_buffer);
        put32(d + 12, (u32)expected_buffer + 0x800);
    } else if (!skip) {
        put32(d + 8, address); put32(d + 12, address);
    }
}

static void run_case(s32 stage, s32 index, s32 alternate, u32 flags,
                     u32 e1d, u32 dfa, u32 dfc, u32 block, s32 mode)
{
    u8 *d = descriptor + GUARD;
    u8 *slot = slots + GUARD + index * 0xE20;
    cases++; current_stage = stage; entry_index = index; mutation = mode;
    fill(descriptor, DESC, cases);
    fill(slots, SLOTS, cases + 0x31);
    fill(raw, RAW, cases + 0x57);
    put32(d + 0x38, index); put32(d + 0x3C, alternate);
    slot[0xE1D] = e1d;
    put16(slot + 0xDFA, dfa); put16(slot + 0xDFC, dfc);
    put32(raw + GUARD + 0x108, block);
    /* A stale destination predicate must not stand in for the copied word. */
    put32(slot + 0xD00, block ^ 0x20200000);
    D_80010000 = destinations[(cases & 1)];
    D_80010004 = destinations[2 + (cases & 1)];
    D_8001000C = destinations[4]; D_80010010 = destinations[5];
    D_80010014 = (s32)destinations[3]; D_80010018 = (s32)destinations[1];
    D_8009B118 = buffers[0]; D_8009B0F4_abs = flags;
    bytes(expected_descriptor, descriptor, DESC);
    bytes(expected_slots, slots, SLOTS);
    bytes(expected_raw, raw, RAW);
    expected_flags = flags; expected_buffer = D_8009B118;
    event_count = event_cursor = 0;
    oracle(stage, index, alternate);
    func_80056D7C((FileTransferDescriptor *)d, stage);
    if (event_cursor != event_count) fail(9);
    if (!equal(expected_descriptor, descriptor, DESC)) fail(10);
    if (!equal(expected_slots, slots, SLOTS)) fail(11);
    if (!equal(expected_raw, raw, RAW)) fail(12);
    if (expected_flags != D_8009B0F4_abs || expected_buffer != D_8009B118) fail(13);
}

int main(void)
{
    static const s32 stages[] = {
        -2147483647 - 1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 17, 2147483647
    };
    static const u32 flags[] = {
        0, ~0u, 0x10000, 0x20000, 0x200000, 0x100000,
        0x40000, 0xA5A55A5A, 0x5A5AA5A5
    };
    static const u32 halves[] = {0, 0xFFFE, 0xFFFF, 0x2345};
    static const u32 blocks[] = {0, 0x200000, 0x20000000, 0x20200000,
                                 0xDFDFFFFF, ~0u};
    static const u32 presence[] = {0, 1, 0x80};
    s32 st, ix, alt, fl, pr, a, b, bl, mode;
    for (st = 0; st < 20; st++)
      for (ix = 0; ix < 2; ix++)
       for (alt = 0; alt < 3; alt++)
        for (fl = 0; fl < 9; fl++)
         for (pr = 0; pr < 3; pr++)
          for (mode = 0; mode < 2; mode++)
            run_case(stages[st], ix, alt, flags[fl], presence[pr],
                     0xFFFF, 0xFFFF, 0, mode);
    for (ix = 0; ix < 2; ix++)
     for (alt = 0; alt < 3; alt++)
      for (fl = 0; fl < 2; fl++)
       for (pr = 0; pr < 3; pr++)
        for (a = 0; a < 4; a++)
         for (b = 0; b < 4; b++)
          for (bl = 0; bl < 6; bl++)
           for (mode = 0; mode < 4; mode++)
            run_case(16, ix, alt, flags[fl], presence[pr],
                     halves[a], halves[b], blocks[bl], mode);
    write_text("cases: ", 7); number(cases);
    write_text("images: ", 8); number(image_calls);
    write_text("copies: ", 8); number(copy_calls);
    write_text("helpers: ", 9); number(helper_calls);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class ModelTextureTransferTests(unittest.TestCase):
    def build_and_run(self, optimization: str, mutated: bool) -> subprocess.CompletedProcess[str]:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        SCRATCH.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="run-", dir=SCRATCH) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutated:
                text = SOURCE.read_text()
                original = "d->phase_size = 0x30000;"
                self.assertEqual(text.count(original), 1)
                text = text.replace(original, "d->phase_size = 0x30001;")
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "' + str(
                        (SOURCE.parent / match.group(1)).resolve()
                    ) + '"',
                    text,
                    flags=re.MULTILINE,
                )
                source = directory / "mutated.c"
                source.write_text(text)
            fixture = directory / "fixture.c"
            fixture.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            flags = [
                "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", optimization, "-I", str(REPOSITORY),
            ]
            objects = []
            for name, path in (("game", source), ("copier", COPIER),
                               ("fixture", fixture), ("start", start)):
                obj = directory / (name + ".o")
                command = ["cc", *flags, "-c", str(path), "-o", str(obj)]
                result = subprocess.run(command, text=True, capture_output=True, timeout=60)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objects.append(str(obj))
            binary = directory / "witness"
            result = subprocess.run(
                ["cc", *flags, *objects, "-Wl,--wrap=func_8005B620",
                 "-o", str(binary)],
                text=True, capture_output=True, timeout=60,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run(
                [str(binary)], text=True, capture_output=True, timeout=120
            )

    def test_retail_stage_contract(self) -> None:
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization, False)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "cases: 20304\nimages: 540\ncopies: 13824\nhelpers: 9216\n",
                )

    def test_actual_source_mutation_is_rejected(self) -> None:
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization, True)
                self.assertEqual(result.returncode, 71, result.stdout + result.stderr)
                self.assertIn("witness mismatch: 10\n", result.stdout)
                self.assertIn("stage: 0\n", result.stdout)


if __name__ == "__main__":
    unittest.main()
