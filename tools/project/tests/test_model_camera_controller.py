"""ILP32 camera behavior witnesses, independent of the executable hash gate."""

from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/func_80051A48.c"
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

WITNESS = r"""
#include "src/types.h"
#include "src/game/model.h"
#include "src/game/camera_view.h"
#include "src/game/model_graphics_state.h"
#include "src/game/model_transfer_state.h"
#include "src/game/model_view_adjustments.h"
#include "src/game/model_effect_requests.h"
#include "src/game/model_update_view_metrics.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "src/game/func_80058434.h"
#include "src/game/func_80058E1C.h"

typedef char ilp32[sizeof(void *) == 4 && sizeof(long) == 4 ? 1 : -1];
typedef char move_size[sizeof(ModelCameraMove) == 44 ? 1 : -1];
typedef char view_size[sizeof(GsRVIEW2) == 32 ? 1 : -1];
typedef char slot_size[sizeof(ModelSlot) == 0xE20 ? 1 : -1];
enum { GUARD = 16, MOVE_SIZE = 44, VIEW_SIZE = 32, SLOT_SIZE = 0xE20,
       GATE = 1, KEY, STEP, ORBIT, METRICS, ADJUST, COMPLETE };
u8 move_storage[GUARD + MOVE_SIZE + GUARD] __attribute__((aligned(16)));
u8 view_storage[GUARD + VIEW_SIZE + GUARD] __attribute__((aligned(16)));
u8 slot_storage[GUARD + 3 * SLOT_SIZE + GUARD] __attribute__((aligned(16)));
__asm__(".globl D_800F2B20\n.set D_800F2B20, move_storage+16\n"
        ".globl D_800F56F0\n.set D_800F56F0, view_storage+16\n"
        ".globl D_800F2C40\n.set D_800F2C40, slot_storage+16\n");
s8 D_8009AF99;
static Key key_record;
static u8 expected_move[sizeof(move_storage)], expected_view[sizeof(view_storage)];
static u8 original_slots[sizeof(slot_storage)];
static s32 gate, have_key, steps[4], step_cursor, expected_step_cursor;
static s32 mutate_kind, mutate_offset, mutate_value;
static u32 event_count, event_cursor, cases, callbacks;
typedef struct {
    s32 kind, args[5];
    u8 move[sizeof(move_storage)], view[sizeof(view_storage)];
} Event;
static Event events[12];

static void copy(u8 *d, const u8 *s, u32 n) { while (n--) *d++ = *s++; }
static s32 same(const u8 *a, const u8 *b, u32 n)
{ while (n--) if (*a++ != *b++) return 0; return 1; }
static void fill(u8 *p, u32 n, u8 v) { while (n--) *p++ = v; }
static u32 u16_at(const u8 *p) { return p[0] | ((u32)p[1] << 8); }
static s32 s16_at(const u8 *p)
{ u32 v = u16_at(p); return v < 32768 ? (s32)v : (s32)v - 65536; }
static s32 s32_at(const u8 *p)
{ return (s32)(u16_at(p) | (u16_at(p + 2) << 16)); }
static void put16(u8 *p, s32 v) { p[0] = v; p[1] = (u32)v >> 8; }
static void put32(u8 *p, s32 v) { put16(p, v); put16(p + 2, (u32)v >> 16); }
static void output(const char *p, u32 n)
{
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(p), "d"(n)
                     : "memory", "cc");
}
static void number(u32 n)
{
    char digits[12], reversed[12];
    u32 i = 0, j = 0;
    do { reversed[i++] = '0' + n % 10; n /= 10; } while (n);
    while (i) digits[j++] = reversed[--i];
    digits[j++] = '\n'; output(digits, j);
}
static void fail(u32 line) __attribute__((noreturn));
static void fail(u32 line)
{
    output("line: ", 6); number(line);
    output("case: ", 6); number(cases);
    output("events: ", 8); number(event_cursor); number(event_count);
    output("steps: ", 7); number(step_cursor); number(expected_step_cursor);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(73) : "memory");
    __builtin_unreachable();
}
#define REQUIRE(x) do { if (!(x)) fail(__LINE__); } while (0)

static void mutation(s32 kind, u8 *move)
{
    if (kind == mutate_kind) put16(move + GUARD + mutate_offset, mutate_value);
}
static s32 result(s32 kind, s32 *cursor)
{
    if (kind == GATE) return gate;
    if (kind == KEY) return have_key ? (s32)&key_record : 0;
    if (kind == STEP) { REQUIRE(*cursor < 4); return steps[(*cursor)++]; }
    return 0;
}
static s32 expect(s32 kind, s32 a, s32 b, s32 c, s32 d, s32 e)
{
    Event *event;
    REQUIRE(event_count < 12);
    event = &events[event_count++];
    event->kind = kind;
    event->args[0] = a; event->args[1] = b; event->args[2] = c;
    event->args[3] = d; event->args[4] = e;
    copy(event->move, expected_move, sizeof(expected_move));
    copy(event->view, expected_view, sizeof(expected_view));
    mutation(kind, expected_move);
    return result(kind, &expected_step_cursor);
}
static s32 observe(s32 kind, s32 a, s32 b, s32 c, s32 d, s32 e)
{
    Event *event;
    if (event_cursor >= event_count) {
        output("unexpected kind: ", 17); number(kind);
        fail(__LINE__);
    }
    event = &events[event_cursor++];
    REQUIRE(event->kind == kind);
    REQUIRE(event->args[0] == a && event->args[1] == b && event->args[2] == c);
    REQUIRE(event->args[3] == d && event->args[4] == e);
    REQUIRE(same(event->move, move_storage, sizeof(move_storage)));
    REQUIRE(same(event->view, view_storage, sizeof(view_storage)));
    REQUIRE(same(original_slots, slot_storage, sizeof(slot_storage)));
    mutation(kind, move_storage);
    callbacks++;
    return result(kind, &step_cursor);
}
s32 func_8005FB08(void) { return observe(GATE, 0, 0, 0, 0, 0); }
s32 func_8005FB14(void) { return observe(KEY, 0, 0, 0, 0, 0); }
s32 Model_GetFrameStep(void) { return observe(STEP, 0, 0, 0, 0, 0); }
void func_80058434(s32 a, s32 b, s32 c, s32 d, s32 e)
{ observe(ORBIT, a, b, c, d, e); }
void Model_UpdateViewMetrics(u8 *view) { observe(METRICS, (s32)view, 0, 0, 0, 0); }
void func_80052694(s32 v) { observe(ADJUST, v, 0, 0, 0, 0); }
void func_80059EBC(s32 v) { observe(COMPLETE, v, 0, 0, 0, 0); }

/* Numeric offsets and widths are the retail contract, not a second use of
 * ModelCameraMove's field names. All tested arithmetic stays in signed range. */
static void track(u8 *out, s32 index, u32 width, u32 components)
{
    u32 axis;
    if (index < 0) return;
    REQUIRE(index < 3);
    for (axis = 0; axis < components; axis++) {
        s32 old = width == 2 ? s16_at(out + axis * width) :
                              s32_at(out + axis * width);
        s32 target = s16_at(original_slots + GUARD + index * SLOT_SIZE +
                           0xDD0 + axis * 2);
        s32 delta = target - old;
        if (delta < -30) delta = -30;
        if (delta > 30) delta = 30;
        if (width == 2) put16(out + axis * width, old + delta);
        else put32(out + axis * width, old + delta);
    }
    if (width == 4 && s32_at(out + 4) > -100) put32(out + 4, -100);
}
static void interpolate(u8 *out, const u8 *leg, u32 elapsed, u32 duration)
{
    u32 axis;
    REQUIRE(duration != 0);
    for (axis = 0; axis < 3; axis++) {
        s32 start = s16_at(leg + axis * 2);
        s32 end = s16_at(leg + 8 + axis * 2);
        put32(out + axis * 4, start + (end - start) * (s32)elapsed / (s32)duration);
    }
    if (s32_at(out + 4) > -100) put32(out + 4, -100);
}
static void orbit(u8 *m)
{
    s32 step = expect(STEP, 0, 0, 0, 0, 0);
    expect(ORBIT, 1, u16_at(m + 4) * step * D_8009AF99, 0, 0, 0);
}
static void simulate(void)
{
    u8 *m = expected_move + GUARD, *v = expected_view + GUARD;
    s32 indices[4], active, key, step, remaining;
    u32 i, duration, elapsed;
    for (i = 0; i < 4; i++) indices[i] = s16_at(m + 18 + i * 8);
    active = expect(GATE, 0, 0, 0, 0, 0);
    if (active && m[1]) {
        duration = u16_at(m + 10);
        if (duration != 65535) {
            step = expect(STEP, 0, 0, 0, 0, 0);
            duration = u16_at(m + 10);
            elapsed = u16_at(m + 8);
            if (elapsed < duration / 5 || elapsed > duration * 4 / 5) {
                step /= 2;
                if (step < 2) step = 2;
            }
            remaining = elapsed + step;
            if (remaining > (s32)duration) remaining = duration;
            put16(m + 8, remaining);
        }
        track(m + 12, indices[0], 2, 2);
        track(m + 20, indices[1], 2, 3);
        track(m + 28, indices[2], 2, 2);
        track(m + 36, indices[3], 2, 3);
        if (m[1] & 1) interpolate(v, m + 12, u16_at(m + 8), u16_at(m + 10));
        else if (m[0]) orbit(m);
        if (m[1] & 2) interpolate(v + 12, m + 28, u16_at(m + 8), u16_at(m + 10));
        expect(METRICS, 0, 0, 0, 0, 0);
        expect(ADJUST, 1, 0, 0, 0, 0);
        if (u16_at(m + 8) >= u16_at(m + 10)) expect(COMPLETE, 1, 0, 0, 0, 0);
        return;
    }
    if (!m[0]) return;
    key = expect(KEY, 0, 0, 0, 0, 0);
    track(v, indices[1], 4, 3);
    track(v + 12, indices[3], 4, 3);
    if (!key || (s16_at((u8 *)&key_record + 6) != 4 &&
                 s16_at((u8 *)&key_record + 14) != 4)) {
        orbit(m);
        expect(ADJUST, 1, 0, 0, 0, 0);
    }
    if (u16_at(m + 2)) {
        step = expect(STEP, 0, 0, 0, 0, 0);
        remaining = (s32)u16_at(m + 2) - step;
        if (remaining > 0) {
            step = expect(STEP, 0, 0, 0, 0, 0);
            remaining = (s32)u16_at(m + 2) - step;
        } else remaining = 0;
        put16(m + 2, remaining);
    }
}
static void reset(void)
{
    u32 i, axis;
    fill(move_storage, sizeof(move_storage), 0xA5);
    fill(view_storage, sizeof(view_storage), 0x5A);
    fill(slot_storage, sizeof(slot_storage), 0xC3);
    fill((u8 *)&D_800F2B20, MOVE_SIZE, 0);
    fill((u8 *)&key_record, sizeof(key_record), 0);
    D_800F2B20.mode = 1; D_800F2B20.flags = 3;
    D_800F2B20.duration = 100; D_800F2B20.elapsed = 40;
    D_800F2B20.field_04 = 8; D_8009AF99 = -1;
    for (i = 0; i < 4; i++) put16(move_storage + GUARD + 18 + i * 8, -1);
    for (i = 0; i < 2; i++)
        for (axis = 0; axis < 3; axis++) {
            put16(move_storage + GUARD + 12 + i * 16 + axis * 2, -210 + axis * 50);
            put16(move_storage + GUARD + 20 + i * 16 + axis * 2, 180 - axis * 170);
            put32(view_storage + GUARD + i * 12 + axis * 4, -1000 + axis * 500);
        }
    for (i = 0; i < 3; i++)
        for (axis = 0; axis < 3; axis++)
            D_800F2C40[i].field_DD0[axis] = (s16)(i * 160 - 200 + axis * 80);
    gate = 1; have_key = 0;
    for (i = 0; i < 4; i++) steps[i] = 6;
    mutate_kind = 0; mutate_offset = 0; mutate_value = 0;
    event_count = event_cursor = 0; step_cursor = expected_step_cursor = 0;
}
static void run(void)
{
    copy(expected_move, move_storage, sizeof(move_storage));
    copy(expected_view, view_storage, sizeof(view_storage));
    copy(original_slots, slot_storage, sizeof(slot_storage));
    simulate();
    func_80051A48();
    REQUIRE(event_cursor == event_count);
    REQUIRE(step_cursor == expected_step_cursor);
    REQUIRE(same(move_storage, expected_move, sizeof(move_storage)));
    REQUIRE(same(view_storage, expected_view, sizeof(view_storage)));
    REQUIRE(same(slot_storage, original_slots, sizeof(slot_storage)));
    cases++;
}
int main(void)
{
    static const s32 boundaries[] = {-31, -30, -29, -1, 0, 1, 29, 30, 31};
    static const u16 times[] = {0, 19, 20, 21, 79, 80, 81, 99, 100};
    static const s32 rates[] = {-3, 0, 1, 3, 6, 25};
    static const s16 selectors[] = {-32768, -1, 0, 1, 2};
    u32 a, b, c, i;
    for (a = 0; a < 2; a++)
        for (b = 0; b < 2; b++)
            for (c = 0; c < 5; c++) {
                reset(); gate = a; D_800F2B20.mode = b; D_800F2B20.flags = c;
                run();
            }
    for (a = 0; a < 9; a++)
        for (b = 0; b < 6; b++) {
            reset(); D_800F2B20.elapsed = times[a]; steps[0] = rates[b]; run();
        }
    for (a = 0; a < 4; a++) {
        reset(); D_800F2B20.duration = 65535;
        D_800F2B20.elapsed = a == 3 ? 65535 : a * 100;
        D_800F2B20.flags = a ? a : 4;
        D_800F2B20.eye.slot = 2; D_800F2B20.target.pair_slot = 1;
        run();
    }
    for (a = 0; a < 5; a++)
        for (b = 0; b < 5; b++)
            for (c = 0; c < 2; c++) {
                reset(); gate = c;
                D_800F2B20.eye.pair_slot = selectors[a];
                D_800F2B20.eye.slot = selectors[b];
                D_800F2B20.target.pair_slot = selectors[b];
                D_800F2B20.target.slot = selectors[a];
                run();
            }
    for (a = 0; a < 9; a++)
        for (b = 0; b < 2; b++) {
            reset(); gate = b;
            for (i = 0; i < 4; i++) put16(move_storage + GUARD + 18 + i * 8, 0);
            for (i = 0; i < 3; i++) {
                D_800F2C40[0].field_DD0[i] = -200 + boundaries[a];
                put16(move_storage + GUARD + 12 + i * 2, -200);
                put16(move_storage + GUARD + 20 + i * 2, -200);
                put16(move_storage + GUARD + 28 + i * 2, -200);
                put16(move_storage + GUARD + 36 + i * 2, -200);
                put32(view_storage + GUARD + i * 4, -200);
                put32(view_storage + GUARD + 12 + i * 4, -200);
            }
            run();
        }
    for (a = 0; a < 5; a++)
        for (b = 0; b < 5; b++)
            for (c = 0; c < 2; c++) {
                reset(); gate = 0; have_key = c;
                key_record.requested[0].kind = a == 3 ? -1 : a;
                key_record.requested[1].kind = b == 3 ? -32768 : b;
                D_800F2B20.field_02 = 10; steps[1] = 3; steps[2] = 7;
                D_8009AF99 = a == 0 ? -128 : a == 1 ? 127 : -1;
                run();
            }
    for (a = 0; a < 5; a++)
        for (b = 0; b < 2; b++) {
            reset(); gate = 0; have_key = 1; key_record.requested[0].kind = 4;
            D_800F2B20.field_02 = a == 4 ? 65535 : a * 3;
            steps[0] = 6; steps[1] = b ? 11 : 2;
            run();
        }
    /* The four selectors are captured even if a helper changes their backing. */
    for (a = 0; a < 4; a++)
        for (b = 0; b < 2; b++) {
            reset(); gate = b;
            put16(move_storage + GUARD + 18 + a * 8, 0);
            mutate_kind = b ? GATE : KEY;
            mutate_offset = 18 + a * 8; mutate_value = 2;
            run();
        }
    for (a = 0; a < 3; a++) {
        reset(); gate = 0; have_key = 1; key_record.requested[1].kind = 4;
        D_800F2B20.field_02 = 10;
        mutate_kind = STEP; mutate_offset = 2; mutate_value = a * 6;
        steps[0] = 6; steps[1] = 3;
        run();
    }
    reset(); mutate_kind = ADJUST; mutate_offset = 8; mutate_value = 100; run();
    reset(); mutate_kind = STEP; mutate_offset = 10; mutate_value = 200; run();
    reset(); gate = 0; D_800F2B20.eye.slot = 0; D_800F2B20.target.slot = 1;
    D_800F56F0.vpx = 70000; D_800F56F0.vrx = -70000; run();
    output("camera cases: ", 14); number(cases);
    output("camera callbacks: ", 18); number(callbacks);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86, a host compiler, and ILP32 execution",
)
class ModelCameraControllerTests(unittest.TestCase):
    def build_and_run(self, optimization: str, mutation: str = ""):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        scratch = ROOT / "tmp/test-model-camera"
        scratch.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=scratch) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation:
                text = SOURCE.read_text()
                edits = {
                    "clamp": ("dx < 31 ? dx : 30", "dx < 31 ? dx : 29"),
                    "start-z": (
                        "dy = (s16)slot->field_DD0[1] - D_800F2B20.eye.start_y;",
                        "dy = (s16)slot->field_DD0[1] - D_800F2B20.eye.start_y;\n"
                        "            D_800F2B20.eye.start_z = slot->field_DD0[2];",
                    ),
                    "key-kind": ("key->requested[1].kind != 4", "key->requested[1].kind != 3"),
                    "timer-step": (
                        "step = Model_GetFrameStep();\n            progress = move->field_02 - step;",
                        "step = 6;\n            progress = move->field_02 - step;",
                    ),
                    "late-index": (
                        "slot = base + eye_slots[1];",
                        "slot = base + D_800F2B20.eye.slot;",
                    ),
                    "completion": (
                        "D_800F2B20.elapsed >= D_800F2B20.duration",
                        "D_800F2B20.elapsed > D_800F2B20.duration",
                    ),
                }
                old, new = edits[mutation]
                self.assertIn(old, text)
                text = text.replace(old, new)
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "' + str(
                        (SOURCE.parent / match[1]).resolve()
                    ) + '"', text, flags=re.MULTILINE,
                )
                source = directory / "mutated.c"
                source.write_text(text)
            fixture = directory / "fixture.c"
            fixture.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            flags = [
                "-std=gnu89", "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", "-D_LANGUAGE_C",
                "-Werror=implicit-function-declaration", optimization, "-I", str(ROOT),
            ]
            objects = []
            for name, path in (("game", source), ("fixture", fixture), ("start", start)):
                obj = directory / f"{name}.o"
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

    def test_retail_camera_contract(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertRegex(result.stdout, r"^camera cases: 220\ncamera callbacks: \d+\n$")

    def test_behavioral_mutations_are_rejected(self):
        for mutation in ("clamp", "start-z", "key-kind", "timer-step", "late-index", "completion"):
            with self.subTest(mutation=mutation):
                result = self.build_and_run("-O2", mutation)
                self.assertEqual(result.returncode, 73, result.stdout + result.stderr)
                self.assertIn("case: ", result.stdout)


if __name__ == "__main__":
    unittest.main()
