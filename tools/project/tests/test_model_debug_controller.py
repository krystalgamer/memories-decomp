"""Bounded ILP32 behavior witnesses for the model-debug controller."""

from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/func_800534B8.c"
START = """
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
PRINT_STUB = r"""
#include "src/types.h"
extern s32 print_arity(char *);
extern s32 print_has_string(char *);
extern void observe_print(char *, s32, s32);
int FntPrint(char *format, ...)
{
    __builtin_va_list ap;
    s32 a = 0, b = 0, count = print_arity(format);
    __builtin_va_start(ap, format);
    if (count) {
        if (print_has_string(format)) a = (s32)__builtin_va_arg(ap, char *);
        else a = __builtin_va_arg(ap, s32);
    }
    if (count > 1) b = __builtin_va_arg(ap, s32);
    __builtin_va_end(ap);
    observe_print(format, a, b);
    return 0;
}
"""
WITNESS = r"""
#include "src/types.h"
#include "src/game/model.h"
#include "src/game/camera_view.h"
#include "src/game/model_graphics_state.h"
#include "src/game/model_handler_state.h"
#include "src/game/model_scene_setup.h"
#include "src/game/model_debug_controller.h"
#include "src/game/model_state_setters.h"
#include "src/game/func_80059AA8.h"
#include "src/game/func_8005922C.h"
#include "src/game/file_transfer.h"
#include "src/game/input.h"
#include "src/psyq/rand.h"

typedef char ilp32[sizeof(void *) == 4 && sizeof(long) == 4 ? 1 : -1];
typedef char slot_size[sizeof(ModelSlot) == 0xE20 ? 1 : -1];
typedef char unit_size[sizeof(GsCOORDUNIT) == 0x50 ? 1 : -1];
enum { GUARD = 16, SLOTS = 3 * 0xE20, FRAME = 1, POLL, LOAD, PLACE,
       ORIENT, WAIT, RESET, PROPERTIES };
u8 slot_storage[GUARD + SLOTS + GUARD] __attribute__((aligned(16)));
__asm__(".globl D_800F2C40\n.set D_800F2C40, slot_storage+16\n");
static u8 expected_slots[sizeof(slot_storage)];
static GsCOORDUNIT units[3], original_units[3];
static u8 record[0xB2];
u8 *D_8009AF88 = record;
s8 D_8009AF9A;
u8 D_8009AFA1;
s16 D_8009B488[3];
u8 D_8009B48E[2], D_8009B490[2];
u16 gInput_wPad1Pressed, gInput_wPad2Pressed, gInput_wPad1Repeat, gInput_wPad1Held;
volatile u32 D_8009B0F4_abs;
u32 D_8009B134_abs;
char D_80011518[] = "LOAD STAGE(%d,%d)\n";
char D_8001152C[] = "LOAD MODEL0(%d,%d)\n";
char D_80011540[] = "LOAD MODEL1(%d,%d)\n";
char D_80011554[] = "(AF=%d,GF=%d)";
char D_80011564[] = "STAGE=%d,Y=%d\n";
static s32 events[20][7], event_count, print_count, display_cursor;
static s32 complete_slot, complete_phase, samples[16], sample_count, sample_cursor;
static s32 mutate_print;
static u32 cases, print_total, random_total;

static void bytes(void *out, const void *in, u32 n)
{ u8 *d = out; const u8 *s = in; while (n--) *d++ = *s++; }
static void fill(void *out, u32 n, u8 v) { u8 *p = out; while (n--) *p++ = v; }
static s32 equal(const void *a, const void *b, u32 n)
{ const u8 *x = a, *y = b; while (n--) if (*x++ != *y++) return 0; return 1; }
static void output(const char *p, u32 n)
{
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(p), "d"(n)
                     : "memory", "cc");
}
static void number(u32 n)
{
    char out[12], reverse[12]; u32 i = 0, j = 0;
    do { reverse[i++] = '0' + n % 10; n /= 10; } while (n);
    while (i) out[j++] = reverse[--i];
    out[j++] = '\n'; output(out, j);
}
static void fail(u32 line) __attribute__((noreturn));
static void fail(u32 line)
{
    output("line: ", 6); number(line);
    output("case: ", 6); number(cases);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(73) : "memory");
    __builtin_unreachable();
}
#define CHECK(x) do { if (!(x)) fail(__LINE__); } while (0)

