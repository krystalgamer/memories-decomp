"""ILP32 allocator witnesses with scripted callees, not SPU hardware emulation."""

import itertools
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/func_800482B0.c"
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


def scenarios():
    for upper, low, mask in itertools.product(
        (0x8000, 0x10000, 0x80000000, 0xFFFF0000, 0xFFFFFF00),
        (0, 1, 0x10, 0x20, 0x31, 0xF0), (0, 8),
    ):
        yield {"mode": upper | low, "mask": mask}
    for cursor, mask, mode in itertools.product(range(4), range(16), (1, 5, 15, 0x31)):
        yield {"cursor": cursor, "mask": mask, "mode": mode}
    for cursor, playing, mode in itertools.product(range(4), range(16), (0, 0x10, 0x20, 0x40, 0x50, 0xF0)):
        yield {"cursor": cursor, "mode": mode, "envelopes": [int(playing >> i & 1) for i in range(4)]}
    for priority, value, cursor in itertools.product((0, 1, 127, 128, 254, 255), (0, 127, 128, 255), range(4)):
        yield {"priorities": [priority, 255, priority, 255], "value": value, "cursor": cursor}
    for sound_id, mode in itertools.product(
        (0, 7, 0x7FFF, 0x8000, 0xFFFF, 0x10007, 0x10000, 0x7FFF0007, -65529, -1),
        (0, 1, 0x10, 0x20, 0x40, 0xF0),
    ):
        yield {"id": sound_id, "mode": mode}
        yield {"id": sound_id, "mode": mode, "missing": True}
    for pitch, pan, volume in itertools.product((-32768, -1, 0, 32767), (-32768, -128, 0, 127, 32767), (0, 255)):
        yield {"pitch": pitch, "pan": pan, "volume": volume, "envelopes": [0] * 4}
    for mode, after, swap in itertools.product((0, 0x10, 0x30), range(4), (False, True)):
        yield {"mode": mode, "cursor_after_envelope": after, "swap_state": swap,
               "ids_after_envelope": [8, 7, 9, 7], "script": [1, 0, 1, -32768, 0]}
        yield {"mode": mode, "envelopes": [0] * 4, "cursor_after_start": after}


def reference(case):
    states = [{"cursor": case.get("cursor", 0), "ids": list(case.get("ids", [7] * 4)),
               "priorities": list(case.get("priorities", [128] * 4))} for _ in range(2)]
    current, queries = 0, 0
    events = []
    key = case.get("id", 7) & 0xFFFF
    mode = case.get("mode", 0) & 0xFF

    def event(kind, args):
        events.append([kind, [(arg & 0xFFFFFFFF) for arg in args] + [0] * (7 - len(args)),
                       states[current]["cursor"]])

    def envelope(slot):
        nonlocal current, queries
        script = case.get("script", [])
        value = script[queries] if queries < len(script) else case.get("envelopes", [1] * 4)[slot]
        event(2, [slot, value])
        queries += 1
        if queries == 1:
            if case.get("swap_state"):
                current = 1
            if "cursor_after_envelope" in case:
                states[current]["cursor"] = case["cursor_after_envelope"]
            if "ids_after_envelope" in case:
                states[current]["ids"] = list(case["ids_after_envelope"])
        return value

    def start(slot):
        event(3, [key, slot, case.get("pitch", -123), case.get("volume", 255),
                  case.get("pan", -37), mode, case.get("value", 128)])
        states[current]["ids"][slot] = key
        states[current]["priorities"][slot] = case.get("value", 128)
        if "cursor_after_start" in case:
            states[current]["cursor"] = case["cursor_after_start"]

    def select():
        if case.get("missing"):
            return
        if key & 0x8000:
            event(0, [key, 0])
            return
        if mode & 15:
            mask = case.get("mask", 0)
            event(1, [mode & 15, mask])
            if mask:
                slot = next(slot for slot in range(4) if mask & (1 << slot))
                states[current]["cursor"] = slot
                start(slot)
                return
        requested = mode >> 4
        if requested:
            matches = 0
            for slot in range(4):
                if envelope(slot) and states[current]["ids"][slot] == key:
                    matches += 1
                    if matches == requested:
                        states[current]["cursor"] = slot
                        start(slot)
                        return
        for quiet in (True, False):
            for _ in range(4):
                cursor = states[current]["cursor"]
                accepted = envelope(cursor) == 0 if quiet else (
                    states[current]["priorities"][cursor] <= case.get("value", 128))
                if accepted:
                    start(states[current]["cursor"])
                states[current]["cursor"] = (states[current]["cursor"] + 1) % 4
                if accepted:
                    return

    select()
    return states, current, events


