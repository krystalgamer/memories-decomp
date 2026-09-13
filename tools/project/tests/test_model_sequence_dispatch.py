"""ILP32 behavioral witnesses for the real animation-sequence dispatch TU.

Python independently interprets finite sequence programs and emits complete
guarded-memory checkpoints, including ordered callback identity and pointer slots.
The actual C TU is compiled separately, with no libc or project build dependency.
An actual omitted-store mutation is checked against unchanged oracle expectations;
a separate wrong-expectation control exercises the traveling-state comparator.

Run from the repository root against the tracked implementation.
All generated fixtures, objects and binaries are automatically cleaned beneath tmp.
"""
from __future__ import annotations

import os
from pathlib import Path
import platform
import re
import shlex
import shutil
import struct
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/model_sequence_dispatch.c"
EXPECTED_CASES = 377
EXPECTED_CALLBACKS = 316
SEMANTIC_TRAVELING_MISMATCH = 21
BASE = 0x10000000
FN = 0x20000000
PRIM, OTHER, CTX, PARAM, ALT_PARAM, HANDLERS, ALT_HANDLERS = (
    32, 512, 768, 1024, 2048, 3072, 3200
)
COMMAND, ALT_COMMAND = 4096, 4096 + 65536 * 4 + 64
SIZE = ALT_COMMAND + 256 + 32
FIELDS = {
    "size": (4, "H"), "num": (6, "H"), "ii": (8, "H"),
    "aframe": (10, "H"), "sid": (12, "B"), "speed": (13, "b"),
    "srcii": (14, "H"), "rframe": (16, "h"), "tframe": (18, "H"),
    "ci": (20, "H"), "ti": (22, "H"), "traveling": (27, "B"),
}
START = """
.text
.globl _start
_start:
    and $-16, %esp
    call main
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""


def normal(payload=0, duration=1, handler=0):
    assert 0 <= handler < 128 and 0 <= duration <= 255
    return payload | duration << 16 | handler << 24


def marker(jump=0, condition=0, action=0, stop=False):
    return (0xC0000000 if stop else 0x80000000) | action << 23 | condition << 16 | jump


def baseline():
    b = bytearray([0xA5]) * SIZE
    b[COMMAND:COMMAND + 65536 * 4] = struct.pack("<I", normal()) * 65536
    return b


class Memory:
    def __init__(self):
        self.data = baseline()
        self.dirty = set()
        self.pointers = set()
        self.writes = []

    def read(self, offset, kind="I"):
        return struct.unpack_from("<" + kind, self.data, offset)[0]

    def write(self, offset, value, kind="I", pointer=False):
        size = struct.calcsize(kind)
        value &= (1 << (size * 8)) - 1
        self.data[offset:offset + size] = value.to_bytes(size, "little")
        self.dirty.update(range(offset // 4 * 4, (offset + size + 3) // 4 * 4, 4))
        if pointer:
            assert offset % 4 == 0 and size == 4
            self.pointers.add(offset)
        self.writes.append((offset, value, size | (0x80 if pointer else 0)))

    def snapshot(self):
        return [(o, self.read(o), 4 | (0x80 if o in self.pointers else 0))
                for o in sorted(self.dirty)]

    def get(self, track, field):
        offset, kind = FIELDS[field]
        return self.read(track + offset, kind)

    def set(self, track, field, value):
        offset, kind = FIELDS[field]
        self.write(track + offset, value, kind)


class Program:
    def __init__(self, name, speed=3, **fields):
        self.name, self.mem, self.controls = name, Memory(), []
        self.tracks = [PRIM + 4]
        m = self.mem
        m.write(PRIM, 96, "H")
        m.write(PRIM + 2, 1, "H")
        m.write(OTHER, 41, "H")
        m.write(OTHER + 2, 0, "H")
        for offset, value in ((0, PRIM), (24, HANDLERS), (28, COMMAND), (32, PARAM)):
            m.write(CTX + offset, BASE + value, pointer=True)
        m.write(CTX + 20, 4)
        for table, ids in ((HANDLERS, (0, 1, 2)), (ALT_HANDLERS, (3, 3, 3))):
            for i, identity in enumerate(ids):
                m.write(table + 4 * (i + 1), FN + identity, pointer=True)
        for i in range(64):
            m.write(ALT_COMMAND + i * 4, normal(i, 9, 2))
        self.track(PRIM + 4, speed=speed, **fields)
        for i in range(32):
            self.command(i, normal(i * 3, (i % 5) + 1, i % 3))

    def track(self, address, **overrides):
        fields = dict(size=9, num=1, ii=0xFFFF, aframe=4, sid=7, speed=3,
                      srcii=0, rframe=0, tframe=0, ci=6, ti=7, traveling=0x65)
        fields.update(overrides)
        for key, value in fields.items():
            self.mem.set(address, key, value)

    def command(self, index, value):
        self.mem.write(COMMAND + index * 4, value)

    def control(self, result=0, fields=None, writes=()):
        self.controls.append((result, fields or {}, writes))

    def interpret(self):
        m = self.mem
        initial = m.snapshot()
        events = []
        command_base = m.read(CTX + 28) - BASE
        param_base = m.read(CTX + 32) - BASE
        handler_base = m.read(CTX + 24) - BASE
        cursor = m.read(CTX) - BASE + 4
        count = m.read(m.read(CTX) - BASE + 2, "H")

        def word(index):
            return m.read(command_base + (index & 65535) * 4)

        def resolve(index, address):
            # This is a small bytecode interpreter: branch, halt, or fallthrough.
            step = -1 if m.get(address, "speed") < 0 else 1
            for _ in range(32):
                index &= 65535
                opcode = word(index)
                if not opcode >> 31:
                    return index
                condition, action = (opcode >> 16) & 127, (opcode >> 23) & 127
                matches = not condition or condition == m.get(address, "sid")
                if opcode >> 30 == 2 and matches:
                    if condition or action:
                        m.set(address, "sid", action)
                    index = opcode & 65535
                elif opcode >> 30 == 3 and action == 1 and matches:
                    return 65535
                else:
                    index += step
            raise AssertionError(f"{self.name}: nonfinite marker program")

        for _ in range(count):
            address = cursor
            active = m.get(address, "aframe")
            if active:
                if active != 65535:
                    m.set(address, "aframe", active - 1)
                direction = -1 if m.get(address, "speed") < 0 else 1
                remaining = m.get(address, "rframe")
                if remaining in (0x6000, 0x7000):
                    direction = {0x6000: -1, 0x7000: 1}[remaining]
                    m.set(address, "rframe", 0)
                primary, other = ("ti", "ci") if direction > 0 else ("ci", "ti")
                for retry in range(16):
                    if not m.get(address, "rframe") or not m.get(address, "tframe"):
                        m.set(address, "traveling", 0)
                        anchor = m.get(address, primary)
                        stopped = False
                        if word(anchor) >> 31:
                            anchor = resolve(anchor + direction, address)
                            m.set(address, primary, anchor)
                            stopped = anchor == 65535
                        if not stopped:
                            m.set(address, other, anchor)
                            anchor = resolve(anchor + direction, address)
                            m.set(address, primary, anchor)
                            stopped = anchor == 65535
                        if stopped:
                            m.set(address, "aframe", 0)
                            break
                        duration = (word(m.get(address, "ti")) >> 16) & 255
                        m.set(address, "tframe", duration * 16)
                        if direction > 0:
                            m.set(address, "rframe", duration * 16 -
                                  (m.get(address, "rframe") & 65535))
                    ci, ti, ii = (m.get(address, field) for field in ("ci", "ti", "ii"))
                    slot = CTX + 20 + m.read(CTX + 20) * 4
                    pointers = [BASE + address, BASE + param_base + (word(ci) & 65535) * 4,
                                BASE + param_base + (word(ti) & 65535) * 4,
                                0 if ii == 65535 else BASE + param_base + (word(ii) & 65535) * 4]
                    for i, pointer in enumerate(pointers):
                        m.write(slot + i * 4, pointer, pointer=True)
                    if ci == ti:
                        break
                    identity = m.read(handler_base + (((word(ti) >> 24) & 127) + 1) * 4) - FN
                    checkpoint = m.snapshot()
                    call = len(events)
                    result, fields, writes = (self.controls[call] if call < len(self.controls)
                                              else (0, {}, ()))
                    m.writes = []
                    for field, value in fields.items():
                        m.set(address, field, value)
                    for offset, value, kind, pointer in writes:
                        m.write(offset, value, kind, pointer)
                    events.append((identity, checkpoint, list(m.writes), result))
                    if result == 1:
                        m.set(address, "rframe", 0)
                        continue
                    if not ((word(m.get(address, "ti")) >> 16) & 255):
                        continue
                    remaining = ((m.get(address, "rframe") & 65535) -
                                 m.get(address, "speed")) & 65535
                    remaining = remaining if remaining < 32768 else remaining - 65536
                    if direction > 0 and remaining < 0:
                        remaining = 0x7000
                    if direction < 0 and remaining >= m.get(address, "tframe"):
                        remaining = 0x6000
                    m.set(address, "rframe", remaining)
                    break
                else:
                    raise AssertionError(f"{self.name}: nonfinite retry program")
            cursor += m.get(address, "size") * 4
        assert len(events) >= len(self.controls), (self.name, "unused callback controls")
        return_base = m.read(CTX) - BASE
        return initial, events, m.snapshot(), return_base + m.read(return_base, "H") * 4


def programs():
    # First case makes the safe traveling-reset negative control unambiguous.
    yield Program("traveling-reset-negative-control")
    p = Program("highest-handler-selector", ci=7, ti=7)
    p.command(8, normal(24, 3, 127))
    p.mem.write(HANDLERS + 4 * 128, FN + 2, pointer=True)
    yield p
    for direction in (-1, 1):
        p = Program(f"chained-sid-jumps-{direction}", speed=direction,
                    ci=12, ti=12, sid=7)
        p.command(12 + direction, marker(jump=20, condition=7, action=8))
        p.command(20, marker(jump=22, condition=8, action=127))
        p.command(22, normal(99, 3, 2))
        yield p
    for speed in (-128, -17, -1, 0, 1, 17, 127):
        for active in (0, 1, 65535):
            for remaining, total in ((0, 0), (0, 32), (1, 32), (31, 32),
                                     (32, 32), (-1, 0), (0x6000, 64), (0x7000, 64),
                                     (32767, 65535), (-32768, 0)):
                yield Program(f"time-speed{speed}-active{active}-r{remaining}-t{total}",
                              speed=speed, aframe=active, rframe=remaining,
                              tframe=total, ii=5 if active == 65535 else 65535)
    for speed in (-1, 1):
        for remaining, total in ((0, 32), (16, 32)):
            p = Program(f"same-index-{speed}-{remaining}", speed=speed,
                        ci=7, ti=7, rframe=remaining, tframe=total)
            if not remaining:
                p.command(7 + speed, marker(jump=7))
            yield p
    for direction in (-1, 1):
        for normalize in (False, True):
            for stop in (False, True):
                for condition in (0, 7, 8, 127):
                    for action in (0, 1, 2, 127):
                        p = Program(f"marker-{direction}-{normalize}-{stop}-{condition}-{action}",
                                    speed=direction, ci=12, ti=12, sid=7, ii=4)
                        at = 12 + direction
                        if normalize:
                            p.command(12, marker(action=2, stop=True))
                        p.command(at, marker(jump=20, condition=condition,
                                             action=action, stop=stop))
                        yield p
    for direction in (-1, 1):
        for override in (False, True):
            speed = -direction if override else direction
            sentinel = (0x7000 if direction == 1 else 0x6000) if override else 0
            p = Program(f"fresh-scan-sign-{direction}-{override}", speed=speed,
                        rframe=sentinel, ci=12, ti=12)
            # A marker mismatch steps according to speed, not override direction.
            p.command(12 + direction, marker(condition=8, action=2, stop=True))
            yield p
    for direction in (-1, 1):
        for count in (1, 2, 3):
            p = Program(f"return-one-{direction}-{count}", speed=direction, ci=12, ti=12)
            for _ in range(count):
                p.control(1)
            yield p
        p = Program(f"zero-duration-{direction}", speed=direction, ci=12, ti=12)
        p.command(13 if direction > 0 else 12, normal(33, 0, 2))
        yield p
        p = Program(f"callback-introduced-zero-{direction}", speed=direction,
                    ci=12, ti=13, rframe=8, tframe=32)
        p.control(fields={"rframe": 0},
                  writes=((COMMAND + 13 * 4, normal(39, 0, 1), "I", False),))
        yield p
        p = Program(f"callback-fields-{direction}", speed=direction, ci=12, ti=12)
        p.control(fields={"speed": -direction * 13, "rframe": 7, "tframe": 6,
                          "sid": 99, "aframe": 37, "traveling": 91, "ii": 5,
                          "ci": 3, "ti": 4})
        yield p
        p = Program(f"callback-header-tables-return-{direction}", speed=direction,
                    ci=12, ti=12)
        p.control(1, {"speed": -direction}, (
            (CTX + 20, 9, "I", False),
            (CTX, BASE + OTHER, "I", True),
            (CTX + 24, BASE + ALT_HANDLERS, "I", True),
            (CTX + 28, BASE + ALT_COMMAND, "I", True),
            (CTX + 32, BASE + ALT_PARAM, "I", True),
            (PRIM + 2, 0, "H", False),
        ))
        # Forward/reverse direction remains captured even when speed flips.
        yield p
    p = Program("live-handler-entry-and-command-payload", ci=12, ti=12)
    p.control(1, writes=(
        (HANDLERS + 4 * 3, FN + 1, "I", True),
        (COMMAND + 14 * 4, normal(101, 3, 2), "I", False),
        (PARAM + 101 * 4, 0x12345678, "I", False),
    ))
    yield p
    p = Program("three-variable-tracks-live-size-captured-count", ci=6, ti=7)
    second, third = PRIM + 4 + 12 * 4, PRIM + 4 + 12 * 4 + 8 * 4
    p.tracks += [second, third]
    p.track(second, size=8, speed=-4, ci=12, ti=12, ii=3)
    p.track(third, size=7, speed=2, ci=3, ti=4, rframe=5, tframe=16)
    p.mem.write(PRIM + 2, 3, "H")
    p.control(fields={"size": 12}, writes=(
        (PRIM + 2, 0, "H", False), (CTX, BASE + OTHER, "I", True),
        (CTX + 24, BASE + ALT_HANDLERS, "I", True),
        (CTX + 28, BASE + ALT_COMMAND, "I", True),
        (CTX + 32, BASE + ALT_PARAM, "I", True),
    ))
    yield p
    for direction in (-1, 1):
        p = Program(f"index-wrap-{direction}", speed=direction,
                    ci=0 if direction < 0 else 65535,
                    ti=0 if direction < 0 else 65535)
        p.command(65535, marker(jump=20) if direction < 0 else normal(4, 2, 1))
        if direction > 0:
            p.command(0, marker(jump=20))
        yield p
    for direction in (-1, 1):
        for duration in (0, 1, 255):
            p = Program(f"duration-{direction}-{duration}", speed=direction, ci=12, ti=12)
            p.command(13 if direction > 0 else 12, normal(100, duration, 2))
            yield p
    for result in (-1, 2, 32767):
        p = Program(f"non-one-handler-result-{result}")
        p.control(result)
        yield p


WITNESS = r"""
#include "src/types.h"
#include "src/psyq/libgte.h"
#include "src/psyq/libgpu.h"
#include "src/psyq/libgs.h"
#include "src/psyq/libhmd.h"
extern u32 *func_8005C7BC(GsARGUNIT_ANIM *);
char D_800117EC[] = "fixture assertion format";
char D_80011814[] = "fixture assertion filename";
#define OFFSET(t, m) __builtin_offsetof(t, m)
typedef char require_layout[
    sizeof(void *) == 4 && sizeof(long) == 4 && sizeof(GsSEQ) == 28 &&
    sizeof(GsARGUNIT_ANIM) == 36 &&
    OFFSET(GsSEQ, size) == 4 && OFFSET(GsSEQ, aframe) == 10 &&
    OFFSET(GsSEQ, speed) == 13 && OFFSET(GsSEQ, rframe) == 16 &&
    OFFSET(GsSEQ, tframe) == 18 && OFFSET(GsSEQ, ci) == 20 &&
    OFFSET(GsSEQ, ti) == 22 && OFFSET(GsSEQ, traveling) == 27 &&
    OFFSET(GsARGUNIT_ANIM, primp) == 0 &&
    OFFSET(GsARGUNIT_ANIM, header_size) == 20 &&
    OFFSET(GsARGUNIT_ANIM, htop) == 24 &&
    OFFSET(GsARGUNIT_ANIM, ctop) == 28 &&
    OFFSET(GsARGUNIT_ANIM, ptop) == 32 ? 1 : -1];