static void event(s32 kind, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    s32 *p;
    CHECK(event_count < 20);
    p = events[event_count++];
    p[0] = kind; p[1] = a; p[2] = b; p[3] = c;
    p[4] = d; p[5] = e; p[6] = f;
    CHECK(equal(slot_storage, expected_slots, sizeof(slot_storage)));
    if (kind == WAIT) CHECK(display_cursor == 13);
    if (kind == FRAME) CHECK(display_cursor == 0 && print_count == 0);
    if (kind == POLL || kind == LOAD) CHECK(display_cursor == 0 && print_count == 1);
}
static void completed(s32 slot)
{
    if (complete_slot == slot) {
        D_800F2C40[slot].field_E1F = 1;
        expected_slots[GUARD + slot * 0xE20 + 0xE1F] = 1;
        D_8009AF9A = complete_phase;
    }
}
void func_80059AEC(s32 v) { event(FRAME, v, 0, 0, 0, 0, 0); CHECK(v == 1); }
void func_80056828(s32 slot) { event(POLL, slot, 0, 0, 0, 0, 0); completed(slot); }
s32 Model_LoadMonsterMerge(s32 slot, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    event(LOAD, slot, a, b, c, d, e);
    CHECK(a == -1 && b == -1 && c == -1 && d == -1 && e == -1 && f == 0);
    completed(slot);
    return 0;
}
s32 func_80059AA8(s32 slot, s32 flag)
{
    event(ORIENT, slot, flag, 0, 0, 0, 0);
    CHECK(flag == (((record[0xA1] >> 1) ^ 1) & 1));
    return flag;
}
void func_8005922C(GsCOORDUNIT *unit, void *scale)
{
    GsCOORDUNIT expected;
    s32 i;
    CHECK(scale == 0 && unit == D_800F2C40[2].field_D18);
    event(PLACE, (s32)unit, 0, 0, 0, 0, 0);
    if (unit) {
        bytes(&expected, &original_units[2], sizeof(expected));
        expected.rot.vx = expected.rot.vy = expected.rot.vz = 0;
        expected.matrix.t[0] = expected.matrix.t[2] = 0;
        expected.matrix.t[1] = (s16)(D_8009B004 >> 16);
        CHECK(equal(unit, &expected, sizeof(expected)));
        bytes(&original_units[2], &expected, sizeof(expected));
    }
    for (i = 0; i < 3; i++) CHECK(equal(&units[i], &original_units[i], sizeof(units[i])));
}
void File_WaitForTransfers(void) { event(WAIT, 0, 0, 0, 0, 0, 0); }
void func_800533D8(void) { event(RESET, 0, 0, 0, 0, 0, 0); }
void Model_SetSlotProperties(s32 slot, ...)
{
    __builtin_va_list ap;
    s32 id, a = 0, b = 0, c = 0, d = 0;
    CHECK(slot >= 0 && slot < 3);
    __builtin_va_start(ap, slot);
    id = __builtin_va_arg(ap, s32);
    if (slot < 2) {
        a = __builtin_va_arg(ap, s32); b = __builtin_va_arg(ap, s32);
        c = __builtin_va_arg(ap, s32); d = __builtin_va_arg(ap, s32);
        CHECK(a == -1 && b == -1);
        CHECK(c == D_8009B48E[slot] && d == D_8009B490[slot]);
    }
    __builtin_va_end(ap);
    CHECK(id == (u16)D_8009B488[slot]);
    event(PROPERTIES, slot, id, a, b, c, d);
}
int rand(void)
{
    CHECK(sample_cursor < sample_count);
    if (sample_cursor == 5) CHECK(D_8009B488[0] == 5);
    if (sample_cursor == 7) CHECK(D_8009B488[1] == 7);
    random_total++;
    return samples[sample_cursor++];
}
s32 print_arity(char *format)
{
    if (format == D_8009B00C || format == D_8009B024 || format == D_8009B02C) return 0;
    CHECK(format == D_80011518 || format == D_8001152C || format == D_80011540 ||
          format == D_8009B01C || format == D_80011554 || format == D_80011564);
    return 2;
}
s32 print_has_string(char *format) { return format == D_8009B01C; }
void observe_print(char *format, s32 a, s32 b)
{
    s32 side, digit, expected, slot;
    static const s32 divisors[3] = {100, 10, 1};
    print_count++; print_total++;
    if (format == D_80011518 || format == D_8001152C || format == D_80011540) {
        slot = format == D_80011518 ? 2 : format == D_8001152C ? 0 : 1;
        CHECK(a == D_800F2C40[slot].field_DF8 && b == D_800F2C40[slot].field_E14);
        return;
    }
    CHECK(display_cursor < 13);
    if (display_cursor < 3 || (display_cursor >= 6 && display_cursor < 9)) {
        side = display_cursor >= 6;
        digit = side ? display_cursor - 6 : display_cursor;
        expected = (u16)D_8009B488[side] / divisors[digit] % 10;
        CHECK(format == D_8009B01C && b == expected);
        CHECK(a == (s32)(D_8009AFA1 == side * 3 + digit ? D_8009B014 : D_8009B00C));
    } else if (display_cursor == 3 || display_cursor == 9) {
        CHECK(format == D_8009B00C);
    } else if (display_cursor == 4 || display_cursor == 10) {
        side = display_cursor == 10;
        CHECK(format == D_80011554 && a == D_8009B48E[side] && b == D_8009B490[side]);
    } else if (display_cursor == 5) CHECK(format == D_8009B024);
    else if (display_cursor == 11) CHECK(format == D_8009B02C);
    else CHECK(format == D_80011564 && a == (u16)D_8009B488[2] &&
               b == (s16)(D_8009B004 >> 16));
    if (mutate_print && display_cursor == 0) {
        D_8009B488[0] = 789; D_8009AFA1 = 2;
    }
    display_cursor++;
}
static void reset(void)
{
    s32 i;
    fill(slot_storage, sizeof(slot_storage), 0xA7);
    fill(record, sizeof(record), 0);
    fill(units, sizeof(units), 0x5B);
    for (i = 0; i < 3; i++) {
        D_800F2C40[i].field_E1F = 1;
        D_800F2C40[i].field_E14 = 0;
        D_800F2C40[i].field_DF8 = 5 + i * 2;
        D_800F2C40[i].field_D18 = &units[i];
    }
    D_8009AF9A = 0; D_8009AFA1 = 0;
    D_8009B004 = 0x0123BEEF; D_8009B008 = 0xABCDEF01;
    D_8009B488[0] = 123; D_8009B488[1] = 456; D_8009B488[2] = 0;
    D_8009B48E[0] = D_8009B48E[1] = 0;
    D_8009B490[0] = D_8009B490[1] = 0;
    gInput_wPad1Pressed = gInput_wPad2Pressed = gInput_wPad1Repeat = gInput_wPad1Held = 0;
    D_8009B0F4_abs = D_8009B134_abs = 0;
    event_count = print_count = display_cursor = sample_cursor = sample_count = 0;
    complete_slot = -1; complete_phase = 0; mutate_print = 0;
}
static s32 run(void)
{
    s32 result, i;
    bytes(expected_slots, slot_storage, sizeof(slot_storage));
    bytes(original_units, units, sizeof(units));
    result = func_800534B8();
    CHECK(equal(slot_storage, expected_slots, sizeof(slot_storage)));
    for (i = 0; i < 3; i++) CHECK(equal(&units[i], &original_units[i], sizeof(units[i])));
    CHECK((D_8009B004 & 0xFFFF) == 0xBEEF);
    CHECK((D_8009B008 & 0xFFFFFF00) == 0xABCDEF00);
    CHECK(sample_cursor == sample_count);
    CHECK(display_cursor == 0 || display_cursor == 13);
    cases++;
    return result;
}
int main(void)
{
    static const s8 phases[] = {-128, -2, -1, 0, 39, 40, 127};
    static const u16 ids[] = {0, 9, 99, 999, 65535};
    static const s32 divs[] = {100, 10, 1};
    s32 i, j, k, slot, direction, before, expected, digit, count;
    for (i = 0; i < 7; i++) {
        reset(); D_8009AF9A = phases[i];
        CHECK(run() == (phases[i] == -2));
        CHECK(event_count == 0);
        CHECK(display_cursor == ((phases[i] < 40 && phases[i] != -2) ? 13 : 0));
    }
    for (i = 0; i < 2; i++) {
        reset(); if (i) gInput_wPad2Pressed = 0x20; else gInput_wPad1Pressed = 0x20;
        CHECK(run() == 0 && D_8009AF9A == 40 && print_count == 0 && event_count == 0);
    }
    for (i = 0; i < 8; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++) {
                reset();
                for (slot = 0; slot < 3; slot++) {
                    D_800F2C40[slot].field_E1F = (i >> slot) & 1;
                    D_800F2C40[slot].field_E14 = j ? 255 : 0;
                }
                slot = !(i & 4) ? 2 : !(i & 1) ? 0 : !(i & 2) ? 1 : -1;
                complete_slot = k ? slot : -1;
                record[0xA1] = k * 2; record[0xA4] = 0x80; record[0xA5] = 0xFF;
                CHECK(run() == 0);
                if (slot < 0) CHECK(event_count == 0 && print_count == 13);
                else {
                    CHECK(events[0][0] == FRAME && events[1][0] == (j ? LOAD : POLL));
                    CHECK(events[1][1] == slot && print_count == 14);
                    CHECK(event_count == (k ? 3 : 2));
                    if (k) CHECK(events[2][0] == (slot == 2 ? PLACE : ORIENT));
                    if (k && slot == 2) CHECK((s16)(D_8009B004 >> 16) == -128);
                }
            }
    for (i = 0; i < 4; i++) {
        reset(); D_800F2C40[2].field_E1F = 0; D_800F2C40[2].field_E14 = 255;
        D_8009B0F4_abs = i < 3 ? (i == 0 ? 0x10 : i == 1 ? 0x20 : 0x2000000) : 0;
        D_8009B134_abs = i == 3;
        CHECK(run() == 0 && event_count == 1 && events[0][0] == FRAME);
    }
    reset(); D_800F2C40[2].field_E1F = 0; D_800F2C40[2].field_D18 = 0; complete_slot = 2;
    CHECK(run() == 0 && events[2][0] == PLACE && events[2][1] == 0);
    reset(); D_8009B008 &= ~255u;
    CHECK(run() == 0 && print_count == 0);
    reset(); gInput_wPad1Held = 0x100; gInput_wPad2Pressed = 0x100;
    CHECK(run() == 0 && (u8)D_8009B008 == 0 && print_count == 0);
    reset(); D_8009B008 &= ~255u; gInput_wPad1Held = 0x100; gInput_wPad2Pressed = 0x100;
    CHECK(run() == 0 && (u8)D_8009B008 == 1 && print_count == 13);
    for (i = 0; i < 7; i++)
        for (j = 0; j < 2; j++) {
            reset(); D_8009B488[2] = i; gInput_wPad1Repeat = j ? 0x100 : 0x800;
            CHECK(run() == 0 && D_8009B488[2] == (i + (j ? 6 : 8)) % 7);
        }
    for (i = 0; i < 6; i++)
        for (j = 0; j < 2; j++) {
            reset(); D_8009AFA1 = i; gInput_wPad1Repeat = j ? 0x8000 : 0x2000;
            CHECK(run() == 0 && D_8009AFA1 == (i + (j ? 5 : 7)) % 6);
        }
    for (i = 0; i < 6; i++)
        for (j = 0; j < 5; j++)
            for (k = 0; k < 2; k++) {
                reset(); slot = i / 3; D_8009AFA1 = i;
                D_8009B488[slot] = ids[j]; before = ids[j] % 1000;
                digit = before / divs[i % 3] % 10; direction = k ? 1 : -1;
                expected = before + (((digit + direction + 10) % 10) - digit) * divs[i % 3];
                gInput_wPad1Repeat = k ? 0x1000 : 0x4000;
                CHECK(run() == 0 && D_8009B488[slot] == expected);
            }
    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++) {
            reset(); D_8009B004 = (i ? 0x7FFEu : 0x8002u) << 16 | 0xBEEF;
            before = D_8009B004 >> 16; gInput_wPad1Held = 0x80;
            gInput_wPad1Repeat = j ? 0x1000 : 0x4000;
            CHECK(run() == 0 && (u16)(D_8009B004 >> 16) == (u16)(before + (j ? -10 : 10)));
            CHECK(event_count == 1 && events[0][0] == PLACE);
        }
    for (i = 0; i < 16; i++) {
        reset(); gInput_wPad1Pressed = i;
        D_8009B48E[0] = D_8009B48E[1] = 254;
        D_8009B490[0] = D_8009B490[1] = 255;
        CHECK(run() == 0);
        CHECK(D_8009B48E[0] == (254 ^ !!(i & 4)) && D_8009B48E[1] == (254 ^ !!(i & 8)));
        CHECK(D_8009B490[0] == (255 ^ !!(i & 1)) && D_8009B490[1] == (255 ^ !!(i & 2)));
    }
    reset(); gInput_wPad1Pressed = 0x10;
    CHECK(run() == 0 && event_count == 5);
    CHECK(events[0][0] == WAIT && events[1][0] == RESET);
    for (i = 0; i < 3; i++) CHECK(events[i + 2][0] == PROPERTIES && events[i + 2][1] == i);
    reset(); D_800F2C40[0].field_E1F = 0; complete_slot = 0; complete_phase = -2;
    samples[0] = -256; samples[1] = 300 << 8; samples[2] = 650 << 8; samples[3] = 720 << 8;
    for (i = 4; i < 9; i++) samples[i] = (i + 1) << 8;
    sample_count = 9;
    CHECK(run() == 0 && D_8009B488[0] == 6 && D_8009B488[1] == 8 && D_8009B488[2] == 2);
    CHECK(event_count == 8 && events[3][0] == WAIT && events[4][0] == RESET);
    reset(); mutate_print = 1;
    CHECK(run() == 0 && display_cursor == 13);
    count = cases;
    output("model-debug cases: ", 19); number(count);
    output("print observations: ", 20); number(print_total);
    output("random samples: ", 16); number(random_total);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86 host compilation and ILP32 execution",
)
class ModelDebugControllerTests(unittest.TestCase):
    def build_and_run(self, optimization, mutation=""):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        scratch = ROOT / "tmp/test-model-debug"
        scratch.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=scratch) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation:
                edits = {
                    "exit": ("D_8009AF9A == -2", "D_8009AF9A == -3"),
                    "digit": ("+ 11) % 10", "+ 12) % 10"),
                    "toggle": ("D_8009B48E[0] ^= 1", "D_8009B48E[0] ^= 2"),
                }
                if mutation == "stage":
                    old, new = ("Model_SetSlotProperties(2, (u16)D_8009B488[2])",
                                "Model_SetSlotProperties(2, (u16)D_8009B488[0])")
                else:
                    old, new = edits[mutation]
                text = source.read_text()
                self.assertIn(old, text)
                text = text.replace(old, new)
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "' + str((SOURCE.parent / match[1]).resolve()) + '"',
                    text, flags=re.MULTILINE,
                )
                source = directory / "mutated.c"
                source.write_text(text)
            paths = [("game", source), ("diagnostics", ROOT / "src/game/model_handler_diagnostics.c")]
            for name, text, suffix in (
                ("fixture", WITNESS, ".c"), ("print", PRINT_STUB, ".c"), ("start", START, ".S"),
            ):
                path = directory / (name + suffix)
                path.write_text(text)
                paths.append((name, path))
            flags = [
                "-std=gnu89", "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", "-D_LANGUAGE_C", optimization,
                "-Werror=implicit-function-declaration", "-I", str(ROOT),
            ]
            objects = []
            for name, path in paths:
                obj = directory / (name + ".o")
                compiled = subprocess.run(
                    ["cc", *flags, "-c", str(path), "-o", str(obj)],
                    capture_output=True, text=True, timeout=60,
                )
                self.assertEqual(compiled.returncode, 0, compiled.stdout + compiled.stderr)
                objects.append(str(obj))
            binary = directory / "witness"
            linked = subprocess.run(
                ["cc", *flags, *objects, "-o", str(binary)],
                capture_output=True, text=True, timeout=60,
            )
            self.assertEqual(linked.returncode, 0, linked.stdout + linked.stderr)
            return subprocess.run([str(binary)], capture_output=True, text=True, timeout=30)

    def test_model_debug_behavior(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "model-debug cases: 158\nprint observations: 1997\nrandom samples: 9\n",
                )

    def test_source_mutations_are_rejected(self):
        for mutation in ("exit", "digit", "toggle", "stage"):
            with self.subTest(mutation=mutation):
                result = self.build_and_run("-O2", mutation)
                self.assertEqual(result.returncode, 73, result.stdout + result.stderr)
                self.assertIn("case: ", result.stdout)


if __name__ == "__main__":
    unittest.main()