def initializer(value):
    if isinstance(value, (list, tuple)):
        return "{" + ",".join(initializer(item) for item in value) + "}"
    return str(int(value)) if value >= 0 else str(value)


def fixture_rows():
    rows = []
    for case in scenarios():
        states, current, events = reference(case)
        script = case.get("script", [])
        rows.append(initializer([
            case.get("id", 7), case.get("pitch", -123), case.get("volume", 255),
            case.get("pan", -37), case.get("mode", 0), case.get("value", 128),
            case.get("cursor", 0), bool(case.get("missing")), case.get("mask", 0),
            case.get("ids", [7] * 4), case.get("priorities", [128] * 4),
            case.get("envelopes", [1] * 4), script + [0] * (5 - len(script)), len(script),
            bool(case.get("swap_state")), case.get("cursor_after_envelope", -1),
            case.get("cursor_after_start", -1), "ids_after_envelope" in case,
            case.get("ids_after_envelope", [0] * 4), current,
            [state["cursor"] for state in states], [state["ids"] for state in states],
            [state["priorities"] for state in states], len(events),
            events or [[0, [0] * 7, 0]],
        ]))
    return ",\n".join(rows)


WITNESS = r"""
#include "src/types.h"
#include "src/psyq/libspu.h"
#include "src/game/sound.h"
#include "src/game/sound_output_state.h"
#include "src/game/sound_voice_selection.h"
#include "src/game/sound_effect_voices.h"
#include "src/game/sound_voice_allocator.h"
typedef char ilp32[sizeof(void *) == 4 && sizeof(long) == 4 ? 1 : -1];
typedef struct { u32 kind, args[7], cursor; } Event;
typedef struct {
    s32 id, pitch;
    u32 volume;
    s32 pan;
    u32 mode, value, cursor, missing, mask;
    u16 ids[4];
    u8 priorities[4];
    s16 envelopes[4], script[5];
    u32 script_size, swap;
    s32 after_envelope, after_start;
    u32 replace_ids;
    u16 replacement_ids[4];
    u32 final_state, cursors[2];
    u16 final_ids[2][4];
    u8 final_priorities[2][4];
    u32 event_count;
    Event events[10];
} Case;
static const Case cases[] = {
@CASES@
};
static SDValue states[2], expected[2];
SDValue *g_SDValue;
static u16 table[65536];
static const Case *test;
static u32 case_index, events, queries;

static void output(const char *p, u32 size)
{
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(p), "d"(size) : "memory", "cc");
}
static void number(u32 value)
{
    char buffer[12], reversed[12];
    u32 i = 0, j = 0;
    do { reversed[i++] = '0' + value % 10; value /= 10; } while (value);
    while (i) buffer[j++] = reversed[--i];
    buffer[j++] = '\n'; output(buffer, j);
}
static void fail(u32 line) __attribute__((noreturn));
static void fail(u32 line)
{
    output("line: ", 6); number(line); output("case: ", 6); number(case_index);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(73) : "memory");
    __builtin_unreachable();
}
#define CHECK(c) do { if (!(c)) fail(__LINE__); } while (0)

static void event(u32 kind, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g)
{
    const Event *want;
    CHECK(events < test->event_count);
    want = &test->events[events++];
    CHECK(want->kind == kind && want->cursor == g_SDValue->field_0435);
    CHECK(want->args[0] == a && want->args[1] == b && want->args[2] == c);
    CHECK(want->args[3] == d && want->args[4] == e && want->args[5] == f && want->args[6] == g);
}
s16 func_800451E0(u16 id, s32 unused)
{ event(0, id, unused, 0, 0, 0, 0, 0); return 0; }
s32 func_80047F38(u8 group)
{ event(1, group, test->mask, 0, 0, 0, 0, 0); return test->mask; }
void SpuGetVoiceEnvelope(int voice, short *envelope)
{
    s16 value;
    u32 i;
    CHECK(voice >= 20 && voice < 24);
    value = queries < test->script_size ? test->script[queries] : test->envelopes[voice - 20];
    event(2, voice - 20, (s32)value, 0, 0, 0, 0, 0);
    *envelope = value;
    if (++queries == 1) {
        if (test->swap) g_SDValue = &states[1];
        if (test->after_envelope >= 0) g_SDValue->field_0435 = test->after_envelope;
        if (test->replace_ids) {
            for (i = 0; i < 4; i++) g_SDValue->voice_ids[i] = test->replacement_ids[i];
        }
    }
}
void func_8004803C(u16 id, u8 voice, s32 pitch, u8 volume, s16 pan, u8 flags, u8 value)
{
    event(3, id, voice, pitch, volume, (s32)pan, flags, value);
    CHECK(voice < 4);
    g_SDValue->voice_ids[voice] = id;
    g_SDValue->field_040C[voice] = value;
    if (test->after_start >= 0) g_SDValue->field_0435 = test->after_start;
}
int main(void)
{
    u32 i, j;
    for (case_index = 0; case_index < sizeof(cases) / sizeof(cases[0]); case_index++) {
        test = &cases[case_index]; events = queries = 0;
        for (i = 0; i < sizeof(states); i++) ((u8 *)states)[i] = 0xA5;
        for (i = 0; i < 2; i++) {
            states[i].field_0435 = test->cursor; states[i].field_043C = table;
            for (j = 0; j < 4; j++) {
                states[i].voice_ids[j] = test->ids[j];
                states[i].field_040C[j] = test->priorities[j];
            }
        }
        for (i = 0; i < sizeof(states); i++) ((u8 *)expected)[i] = ((u8 *)states)[i];
        for (i = 0; i < 2; i++) {
            expected[i].field_0435 = test->cursors[i];
            for (j = 0; j < 4; j++) {
                expected[i].voice_ids[j] = test->final_ids[i][j];
                expected[i].field_040C[j] = test->final_priorities[i][j];
            }
        }
        g_SDValue = states; table[(u16)test->id] = test->missing ? 0xFFFF : 3;
        func_800482B0(test->id, test->pitch, test->volume, test->pan, test->mode, test->value);
        CHECK(events == test->event_count && g_SDValue == &states[test->final_state]);
        for (i = 0; i < sizeof(states); i++) CHECK(((u8 *)states)[i] == ((u8 *)expected)[i]);
    }
    output("voice allocator cases: ", 23); number(case_index);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86, a host compiler, and ILP32 execution",
)
class SoundVoiceAllocatorTests(unittest.TestCase):
    def build_and_run(self, optimization, mutation=""):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        scratch = ROOT / "tmp/test-sound-voice-allocator"
        scratch.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=scratch) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation:
                edits = {
                    "id-narrowing": ("observed = (u16)sound_id;", "observed = id;"),
                    "priority-equality": ("value >= g_SDValue", "value > g_SDValue"),
                    "cursor-wrap": ("field_0435 & 3", "field_0435 & 1"),
                    "occurrence": ("& 0xF0) >> 4", "& 0xF0) >> 3"),
                    "envelope-order": ("SpuGetVoiceEnvelope(index + 0x14", "SpuGetVoiceEnvelope((3 - index) + 0x14"),
                }
                old, new = edits[mutation]
                text = SOURCE.read_text()
                self.assertIn(old, text)
                text = text.replace(old, new)
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "' + str((SOURCE.parent / match[1]).resolve()) + '"',
                    text, flags=re.MULTILINE,
                )
                source = directory / "mutated.c"
                source.write_text(text)
            fixture = directory / "fixture.c"
            fixture.write_text(WITNESS.replace("@CASES@", fixture_rows()))
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
                obj = directory / (name + ".o")
                result = subprocess.run(["cc", *flags, "-c", str(path), "-o", str(obj)],
                                        capture_output=True, text=True, timeout=60)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objects.append(str(obj))
            binary = directory / "witness"
            result = subprocess.run(["cc", *flags, *objects, "-o", str(binary)],
                                    capture_output=True, text=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run([str(binary)], capture_output=True, text=True, timeout=30)

    def test_selection_arguments_and_cursor_contract(self):
        self.assertEqual(len(list(scenarios())), 1004)
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(result.stdout, "voice allocator cases: 1004\n")

    def test_behavioral_mutations_are_rejected(self):
        for mutation in ("id-narrowing", "priority-equality", "cursor-wrap", "occurrence", "envelope-order"):
            with self.subTest(mutation=mutation):
                result = self.build_and_run("-O2", mutation)
                self.assertEqual(result.returncode, 73, result.stdout + result.stderr)
                self.assertIn("case: ", result.stdout)


if __name__ == "__main__":
    unittest.main()
