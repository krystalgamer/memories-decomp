"""Executable scene-transition witnesses, not a graphics or sound emulator.

The implementation and opaque helpers are separate translation units without
LTO. A Linux i386 startup makes the real model layouts usable without multilib
libc; only that test bootstrap is assembly. All artifacts stay beneath tmp/.
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
SOURCE = REPOSITORY / "src/game/model_intro_controller.c"
SOURCE_MARKER = (
    '\n#include "../psyq/rand.h"\n'
    '#include "model.h"\n'
    "#define MODEL_GRAPHICS_STATE_SCENE_BYTES\n"
)
START = """
.text
.globl _start
_start:
    mov (%esp), %eax
    lea 4(%esp), %edx
    and $-16, %esp
    sub $8, %esp
    push %edx
    push %eax
    call main
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

WITNESS = r"""
#include "src/types.h"
#include "src/psyq/rand.h"
#include "src/game/model.h"
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
#include "src/game/model_graphics_state.h"
#include "src/game/model_scene_states.h"
#include "src/game/model_scene_imports.h"
#include "src/game/high_memory_addresses.h"
#include "src/game/model_handler_state.h"
#include "src/game/model_state_setters.h"
#include "src/game/model_slot_properties.h"
#include "src/game/model_effect_requests.h"
#include "src/game/model_control_slot_animation.h"
#include "src/game/file_transfer.h"
#include "src/game/file_names.h"
#include "src/game/sound_init.h"
#include "src/game/sound_output.h"
#include "src/game/sound_pending_entries.h"

ModelSlot D_800F2C40[MODEL_SLOT_COUNT];
s8 D_8009AF9A;
ModelDebugState D_8009B004;
/* A genuine four-byte allocation, not indexing past a scalar frame override.
   Both implementation owner views resolve to the same linker symbol. */
u8 D_8009AFA4[4] = {1, 0, 0, 0};
s32 D_80010030;
void *D_80010034;
u8 D_800114F8[] = "SU";

typedef struct { s32 kind, a, b, c, d; } Event;
static Event events[32];
static s32 count, error, random_count, random_values[3];
static s32 status, poll_result, status_calls, poll_calls;
static s32 activate_action, animation_action, effect_action, random_action;
static s32 prelude_phase, poll_action, status_action;
static u8 sound_data[4];

#define ACTIVE D_8009AFA4[3]
#define FLAGS (D_8009B004.bytes)
#define CHECK(condition, code) do { if (!(condition)) return (code); } while (0)

static void clear(void *pointer, u32 size)
{
    volatile u8 *p = pointer;
    while (size--) *p++ = 0;
}

static void record(s32 kind, s32 a, s32 b, s32 c, s32 d)
{
    if (count >= 32) { error = 1; return; }
    events[count].kind = kind;
    events[count].a = a;
    events[count].b = b;
    events[count].c = c;
    events[count].d = d;
    count++;
}

void Model_SetFrameStepOverride(s32 value)
{
    record(1, value, 0, 0, 0);
    D_8009AFA4[0] = value;
    if (prelude_phase != -99) D_8009AF9A = prelude_phase;
}

FileTransferDescriptor *File_RequestAsyncTransfer(
    s32 first, u8 *name, s32 sector, s32 length,
    FileTransferCallback callback, s32 sixth, s32 seventh)
{
    record(2, first, sector, length, seventh);
    if (name != D_800114F8 || callback != 0 || sixth != 0) error = 2;
    return 0;
}

void File_WaitForTransfers(void) { record(3, 0, 0, 0, 0); }

void func_80059590(s32 index, s32 fourth, s32 first, s32 second, s32 third)
{
    record(4, index, fourth, first, second);
    if (third || ACTIVE || FLAGS[0] || FLAGS[1] ||
        D_800F2C40[0].field_E1F || D_800F2C40[1].field_E1F)
        error = 3;
}

void func_801807B0(void) { record(5, 0, 0, 0, 0); }
void func_80181C4C(s32 value) { record(6, value, 0, 0, 0); }
void func_80049394(u16 *value)
{
    record(7, value == sound_data, 0, 0, 0);
}
void func_80047314(u32 value) { record(8, value, 0, 0, 0); }
void func_800493F8(void) { record(9, 0, 0, 0, 0); }
void func_8004763C(void) { record(10, 0, 0, 0, 0); }

void func_80050584(s32 index)
{
    record(11, index, 0, 0, 0);
    if (index < 0 || index > 1) { error = 4; return; }
    if (activate_action == 1) D_800F2C40[index].field_E1F = 1;
    if (activate_action == 2 || activate_action == 3) {
        ACTIVE ^= 1;
        if (activate_action == 3) D_800F2C40[ACTIVE].field_E1F = 1;
    }
    if (activate_action == 4) D_800F2C40[ACTIVE].field_E0F = 1;
    if (activate_action == 5) FLAGS[1] = 1;
}

void Model_ControlSlotAnimation(s32 index, s32 animation, s32 flag)
{
    record(12, index, animation, flag, 0);
    if (animation_action == 1) D_800F2C40[index].field_E15 = 250;
    if (animation_action == 2) ACTIVE ^= 1;
}

void func_80059F18(s32 first, s32 second, s32 third, s32 fourth)
{
    record(13, first, second, third, fourth);
    if (effect_action) D_8009AF9A = 10;
}

int rand(void)
{
    s32 result;
    record(14, random_count, 0, 0, 0);
    if (random_count >= 3) { error = 5; return 0; }
    result = random_values[random_count++];
    if (random_action && random_count == 1) ACTIVE ^= 1;
    return result;
}

s32 func_80180A24(void)
{
    record(15, 0, 0, 0, 0);
    poll_calls++;
    if (poll_action) FLAGS[1] = 1;
    return poll_result;
}

s32 SD_GetStatusFlags(void)
{
    record(16, 0, 0, 0, 0);
    status_calls++;
    if (status_action) D_800F2C40[0].field_E1F = 1;
    return status;
}

static void setup(s32 phase, s32 active)
{
    s32 i;
    clear(D_800F2C40, sizeof(D_800F2C40));
    clear(events, sizeof(events));
    for (i = 0; i < sizeof(ModelSlot); i++) ((u8 *)&D_800F2C40[2])[i] = 0xA5;
    D_8009AFA4[0] = 0x12;
    D_8009AFA4[1] = 0x34;
    D_8009AFA4[2] = 0x56;
    ACTIVE = active;
    D_8009AF9A = phase;
    D_8009B004.word = 0xBBAA0000;
    D_80010030 = 0x12345678;
    D_80010034 = sound_data;
    count = error = random_count = status = poll_result = status_calls = poll_calls = 0;
    activate_action = animation_action = effect_action = random_action = 0;
    poll_action = status_action = 0;
    prelude_phase = -99;
    random_values[0] = 0;
    random_values[1] = (23 << 8) + 255;
    random_values[2] = (48 << 8) + 128;
}

static s32 invariant(void)
{
    s32 i;
    if (error || count < 2 || events[0].kind != 1 || events[0].a != 1 ||
        D_8009AFA4[0] != 1 || D_8009AFA4[1] != 0x34 || D_8009AFA4[2] != 0x56 ||
        (D_8009B004.word & 0xFFFF0000) != 0xBBAA0000) return 0;
    for (i = 0; i < sizeof(ModelSlot); i++)
        if (((u8 *)&D_800F2C40[2])[i] != 0xA5) return 0;
    return 1;
}

static s32 find(s32 kind)
{
    s32 i;
    for (i = 0; i < count; i++) if (events[i].kind == kind) return i;
    return -1;
}

static s32 initialization(void)
{
    static const s32 order[] = {1, 2, 3, 4, 4, 5, 6, 7, 8, 16};
    s32 i;
    setup(-1, 1);
    FLAGS[0] = FLAGS[1] = 255;
    D_800F2C40[0].field_E1F = D_800F2C40[1].field_E1F = 1;
    func_800507D0();
    CHECK(invariant() && count == 10, 10);
    for (i = 0; i < 10; i++) CHECK(events[i].kind == order[i], 11);
    CHECK(events[1].a == 1 && events[1].b == 1223 &&
          events[1].c == 16 && events[1].d == 0x12345678, 12);
    CHECK(events[3].a == 0 && events[4].a == 1 &&
          events[3].b == 2 && events[4].b == 2 &&
          !events[3].c && !events[3].d && !events[4].c && !events[4].d, 13);
    CHECK(!events[6].a && events[7].a == 1 && events[8].a == 0x7330, 14);
    CHECK(D_8009AF9A == 0 && ACTIVE == 0 && !FLAGS[0] && !FLAGS[1] &&
          !D_800F2C40[0].field_E1F && !D_800F2C40[1].field_E1F, 15);
    return 0;
}

static s32 color_targets(void)
{
    s32 active, frame, busy, negative;
    for (active = 0; active < 2; active++)
    for (frame = 0; frame < 8; frame++)
    for (busy = 0; busy < 2; busy++)
    for (negative = 0; negative < 2; negative++) {
        u8 *rgb;
        setup(0, active);
        FLAGS[1] = 1;
        D_800F2C40[active].field_E1F = 1;
        D_800F2C40[active].field_E15 = frame;
        D_800F2C40[active].field_E0F = busy;
        rgb = D_800F2C40[active].field_DC0;
        rgb[0] = 0; rgb[1] = 255; rgb[2] = 8;
        if (negative) {
            random_values[0] = -256;
            random_values[1] = -6144;
            random_values[2] = -6400;
        }
        func_800507D0();
        CHECK(invariant() && random_count == (frame % 4 == 0 ? 3 : 0), 20);
        CHECK(rgb[0] == (frame % 4 == 0 ? 1 : 0) &&
              rgb[1] == (frame % 4 == 0 ? 254 : 255) &&
              rgb[2] == (negative && frame % 4 == 0 ? 7 : 8), 21);
        CHECK(D_800F2C40[active].field_E15 == frame + !busy &&
              !D_800F2C40[active ^ 1].field_E15, 22);
        CHECK(find(11) < 0 && (find(12) >= 0) == (!frame && !busy), 23);
    }
    setup(0, 0);
    FLAGS[1] = 1;
    D_800F2C40[0].field_E1F = 1;
    D_800F2C40[0].field_E15 = 255;
    func_800507D0();
    CHECK(invariant() && !random_count && !D_800F2C40[0].field_E15, 24);
    return 0;
}

static s32 animation_schedule(void)
{
    static const s32 frames[] = {0, 1, 29, 30, 59, 60, 89, 90, 119, 120,
                                 149, 150, 180, 210, 240, 255};
    static const s32 animations[] = {1, 2, 1, 7, 1};
    s32 active, f, other, flag;
    for (active = 0; active < 2; active++)
    for (f = 0; f < 16; f++)
    for (other = 0; other < 2; other++)
    for (flag = 0; flag < 2; flag++) {
        s32 frame = frames[f], anim, effect, special;
        setup(0, active);
        FLAGS[1] = flag;
        D_800F2C40[active].field_E1F = 1;
        D_800F2C40[active ^ 1].field_E1F = other;
        D_800F2C40[active].field_E15 = frame;
        func_800507D0();
        anim = find(12);
        effect = find(13);
        special = frame % 30 == 0 && frame >= 150;
        CHECK(invariant() && D_800F2C40[active].field_E15 == (u8)(frame + 1), 30);
        CHECK((anim >= 0) == (frame % 30 == 0 && frame < 150), 31);
        if (anim >= 0)
            CHECK(events[anim].a == active && events[anim].b == animations[frame / 30] &&
                  events[anim].c == 1, 32);
        CHECK((effect >= 0) == (special && !flag && other), 33);
        if (effect >= 0)
            CHECK(events[effect].a == 1 && events[effect].b == -1 &&
                  events[effect].c == (active ^ 1) && events[effect].d == 90, 34);
        CHECK(D_8009AF9A == (special ? flag ? 2 : other ? 1 : 0 : 0), 35);
        CHECK((find(11) >= 0) == !flag, 36);
    }
    return 0;
}

static s32 inactive_activation(void)
{
    s32 active, activate, flag;
    for (active = 0; active < 2; active++)
    for (activate = 0; activate < 2; activate++)
    for (flag = 0; flag < 2; flag++) {
        s32 helper, effect;
        setup(0, active);
        FLAGS[1] = flag;
        activate_action = activate;
        D_800F2C40[active].field_E15 = 255;
        func_800507D0();
        helper = find(11); effect = find(13);
        CHECK(invariant() && helper == 1 && events[helper].a == active, 40);
        CHECK(!random_count && find(12) < 0 && D_800F2C40[active].field_E15 == 255, 41);
        CHECK((effect >= 0) == activate && D_8009AF9A == 0, 42);
        if (effect >= 0)
            CHECK(events[effect].a == 1 && events[effect].b == -1 &&
                  events[effect].c == active && events[effect].d == 30, 43);
    }
    return 0;
}

static s32 crossfade(void)
{
    s32 active, gate, finish;
    for (active = 0; active < 2; active++)
    for (gate = 0; gate < 2; gate++)
    for (finish = 0; finish < 2; finish++) {
        u8 *a, *b;
        setup(1, active);
        D_800F2C40[0].field_E1F = D_800F2C40[1].field_E1F = 1;
        D_800F2C40[active].field_E15 = gate ? 4 : 3;
        D_800F2C40[active ^ 1].field_E15 = 255;
        a = D_800F2C40[active].field_DC0;
        b = D_800F2C40[active ^ 1].field_DC0;
        a[0] = 0; a[1] = 1; a[2] = finish ? 1 : 255;
        b[0] = 0; b[1] = 7; b[2] = 9;
        func_800507D0();
        CHECK(invariant() && count == 2 && !random_count, 50);
        CHECK(a[0] == 0 && a[1] == !gate &&
              a[2] == (finish ? 1 : 255) - gate, 51);
        CHECK(b[0] == gate && b[1] == 7 + gate && b[2] == 9, 52);
        CHECK(ACTIVE == (active ^ (gate && finish)) &&
              D_8009AF9A == !(gate && finish), 53);
        CHECK(D_800F2C40[active].field_E1F == !(gate && finish) &&
              D_800F2C40[active ^ 1].field_E1F == 1, 54);
        CHECK(D_800F2C40[active].field_E15 == (gate ? finish ? 4 : 5 : 4) &&
              D_800F2C40[active ^ 1].field_E15 == (gate && finish ? 0 : 255), 55);
    }
    return 0;
}

static s32 final_fade(void)
{
    s32 active, gate, busy, finish;
    for (active = 0; active < 2; active++)
    for (gate = 0; gate < 2; gate++)
    for (busy = 0; busy < 2; busy++)
    for (finish = 0; finish < 2; finish++) {
        u8 *a, *b;
        setup(2, active);
        D_800F2C40[0].field_E1F = D_800F2C40[1].field_E1F = 1;
        D_800F2C40[active].field_E15 = gate ? 252 : 255;
        D_800F2C40[active].field_E0F = busy;
        a = D_800F2C40[0].field_DC0; b = D_800F2C40[1].field_DC0;
        a[0] = 0; a[1] = a[2] = 1;
        b[0] = b[1] = 1; b[2] = finish ? 1 : 255;
        func_800507D0();
        CHECK(invariant() && count == 2 && !random_count && ACTIVE == active, 60);
        CHECK(a[0] == 0 && a[1] == !gate && a[2] == !gate &&
              b[0] == !gate && b[1] == !gate &&
              b[2] == (finish ? 1 : 255) - gate, 61);
        CHECK(D_8009AF9A == (gate && finish ? 3 : 2) &&
              D_800F2C40[0].field_E1F == !(gate && finish) &&
              D_800F2C40[1].field_E1F == !(gate && finish), 62);
        CHECK(D_800F2C40[active].field_E15 == (gate ? 253 : 0) &&
              D_800F2C40[active ^ 1].field_E15 == 0, 63);
    }
    return 0;
}

static s32 shutdown_and_defaults(void)
{
    static const s32 phases[] = {-128, -2, 3, 4, 127};
    s32 i;
    for (i = 0; i < 5; i++) {
        setup(phases[i], 1);
        D_800F2C40[0].field_E15 = 21;
        D_800F2C40[1].field_E15 = 25;
        func_800507D0();
        CHECK(invariant() && D_8009AF9A == (phases[i] == 3 ? -2 : phases[i]), 70);
        CHECK(count == (phases[i] == 3 ? 4 : 2) &&
              events[count - 1].kind == 16, 71);
        if (phases[i] == 3) CHECK(events[1].kind == 9 && events[2].kind == 10, 72);
        CHECK(D_800F2C40[0].field_E15 == 21 && D_800F2C40[1].field_E15 == 25 &&
              ACTIVE == 1, 73);
    }
    return 0;
}

static s32 ui_poll(void)
{
    static const s32 statuses[] = {0, 127, 128, 129, 255, -1, 0x10080, 0x18080, -65408};
    static const s32 flags[] = {0, 1, 255};
    s32 b0, b1, answer, s, active;
    for (b0 = 0; b0 < 3; b0++)
    for (b1 = 0; b1 < 3; b1++)
    for (answer = 0; answer < 2; answer++)
    for (s = 0; s < 9; s++)
    for (active = 0; active < 2; active++) {
        s32 change;
        setup(-2, 1);
        FLAGS[0] = flags[b0]; FLAGS[1] = flags[b1];
        poll_result = answer ? -1 : 0;
        status = statuses[s];
        D_800F2C40[0].field_E1F = active;
        func_800507D0();
        change = b0 && !b1 && answer && (s16)status != 128;
        CHECK(invariant() && D_8009AF9A == (change ? 2 : -2), 80);
        CHECK(poll_calls == (b0 != 0) &&
              status_calls == (!b0 || (answer && !b1)), 81);
        CHECK(FLAGS[0] == (b0 ? flags[b0] : ((status & 128) && active ? 1 : 0)) &&
              FLAGS[1] == (change ? 1 : flags[b1]), 82);
        CHECK(events[1].kind == (b0 ? 15 : 16), 83);
    }
    return 0;
}

static s32 opaque_reloads(void)
{
    s32 a;
    setup(-2, 0);
    prelude_phase = 0;
    activate_action = 1;
    func_800507D0();
    CHECK(invariant() && find(11) == 1 && find(13) == 2, 90);

    setup(0, 0);
    D_800F2C40[0].field_E1F = 1;
    D_800F2C40[0].field_E0F = 1;
    D_800F2C40[0].field_E15 = 1;
    D_800F2C40[1].field_E15 = 30;
    activate_action = 2;
    func_800507D0();
    a = find(12);
    CHECK(invariant() && ACTIVE == 1 && a >= 0 &&
          events[a].a == 1 && events[a].b == 2 &&
          D_800F2C40[0].field_E15 == 1 && D_800F2C40[1].field_E15 == 31, 91);

    setup(0, 0);
    activate_action = 3;
    func_800507D0();
    a = find(13);
    CHECK(invariant() && ACTIVE == 1 && a >= 0 && events[a].c == 1 &&
          events[a].d == 30 && !D_800F2C40[1].field_E15, 92);

    setup(0, 0);
    D_800F2C40[0].field_E1F = 1;
    D_800F2C40[0].field_E15 = 30;
    activate_action = 4;
    func_800507D0();
    CHECK(invariant() && find(12) < 0 && D_800F2C40[0].field_E15 == 30, 93);

    setup(0, 0);
    D_800F2C40[0].field_E1F = D_800F2C40[1].field_E1F = 1;
    D_800F2C40[0].field_E15 = 150;
    activate_action = 5;
    func_800507D0();
    CHECK(invariant() && D_8009AF9A == 2 && find(13) < 0, 94);

    setup(0, 0);
    FLAGS[1] = 1;
    D_800F2C40[0].field_E1F = 1;
    animation_action = 1;
    func_800507D0();
    CHECK(invariant() && D_800F2C40[0].field_E15 == 251, 95);

    setup(0, 0);
    FLAGS[1] = 1;
    D_800F2C40[0].field_E1F = 1;
    D_800F2C40[1].field_E15 = 255;
    animation_action = 2;
    func_800507D0();
    CHECK(invariant() && ACTIVE == 1 && !D_800F2C40[0].field_E15 &&
          !D_800F2C40[1].field_E15, 96);

    setup(0, 0);
    D_800F2C40[0].field_E1F = D_800F2C40[1].field_E1F = 1;
    D_800F2C40[0].field_E15 = 150;
    effect_action = 1;
    func_800507D0();
    CHECK(invariant() && D_8009AF9A == 11, 97);

    setup(0, 0);
    FLAGS[1] = 1;
    D_800F2C40[0].field_E1F = 1;
    D_800F2C40[1].field_E15 = 1;
    random_action = 1;
    func_800507D0();
    CHECK(invariant() && random_count == 3 && ACTIVE == 1 &&
          D_800F2C40[0].field_DC0[0] == 1 &&
          !D_800F2C40[1].field_DC0[0] &&
          D_800F2C40[1].field_E15 == 2 && find(12) < 0, 98);

    setup(-2, 0);
    FLAGS[0] = 1;
    poll_result = poll_action = 1;
    func_800507D0();
    CHECK(invariant() && !status_calls && D_8009AF9A == -2 && FLAGS[1] == 1, 99);

    setup(-2, 0);
    status = 128;
    status_action = 1;
    func_800507D0();
    CHECK(invariant() && FLAGS[0] == 1, 100);
    return 0;
}

int main(int argc, char **argv)
{
    volatile u32 interior_address = (u32)&D_8009AFA6;
    if (sizeof(void *) != 4 || sizeof(long) != 4 ||
        sizeof(ModelSlot) != 0xE20 || sizeof(D_8009AFA4) != 4 ||
        interior_address != (u32)&D_8009AFA4[2] || argc != 2) return 110;
    switch (argv[1][0]) {
    case '0': return initialization();
    case '1': return color_targets();
    case '2': return animation_schedule();
    case '3': return inactive_activation();
    case '4': return crossfade();
    case '5': return final_fade();
    case '6': return shutdown_and_defaults();
    case '7': return ui_poll();
    case '8': return opaque_reloads();
    }
    return 111;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86 and a host compiler with ILP32 execution",
)
class ModelSceneTransitionTests(unittest.TestCase):
    @staticmethod
    def source_text() -> str:
        text = SOURCE.read_text()
        if text.count(SOURCE_MARKER) != 1:
            raise AssertionError("model scene source boundary changed")
        return '#include "../types.h"\n' + SOURCE_MARKER[1:] + text.split(
            SOURCE_MARKER, 1
        )[1]

    @classmethod
    def setUpClass(cls):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="model-scene-test-", dir=REPOSITORY / "tmp")
        cls.addClassCleanup(temporary.cleanup)
        cls.directory = Path(temporary.name)
        cls.witness = cls.directory / "witness.c"
        cls.witness.write_text(WITNESS)
        cls.startup = cls.directory / "start.S"
        cls.startup.write_text(START)
        cls.source = cls.directory / "func_800507D0.c"
        cls.source.write_text(re.sub(
            r'^#include "([^"]+)"',
            lambda match: '#include "' + str((SOURCE.parent / match[1]).resolve()) + '"',
            cls.source_text(), flags=re.MULTILINE,
        ))
        cls.binaries = {
            optimization: cls.compile(cls.source, optimization, "actual")
            for optimization in ("-O0", "-O2")
        }

    @classmethod
    def compile(cls, source: Path, optimization: str, label: str) -> Path:
        binary = cls.directory / (label + optimization)
        result = subprocess.run(
            ["cc", "-m32", optimization, "-std=gnu99", "-ffreestanding",
             "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
             "-fno-stack-protector", "-nostdlib", f"-I{REPOSITORY}",
             "-Wl,--defsym,D_8009AFA6=D_8009AFA4+2",
             str(source), str(cls.witness), str(cls.startup), "-o", str(binary)],
            cwd=REPOSITORY, env={**os.environ, "TMPDIR": str(cls.directory)},
            capture_output=True, text=True, timeout=60,
        )
        if result.returncode:
            raise AssertionError(result.stderr)
        return binary

    def run_witness(self, scenario: int):
        for optimization, binary in self.binaries.items():
            with self.subTest(optimization=optimization):
                result = subprocess.run(
                    [str(binary), str(scenario)], cwd=REPOSITORY,
                    capture_output=True, text=True, timeout=10,
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_initialization_arguments_order_and_byte_preservation(self):
        self.run_witness(0)

    def test_random_color_targets_gating_busy_and_counter_wrap(self):
        self.run_witness(1)

    def test_animation_quotients_phase_changes_and_other_slot(self):
        self.run_witness(2)

    def test_inactive_slot_activation_without_frame_increment(self):
        self.run_witness(3)

    def test_crossfade_bounds_and_new_active_frame_increment(self):
        self.run_witness(4)

    def test_final_fade_completion_and_busy_frame_increment(self):
        self.run_witness(5)

    def test_shutdown_and_signed_default_phases(self):
        self.run_witness(6)

    def test_common_ui_poll_short_circuit_low16_status_and_flags(self):
        self.run_witness(7)

    def test_opaque_helpers_preserve_required_reloads(self):
        self.run_witness(8)

    def mutation(self, pattern: str, replacement: str, scenario: int, expected: range):
        text = self.source_text()
        # Protect each semantic edit with exactly one matching expression.
        # Captures avoid depending on the decompiler's temporary names.
        text, count = re.subn(pattern, replacement, text)
        self.assertEqual(count, 1, "mutation no longer identifies one semantic operation")
        text = re.sub(
            r'^#include "([^"]+)"',
            lambda match: '#include "' + str((SOURCE.parent / match[1]).resolve()) + '"',
            text, flags=re.MULTILINE,
        )
        source = self.directory / f"mutant-{scenario}.c"
        source.write_text(text)
        binary = self.compile(source, "-O2", f"mutant-{scenario}")
        result = subprocess.run(
            [str(binary), str(scenario)], cwd=REPOSITORY,
            capture_output=True, text=True, timeout=10,
        )
        self.assertIn(result.returncode, expected, result.stderr)

    def test_wrong_animation_is_detected(self):
        self.mutation(r"(case 1u:\s*\w+\s*=\s*)2;", r"\g<1>7;", 2, range(30, 37))

    def test_missing_active_flip_is_detected(self):
        self.mutation(r"\^=\s*1u;", "^= 0u;", 4, range(50, 56))

    def test_wrong_shared_frame_increment_is_detected(self):
        self.mutation(
            r"\+\+([^;\n]*->field_E15);", r"\1 += 2;", 5, range(60, 64)
        )


if __name__ == "__main__":
    unittest.main()