struct Patch { u32 offset, value, kind; };
struct Snapshot { const struct Patch *patches; u32 count; };
struct Event { u32 identity, before, actions; s32 result; };
struct Case {
    const char *name; u32 initial, final, first_event, events, end;
    u32 tracks[3];
};
#include "fixtures.h"
static u8 arena[ARENA_SIZE] __attribute__((aligned(16)));
static u8 expected[ARENA_SIZE] __attribute__((aligned(16)));
static const struct Case *current;
static u32 event_index, cases, callbacks;
static void quit(s32 code) __attribute__((noreturn));
static void print(const char *s)
{
    u32 n = 0;
    s32 syscall = 4;
    while (s[n]) n++;
    __asm__ volatile("int $0x80" : "+a"(syscall) : "b"(1), "c"(s), "d"(n) : "memory", "cc");
}
static void quit(s32 code)
{
    if (current) { print(current->name); print("\n"); }
    __asm__ volatile("int $0x80" : : "a"(1), "b"(code) : "memory", "cc");
    __builtin_unreachable();
}
int printf(const char *format, ...) { (void)format; quit(90); }
void exit(s32 status) { (void)status; quit(91); }
static s32 callback(u32 identity, GsARGUNIT_ANIM *ctx);
static s32 callback0(GsARGUNIT_ANIM *ctx) { return callback(0, ctx); }
static s32 callback1(GsARGUNIT_ANIM *ctx) { return callback(1, ctx); }
static s32 callback2(GsARGUNIT_ANIM *ctx) { return callback(2, ctx); }
static s32 callback3(GsARGUNIT_ANIM *ctx) { return callback(3, ctx); }
static u32 relocate(u32 value)
{
    static s32 (*const functions[])(GsARGUNIT_ANIM *) =
        {callback0, callback1, callback2, callback3};
    if (!value) return 0;
    if (value >= 0x20000000 && value < 0x20000004)
        return (u32)functions[value - 0x20000000];
    if (value < 0x10000000 || value >= 0x10000000 + ARENA_SIZE) quit(12);
    return (u32)(arena + value - 0x10000000);
}
static void reset(u8 *storage)
{
    u32 i;
    for (i = 0; i < ARENA_SIZE; i++) storage[i] = 0xA5;
    for (i = 0; i < 65536; i++) ((u32 *)(storage + COMMAND_OFFSET))[i] = 0x10000;
}
static void apply(u8 *storage, u32 index)
{
    const struct Snapshot *s = snapshots + index;
    u32 i, j;
    for (i = 0; i < s->count; i++) {
        const struct Patch *p = s->patches + i;
        u32 value = p->kind & 0x80 ? relocate(p->value) : p->value;
        for (j = 0; j < (p->kind & 0x7F); j++)
            storage[p->offset + j] = (u8)(value >> (8 * j));
    }
}
static void check(u32 snapshot)
{
    u32 i, t;
    reset(expected);
    apply(expected, snapshot);
    for (t = 0; t < 3 && current->tracks[t]; t++) {
        u32 a = current->tracks[t];
        if (arena[a+16] != expected[a+16] || arena[a+17] != expected[a+17]) quit(20);
        if (arena[a+27] != expected[a+27]) quit(21);
    }
    for (i = 0; i < ARENA_SIZE; i++) if (arena[i] != expected[i]) quit(23);
}
static s32 callback(u32 identity, GsARGUNIT_ANIM *ctx)
{
    const struct Event *event;
    if ((u8 *)ctx != arena + CONTEXT_OFFSET) quit(11);
    if (event_index >= current->events) quit(25);
    event = events + current->first_event + event_index++;
    if (event->identity != identity) quit(22);
    check(event->before);
    apply(arena, event->actions);
    callbacks++;
    return event->result;
}
static void report_value(const char *label, u32 value)
{
    char output[32], digits[10];
    u32 length = 0, n = 0;
    print(label);
    do { digits[n++] = '0' + value % 10; value /= 10; } while (value);
    while (n) output[length++] = digits[--n];
    output[length++] = '\n'; output[length] = 0;
    print(output);
}
int main(void)
{
    u32 i;
    for (i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        u32 *end;
        current = test_cases + i;
        event_index = 0;
        reset(arena);
        apply(arena, current->initial);
        end = func_8005C7BC((GsARGUNIT_ANIM *)(arena + CONTEXT_OFFSET));
        if ((u8 *)end != arena + current->end) quit(24);
        if (event_index != current->events) quit(26);
        check(current->final);
        cases++;
    }
    current = 0;
    report_value("cases=", cases);
    report_value("callbacks=", callbacks);
    return 0;
}
"""


def fixtures(directory):
    snapshots, events, cases = [], [], []

    def snapshot(patches):
        snapshots.append(patches)
        return len(snapshots) - 1

    for p in programs():
        initial, checkpoints, final, end = p.interpret()
        first = len(events)
        ini = snapshot(initial)
        for identity, before, actions, result in checkpoints:
            events.append((identity, snapshot(before), snapshot(actions), result))
        cases.append((p.name, ini, snapshot(final), first, len(checkpoints), end, p.tracks))
    with (directory / "fixtures.h").open("w") as f:
        f.write(f"#define ARENA_SIZE {SIZE}\n#define COMMAND_OFFSET {COMMAND}\n"
                f"#define CONTEXT_OFFSET {CTX}\n")
        for i, patches in enumerate(snapshots):
            content = ",".join(f"{{{o},0x{v:08x},{k}}}" for o, v, k in patches) or "{0,0,0}"
            f.write(f"static const struct Patch patch_{i}[] = {{{content}}};\n")
        f.write("static const struct Snapshot snapshots[] = {\n")
        for i, patches in enumerate(snapshots):
            f.write(f"{{patch_{i},{len(patches)}}},\n")
        f.write("};\nstatic const struct Event events[] = {\n")
        for event in events:
            f.write("{" + ",".join(map(str, event)) + "},\n")
        f.write("};\nstatic const struct Case test_cases[] = {\n")
        for name, ini, final, first, count, end, tracks in cases:
            track_text = ",".join(map(str, tracks + [0] * (3 - len(tracks))))
            f.write(f'{{"{name}",{ini},{final},{first},{count},{end},{{{track_text}}}}},\n')
        f.write("};\n")
    return len(cases), len(events)


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class ModelSequenceDispatchTests(unittest.TestCase):
    def setUp(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        self.source = SOURCE.resolve()
        self.source.relative_to(ROOT)
        self.original = self.source.read_text()
        (ROOT / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(
            prefix="model-sequence-dispatch-", dir=ROOT / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.directory = Path(temporary.name)
        self.environment = os.environ.copy()
        self.environment["TMPDIR"] = str(self.directory)
        self.assertEqual(fixtures(self.directory), (EXPECTED_CASES, EXPECTED_CALLBACKS))
        (self.directory / "witness.c").write_text(WITNESS)
        (self.directory / "start.S").write_text(START)

    def tearDown(self):
        self.assertEqual(self.source.read_text(), self.original, "SOURCE must remain unchanged")

    def source_without_traveling_reset(self):
        first = next(programs())
        self.assertEqual(first.mem.get(PRIM + 4, "traveling"), 0x65)
        _, checkpoints, _, _ = first.interpret()
        self.assertTrue(checkpoints)
        self.assertEqual(first.mem.get(PRIM + 4, "traveling"), 0)
        store = "track->traveling = 0;"
        self.assertEqual(self.original.count(store), 1,
                         "require exactly one actual traveling-reset store")
        text = self.original.replace(store, "", 1)
        text = re.sub(
            r'^#include "([^"]+)"',
            lambda match: '#include "' + str((self.source.parent / match[1]).resolve()) + '"',
            text, flags=re.MULTILINE,
        )
        source = self.directory / "omitted-traveling-reset.c"
        source.write_text(text)
        return source

    def witness_with_wrong_expectation(self):
        # Alter only the first expected callback's traveling byte. This control
        # tests the comparator, not the actual source mutation tested separately.
        text = (self.directory / "fixtures.h").read_text()
        line = next(line for line in text.splitlines()
                    if line.startswith("static const struct Patch patch_1[]"))
        offset = (PRIM + 4 + 27) // 4 * 4
        expression = rf"\{{{offset},0x([0-9a-f]{{8}}),4\}}"
        match = re.search(expression, line)
        self.assertIsNotNone(match)
        value = int(match[1], 16) | 0x65000000
        wrong_line = re.sub(expression, f"{{{offset},0x{value:08x},4}}", line)
        self.assertNotEqual(wrong_line, line)
        (self.directory / "wrong-expectation-fixtures.h").write_text(
            text.replace(line, wrong_line))
        witness = self.directory / "wrong-expectation-witness.c"
        witness.write_text(WITNESS.replace('"fixtures.h"', '"wrong-expectation-fixtures.h"'))
        return witness

    def run_witness(self, optimization, source=None, witness=None):
        source = self.source if source is None else source
        witness = self.directory / "witness.c" if witness is None else witness
        actual_object = self.directory / "actual.o"
        witness_object = self.directory / "witness.o"
        executable = self.directory / "witness"
        flags = [
            "cc", "-m32", optimization, "-std=c99", "-nostdlib", "-fno-pie", "-no-pie",
            "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
            "-fno-strict-aliasing",
        ]
        commands = [
            flags + ["-c", str(source), "-o", str(actual_object)],
            flags + ["-I", str(ROOT), "-I", str(self.directory), "-c",
                     str(witness), "-o", str(witness_object)],
            flags + [str(actual_object), str(witness_object),
                     str(self.directory / "start.S"), "-o", str(executable)],
        ]
        for command in commands:
            result = subprocess.run(command, cwd=ROOT, env=self.environment, text=True,
                                    capture_output=True, timeout=120)
            self.assertEqual(result.returncode, 0,
                             shlex.join(command) + "\n" + result.stdout + result.stderr)
        return subprocess.run([str(executable)], cwd=ROOT, env=self.environment,
                              capture_output=True, text=True, timeout=120)

    def assert_traveling_mismatch(self, result):
        self.assertEqual(
            result.returncode, SEMANTIC_TRAVELING_MISMATCH,
            f"expected traveling mismatch, not crash/other failure; "
            f"exit={result.returncode}\n{result.stdout}{result.stderr}",
        )
        self.assertEqual(result.stdout, "traveling-reset-negative-control\n")
        self.assertEqual(result.stderr, "")

    def test_actual_sequence_matches_oracle_at_o0_and_o2(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization)
                self.assertEqual(
                    result.returncode, 0,
                    f"exit={result.returncode}; 11/12=pointer/layout, 20=rframe, "
                    "21=traveling, 22=callback identity, 23=guarded storage, "
                    "24=return pointer, 25/26=callback count, 90/91=printf/exit\n"
                    + result.stdout + result.stderr,
                )
                self.assertEqual(
                    result.stdout,
                    f"cases={EXPECTED_CASES}\ncallbacks={EXPECTED_CALLBACKS}\n",
                )
                self.assertEqual(result.stderr, "")

    def test_actual_omitted_traveling_store_is_semantically_rejected(self):
        source = self.source_without_traveling_reset()
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                self.assert_traveling_mismatch(self.run_witness(optimization, source=source))

    def test_wrong_expectation_comparator_control_is_rejected(self):
        witness = self.witness_with_wrong_expectation()
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                self.assert_traveling_mismatch(self.run_witness(optimization, witness=witness))


if __name__ == "__main__":
    unittest.main()
