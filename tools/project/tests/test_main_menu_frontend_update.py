"""Source-only ILP32 oracle for frontend entry motion and prompt boundaries."""

from pathlib import Path
import os
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/overlays/main_menu/frontend_update.c"
SCRATCH = ROOT / "tmp/main-menu-frontend-witness"
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

# Reuse the production includes to type-check every stub/global against its
# real headers (including the SDK's int rsin(int)), in a separate translation unit.
WITNESS = r"""
#include "src/types.h"
#include "declarations.h"
typedef char ilp32[sizeof(void *) == 4 && sizeof(int) == 4 ? 1 : -1];
u8 state[10];
__asm__(".globl gMain_bMenuID\n.set gMain_bMenuID, state\n"
        ".globl D_80184595\n.set D_80184595, state+1\n"
        ".globl D_80184596\n.set D_80184596, state+2\n"
        ".globl D_80184597\n.set D_80184597, state+3\n"
        ".globl D_80184598\n.set D_80184598, state+4\n"
        ".globl D_80184599\n.set D_80184599, state+5\n"
        ".globl D_8018459A\n.set D_8018459A, state+6\n"
        ".globl D_8018459B\n.set D_8018459B, state+7\n"
        ".globl D_8018459C\n.set D_8018459C, state+8\n"
        ".globl D_8018459D\n.set D_8018459D, state+9\n");
DisplayObject *D_80184560;
u8 *gMain_apMenuEntries[11];
volatile u16 gInput_wPad1Pressed;
u16 gInput_wPad1Repeat;
s32 D_8009B0D8;
u8 D_8009B3ED, D_8009B3EA;

enum { SIZE = 0x90, GUARD = 16, SIN = 1, AFTERIMAGE, CONFIGURE };
static u8 memory[SIZE] __attribute__((aligned(16)));
static u8 expected[SIZE], expected_state[10];
static u8 *object = memory + GUARD;
typedef struct { s32 kind, argument; u8 snapshot[SIZE]; } Event;
static Event events[3];
static s32 event_count, cursor, sine_value, case_number;
static void copy(u8 *dst, const u8 *src, s32 n)
{ while (n--) *dst++ = *src++; }
static s32 equal(const u8 *a, const u8 *b, s32 n)
{ while (n--) if (*a++ != *b++) return 0; return 1; }
static void put16(u8 *p, s32 value)
{ p[0] = value; p[1] = (u32)value >> 8; }
static s32 narrow(s32 value)
{ s32 bits = (u32)value & 65535; return bits < 32768 ? bits : bits - 65536; }
static void text(const char *p, u32 n)
{
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(p), "d"(n)
                     : "memory", "cc");
}
static void number(u32 n)
{
    char out[12]; s32 i = 11;
    out[i] = '\n';
    do { out[--i] = '0' + n % 10; n /= 10; } while (n);
    text(out+i, 12-i);
}
static void fail(s32 reason) __attribute__((noreturn));
static void fail(s32 reason)
{
    text("mismatch reason/case: ", sizeof("mismatch reason/case: ")-1);
    number(reason); number(case_number);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(75) : "memory");
    __builtin_unreachable();
}
static void record(s32 kind, s32 argument)
{
    Event *e = &events[event_count++];
    e->kind = kind; e->argument = argument;
    copy(e->snapshot, expected, SIZE);
}
static void consume(s32 kind, s32 argument)
{
    Event *e;
    if (cursor >= event_count) fail(1);
    e = &events[cursor++];
    if (e->kind != kind || e->argument != argument) fail(2);
    if (!equal(memory, e->snapshot, SIZE)) fail(3);
}
int rsin(int angle) { consume(SIN, angle); return sine_value; }
void MainMenu_SpawnFrontendEntryAfterimage(u8 *source)
{
    if (source != object) fail(4);
    /* Snapshot includes the newly interpolated coordinate, but old flags. */
    consume(AFTERIMAGE, 0);
}
void func_80040410(DisplayObjectConfig *entry, s32 selector)
{
    if ((u8 *)entry != object) fail(5);
    consume(CONFIGURE, selector);
    /* Owned contract: src/game/display_object_core.c, func_80040410. */
    entry->field_69 = selector;
    entry->flags &= 0xFFEF;
}
s32 SaveData_PollLoad(void) { fail(10); }
s32 SaveData_UpdateTradeLoad(void) { fail(11); }
s32 SaveData_UpdateDuelLoad(void) { fail(12); }
s32 MemCardDialog_Poll(void) { fail(13); }
void Input_ResetPads(void) { fail(14); }
void MainMenu_StartFrontendEntryTransition(s32 mode) { fail(15); }
void SaveData_RequestLoad(void) { fail(16); }
void SaveData_RequestWrite(void) { fail(17); }
void SD_SEPlay(s32 id, s32 volume, s32 pan) { fail(18); }

static void reset(void)
{
    s32 i;
    case_number++;
    for (i=0; i<SIZE; i++) memory[i] = i * 13 + 17;
    for (i=0; i<10; i++) state[i] = 0;
    for (i=0; i<11; i++) gMain_apMenuEntries[i] = 0;
    D_80184560 = 0;
    gInput_wPad1Pressed = gInput_wPad1Repeat = 0;
    D_8009B0D8 = 0;
    D_8009B3ED = 0xA5; D_8009B3EA = 0x5A;
    event_count = cursor = 0;
}
static void snapshot(void)
{ copy(expected, memory, SIZE); copy(expected_state, state, 10); }
static void check(s32 result, s32 slot, s32 prompt)
{
    s32 i;
    if (MainMenu_UpdateFrontendMenu() != result) fail(20);
    if (cursor != event_count) fail(21);
    if (!equal(memory, expected, SIZE)) fail(22);
    if (!equal(state, expected_state, 10)) fail(23);
    for (i=0; i<11; i++)
        if (gMain_apMenuEntries[i] != (i == slot ? object : 0)) fail(24);
    if (D_80184560 != (prompt ? (DisplayObject *)object : 0)) fail(25);
    if (gInput_wPad1Pressed || gInput_wPad1Repeat ||
        D_8009B3ED != 0xA5 || D_8009B3EA != 0x5A) fail(26);
}
static void entries(void)
{
    static const s32 timers[] = {-32768,-1,0,1,2,8,15,16,17};
    static const s32 endpoints[][2] = {
        {-32768,32767}, {32767,-32768}, {0,1000}, {100,-100}
    };
    static const s32 sines[] = {0,4096,-4096,1365,-1365};
    s32 group,index,t,e,s,selected,timer,start,end,phase,product,x,flags;
    u8 *out = expected + GUARD;
    for (group=0; group<2; group++)
     for (index=0; index<=10; index+=5)
      for (t=0; t<9; t++)
       for (e=0; e<4; e++)
        for (s=0; s<5; s++) {
            reset();
            selected = group * 5; timer = timers[t];
            start = endpoints[e][0]; end = endpoints[e][1];
            state[0] = selected; state[5] = 1;
            gMain_apMenuEntries[index] = object;
            /* Exercise both pre-existing visibility states and other bits. */
            flags = 0x115 | ((e & 1) ? 0x40 : 0);
            put16(object+8, flags); put16(object+0x30, 1234);
            put16(object+0x36, start); put16(object+0x38, end);
            put16(object+0x60, timer);
            sine_value = sines[s]; snapshot();
            if (timer <= 0) {
                expected_state[5] = 0;
            } else {
                put16(out+0x60, timer-1);
                if ((selected < 5) == (index < 5)) {
                    phase = 17 - timer;
                    x = end;
                    if (timer != 1) {
                        record(SIN, phase * 64);
                        product = sine_value * (end-start);
                        /* Division, not a signed right shift: truncate to zero. */
                        x = start + product / 4096;
                    }
                    put16(out+0x30, x);
                    if (phase % 2 != 0) record(AFTERIMAGE, 0);
                    flags |= 0x40;
                } else {
                    flags &= ~0x40;
                }
                put16(out+8, flags);
                record(CONFIGURE, 2*index + (selected != index));
                out[0x69] = 2*index + (selected != index);
                put16(out+8, flags & ~0x10);
            }
            check(-1, index, 0);
        }
}
static void completions(void)
{
    static const s32 selections[] = {0,2,3,4,5,6,10,255};
    s32 i;
    for (i=0; i<8; i++) {
        reset(); state[0] = selections[i]; state[2] = state[5] = 1;
        snapshot(); expected_state[5] = 0;
        check(selections[i], -1, 0);
    }
}
static void prompts(void)
{
    static const s32 counters[] = {2998,2999,3000,32767,65535};
    s32 c,delta,accumulated;
    for (c=0; c<5; c++) for (delta=0; delta<3; delta++) {
        reset(); D_80184560 = (DisplayObject *)object;
        put16(object+8, 0x155); object[0x6C] = 1;
        put16(object+0x36, counters[c]); D_8009B0D8 = delta;
        snapshot();
        accumulated = narrow(counters[c] + delta);
        put16(expected+GUARD+0x36, accumulated);
        expected[GUARD+0x6C] = 0;
        check(accumulated >= 3000 ? -2 : -1, -1, 1);
        if (D_8009B0D8 != delta) fail(27);
    }
}
int main(void)
{
    entries(); completions(); prompts();
    if (case_number != 1103) fail(28);
    text("entry=1080 completion=8 prompt=15\n",
         sizeof("entry=1080 completion=8 prompt=15\n")-1);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host compiler and native ILP32 execution",
)
class MainMenuFrontendUpdateTests(unittest.TestCase):
    def build_and_run(self, optimization, mutation=None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run this test from the repository root")
        SCRATCH.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="run-", dir=SCRATCH) as temporary:
            directory = Path(temporary)
            source = SOURCE
            def normalized(code):
                return re.sub(
                    r'^#include "([^"]+)"',
                    lambda m: '#include "' + str(
                        (SOURCE.parent / m.group(1)).resolve()
                    ) + '"',
                    code, flags=re.MULTILINE,
                )

            prefix = SOURCE.read_text().split(
                "s32 MainMenu_UpdateFrontendMenu(void)", 1
            )[0]
            (directory / "declarations.h").write_text(normalized(prefix))
            # Modern GCC rejects graphics_frame.h's incomplete extern array.
            # Complete it using the owner's actual definition, not a substitute.
            owner = (ROOT / "src/game/graphics_frame_buffer.h").read_text()
            layouts = re.findall(r"struct GraphicsFrameBuffer \{.*?\n\};", owner, re.DOTALL)
            self.assertEqual(len(layouts), 1)
            compatibility = directory / "host-types.h"
            compatibility.write_text(
                '#ifndef __ASSEMBLER__\n#include "src/types.h"\n'
                '#include "src/psyq/libgte.h"\n#include "src/psyq/libgpu.h"\n'
                '#include "src/psyq/libgs.h"\n' + layouts[0] + "\n#endif\n"
            )
            if mutation is not None:
                original, replacement = {
                    "timer": ("timer = step - 1;", "timer = step - 2;"),
                    "sign": ("delta = first - value;", "delta = value - first;"),
                }[mutation]
                code = SOURCE.read_text()
                self.assertEqual(code.count(original), 1, "mutation must be active")
                code = code.replace(original, replacement)
                source = directory / "mutated.c"
                source.write_text(normalized(code))
            fixture = directory / "fixture.c"
            fixture.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            binary = directory / "witness"
            # Keep the compiler's own temporary assembly/objects here too.
            environment = dict(os.environ, TMPDIR=str(directory))
            result = subprocess.run(
                ["cc", "-m32", "-nostdlib", "-ffreestanding", "-fno-builtin",
                 "-fno-pie", "-no-pie", "-fno-stack-protector", "-std=gnu89",
                 "-fno-strict-aliasing", "-D_LANGUAGE_C", optimization, "-I", str(ROOT),
                 "-include", str(compatibility),
                 str(source), str(fixture), str(start), "-o", str(binary)],
                capture_output=True, text=True, timeout=60, env=environment,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run(
                [str(binary)], capture_output=True, text=True, timeout=30,
            )

    def test_native_oracle_and_negative_controls(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization, mutation=None):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(result.stdout, "entry=1080 completion=8 prompt=15\n")
            for mutation in ("timer", "sign"):
                with self.subTest(optimization=optimization, mutation=mutation):
                    result = self.build_and_run(optimization, mutation)
                    self.assertEqual(result.returncode, 75, result.stdout + result.stderr)
                    self.assertIn("mismatch reason/case:", result.stdout)


if __name__ == "__main__":
    unittest.main()
