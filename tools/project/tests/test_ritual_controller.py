"""ILP32 ritual-controller behavior, with opaque helpers and mutation controls.

Expected events and numeric boundaries come from the retail control flow, not
an executable copy of the implementation. SDK helpers are witnesses, not GPU,
audio, or display-object emulators. Only the freestanding startup is assembly.
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
SOURCE = REPOSITORY / "src/game/func_800262D4.c"
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

# The source's include prefix supplies the same owner views to the fixture.
WITNESS = r"""
void DuelEffect_ApplyRitual(void);
#define R8(p, n) (((u8 *)(p))[n])
#define R16(p, n) (*(u16 *)((u8 *)(p) + (n)))
#define RS16(p, n) (*(s16 *)((u8 *)(p) + (n)))
#define R32(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define CHECK(c, n) do { if (!(c)) return (n); } while (0)

DisplayObject *D_8009B1C0;
s16 D_8009B1A0, gDuel_wEffectCardID, D_8009B20C[2];
u16 D_8009B210, gDuel_wCardEffectFlags;
u8 *D_8009B17C;
u8 D_8009B19C, D_8009B1D5;
s8 D_8009B360[2];
/* The result and its trailing cleared word share the full owning allocation. */
DisplayObjectRitualWorkArea D_800E9EF0;
u16 D_800EA128[22];
u8 D_8018C2D8[1408 * 256];
DuelCardRecord D_801A7AD8[DUEL_CARD_RECORD_COUNT];
DuelCardStagingDeckView D_8015C424_cards;
DuelEffectChannel D_800EB0F8[DUEL_EFFECT_CHANNEL_COUNT];
s16 gDuel_wSelectedCardID;
u16 gInput_wPad1Pressed;
s8 gDialog_bChoice;

static DisplayObject main_object, clone_objects[2], tributes[3], card_object, choice_object;
static u8 request[32], card_data[8];
typedef struct { s32 kind, a, b, c, d; } Event;
static Event events[64];
static s32 count, error, initialized, ritual_result, clone_count, busy;
static s32 random_value, random_calls, choice_result, pumps, ready_after;
static s32 mutation, executions;
static RECT saved_rect;
static u32 *saved_readback;

static void clear(void *pointer, u32 size)
{
    volatile u8 *p = pointer;
    while (size--) *p++ = 0;
}

static void event(s32 kind, s32 a, s32 b, s32 c, s32 d)
{
    if (count >= 64) { error = 1; return; }
    events[count].kind = kind;
    events[count].a = a; events[count].b = b;
    events[count].c = c; events[count].d = d;
    count++;
}

static s32 object_id(void *p)
{
    if (p == &main_object) return 0;
    if (p == &clone_objects[0]) return 1;
    if (p == &clone_objects[1]) return 2;
    if (p == &card_object) return 3;
    if (p == &choice_object) return 4;
    error = 2;
    return -1;
}

s32 DuelEffect_MarkInitialized(void)
{
    event(1, initialized, 0, 0, 0);
    return initialized;
}

s32 Duel_CheckRitual(DuelRitualResult *out, s32 id)
{
    s32 i;
    event(2, id, 0, 0, 0);
    if (out != &D_800E9EF0.ritual.result) error = 3;
    for (i = 0; i < 3; i++) out->tribute_objects[i] = &tributes[i];
    out->field_0C = 0;
    return ritual_result;
}

void func_80019CC8(void *id) { event(3, (s32)id, 0, 0, 0); }
u8 *DuelEffect_AllocateRequest(s32 id) { event(4, id, 0, 0, 0); return request; }
void func_8003FF88(u32 id) { event(5, id, 0, 0, 0); }
void func_80024914(DuelCardRecord *card)
{
    event(6, card - D_801A7AD8, 0, 0, 0);
}
u8 *func_800291E0(s32 id, s32 x, s32 y)
{
    event(7, id, x, y, 0);
    return (u8 *)&main_object;
}
void DisplayObject_ResetVelocity(DisplayObjectVelocity *p)
{
    event(8, object_id(p), 0, 0, 0);
    if (R16(p, 0x30) != 90 || RS16(p, 0x32) != -34 ||
        R16(p, 0x60) != 24 || R16(p, 0x44) || R16(p, 0x46) ||
        R32(p, 4) != 0xF7FFFFFF || R16(p, 8) != 0xA5A5) error = 4;
}
void DisplayObject_StepPositionY(DisplayObjectVelocity *p)
{
    event(9, object_id(p), RS16(p, 0x38), 0, 0);
    if (mutation == 1) { R16(p, 0x44) = 1000; R16(p, 0x60) = 1; }
}
void SD_SEPlayFull(u32 id) { event(10, id, 0, 0, 0); }
void func_8001944C(DisplayObject *p) { event(11, object_id(p), 0, 0, 0); }
DisplayObject *Duel_CreateCardEffectOverlay(DisplayObjectConfigView *p)
{
    event(12, object_id(p), clone_count, 0, 0);
    if (clone_count >= 2) { error = 5; return &clone_objects[0]; }
    return &clone_objects[clone_count++];
}
s32 DisplayObject_SetDepthOffset(DisplayObject *p, s8 order)
{
    event(13, object_id(p), order, 0, 0);
    return 0;
}
void func_80029528(s32 id) { event(14, id, 0, 0, 0); }
void DisplayObject_ReleaseIfPresent(void *p) { event(15, object_id(p), 0, 0, 0); }
int StoreImage(RECT *rect, u32 *data)
{
    event(16, 0, 0, 0, 0);
    saved_rect = *rect;
    saved_readback = data;
    if (rect != (RECT *)&D_800EA128[4]) error = 6;
    return 0;
}
void func_80024D34(s32 index, s32 id)
{
    event(17, index, id, 0, 0);
    if (mutation == 2) D_801A7AD8[index].object = &main_object;
}
u8 *func_80017F04(DuelCardRecord *p, s32 x, s32 y)
{
    event(18, p - D_801A7AD8, x, y, 0);
    return (u8 *)&choice_object;
}
void func_8001EC70(u8 *p) { event(26, object_id(p), 0, 0, 0); }
void *DisplayObject_FindAllocatedByTag(s32 id)
{
    event(19, id, 0, 0, 0);
    return busy ? &main_object : 0;
}
void *TextBox_CreateFlagged(s32 index, s32 text, s32 x, s32 y,
                           s32 width, s32 height, s32 flags)
{
    event(20, index, text, x, y);
    if (width != 176 || height != 48 || flags != 32) error = 7;
    return D_800EB0F8;
}
void func_80039794(void)
{
    event(21, pumps, 0, 0, 0);
    if (R8(D_800EB0F8, 0x5A) != 8 || R8(D_800EB0F8, 0x5B) != 16) error = 8;
    if (++pumps >= ready_after) D_800EB0F8[0].field_30 = &choice_object;
    if (pumps > 4) error = 9;
}
s32 Dialog_ReadChoiceInput(DuelEffectChannel *p)
{
    event(22, p == D_800EB0F8, 0, 0, 0);
    if (mutation == 3) { gInput_wPad1Pressed = 0x80; gDialog_bChoice = -1; }
    return choice_result;
}
void TextBox_Destroy(void *p)
{
    event(23, p == D_800EB0F8, 0, 0, 0);
    if (mutation == 4) D_8009B210 = 0x1C4;
}
void DisplayObject_SavePosition(DisplayObjectSnapshot *p)
{
    event(24, object_id(p), 0, 0, 0);
}
void Widget_SlideSine(DisplayObjectPosition *p, int x, int y, int phase)
{
    event(25, object_id(p), x, y, phase);
    if (mutation == 5) R16(p, 0x60) = 0xFFF0;
}
int rand(void)
{
    event(27, 0, 0, 0, 0);
    random_calls++;
    return random_value;
}

static void setup(s32 state)
{
    s32 i;
    clear(&main_object, sizeof(main_object));
    clear(clone_objects, sizeof(clone_objects));
    clear(tributes, sizeof(tributes));
    clear(&card_object, sizeof(card_object));
    clear(&choice_object, sizeof(choice_object));
    clear(request, sizeof(request)); clear(card_data, sizeof(card_data));
    clear(&D_800E9EF0, sizeof(D_800E9EF0));
    clear(D_801A7AD8, sizeof(D_801A7AD8));
    clear(&D_8015C424_cards, sizeof(D_8015C424_cards));
    clear(D_800EB0F8, sizeof(D_800EB0F8));
    clear(events, sizeof(events));
    for (i = 0; i < 22; i++) D_800EA128[i] = 0x5555;
    R8(&tributes[0], 0x6A) = 2;
    R8(&tributes[1], 0x6A) = 7;
    R8(&tributes[2], 0x6A) = 29;
    D_800E9EF0.slots[0] = &card_object; D_800E9EF0.slots[1] = &choice_object;
    for (i = 0; i < 3; i++) D_800E9EF0.slots[i + 2] = &tributes[i];
    D_800E9EF0.ritual.result.field_0C = 0x11223344;
    D_8009B17C = request; D_8009B1C0 = &main_object;
    D_8009B1A0 = 321; gDuel_wEffectCardID = -123;
    D_8009B210 = state; gDuel_wCardEffectFlags = 0xA55A;
    D_8009B20C[0] = 0x1234; D_8009B20C[1] = 0;
    D_8009B19C = 7; D_8009B1D5 = 0;
    D_8009B360[0] = D_8009B360[1] = -1;
    D_801A7AD8[7].object = &card_object;
    R32(&D_801A7AD8[7], 4) = (u32)card_data;
    D_801A7AD8[7].flags = 0xA7A5;
    R8(&choice_object, 0x6A) = 29;
    D_801A7AD8[29].flags = 0xB7A5;
    D_8015C424_cards.field_cards[29].card_id = 0x9234;
    card_data[2] = 0xFD; card_data[3] = 1;
    R32(&main_object, 4) = 0xFFFFFFFF; R16(&main_object, 8) = 0xA5A1;
    R32(&clone_objects[0], 4) = R32(&clone_objects[1], 4) = 0x08000003;
    R16(&main_object, 0x30) = 90;
    R16(&card_object, 0x30) = 0xFF85;
    gDuel_wSelectedCardID = -1;
    gInput_wPad1Pressed = 0; gDialog_bChoice = 0;
    initialized = 1; ritual_result = 123;
    clone_count = busy = count = error = random_calls = choice_result = pumps = mutation = 0;
    random_value = 0; ready_after = 3;
    saved_readback = 0;
}

static void run(void) { executions++; DuelEffect_ApplyRitual(); }

static s32 valid(void)
{
    return !error && count > 0 && events[0].kind == 1 &&
           D_8009B20C[0] == 0x1234;
}

static s32 initialization(void)
{
    static const s32 results[] = {0, 123, -32767, 65536};
    static const s32 arguments[] = {-32768, 32767};
    s32 i, j;
    for (i = 0; i < 4; i++) for (j = 0; j < 2; j++) {
        setup(0xF5); initialized = 0;
        ritual_result = results[i]; gDuel_wEffectCardID = arguments[j];
        run();
        CHECK(valid() && events[1].kind == 2 && events[1].a == arguments[j], 10);
        CHECK(D_8009B1A0 == (s16)results[i] && D_800E9EF0.ritual.result.field_0C == 0 &&
              D_800E9EF0.slots[2] == &tributes[0] && D_800E9EF0.slots[3] == &tributes[1] &&
              D_800E9EF0.slots[4] == &tributes[2], 11);
        if (i == 0 || i == 3) {
            CHECK(count == 2 && gDuel_wCardEffectFlags == 0 && D_8009B210 == 0xF5, 12);
        } else {
            CHECK(count == 5 && events[2].kind == 3 && events[2].a == results[i] &&
                  events[3].kind == 4 && events[3].a == 22 &&
                  events[4].kind == 5 && events[4].a == 0x8022, 13);
            CHECK(D_8009B17C == request && D_8009B210 == 0 &&
                  RS16(request, 0x1A) == arguments[j] && gDuel_wCardEffectFlags == 0xA55A, 14);
        }
    }
    return 0;
}

static s32 request_ready(void)
{
    static const s32 ready[] = {0, 1, 255};
    s32 i;
    for (i = 0; i < 3; i++) {
        setup(0xA0); R8(request, 0x1D) = ready[i]; D_8009B19C = 3;
        run();
        CHECK(valid() && count == (i ? 4 : 1) && gDuel_wCardEffectFlags == 0xA55A, 20);
        CHECK(D_8009B210 == (i ? 1 : 0xA0) && D_8009B19C == (i ? 7 : 3), 21);
        if (i) CHECK(events[1].kind == 6 && events[1].a == 2 &&
                     events[2].kind == 6 && events[2].a == 7 &&
                     events[3].kind == 6 && events[3].a == 29, 22);
    }
    return 0;
}

static s32 scale_countdown(void)
{
    static const u16 counters[] = {0, 1, 2, 0x7FFF, 0x8000, 0x8001, 0xFFFF, 24};
    static const s32 completed[] = {1, 1, 0, 0, 0, 1, 1, 0};
    s32 i;
    setup(1); run();
    CHECK(valid() && count == 5 && events[1].kind == 7 &&
          events[1].a == 1 && events[1].b == -1 && events[1].c == -1 &&
          events[2].kind == 8 && events[3].kind == 10 && events[3].a == 38 &&
          events[4].kind == 9 && events[4].b == 597, 30);
    CHECK(D_8009B210 == 0x81 && D_8009B1C0 == &main_object &&
          R16(&main_object, 0x44) == 170 && R16(&main_object, 0x46) == 170 &&
          R16(&main_object, 0x60) == 23, 31);
    for (i = 0; i < 8; i++) {
        setup(0x81); R16(&main_object, 0x60) = counters[i];
        R16(&main_object, 0x44) = 65530;
        run();
        CHECK(valid() && count == 2 && events[1].kind == 9, 32);
        CHECK(R16(&main_object, 0x60) == (u16)(counters[i] - 1) &&
              R16(&main_object, 0x44) == (completed[i] ? 4096 : 164) &&
              R16(&main_object, 0x46) == (completed[i] ? 4096 : 164), 33);
        CHECK(D_8009B210 == (completed[i] ? 2 : 0x81), 34);
        if (completed[i]) CHECK(R16(&main_object, 0x32) == 22 &&
                               R32(&main_object, 4) == 0xFFFFFFFF &&
                               R16(&main_object, 8) == 0xA5A1, 35);
    }
    return 0;
}

static s32 clone_creation(void)
{
    static const u16 counters[] = {0, 1, 2, 0x7FFF, 0x8000, 0x8001, 0xFFFF, 32};
    static const s32 spawn[] = {1, 1, 0, 0, 0, 1, 1, 0};
    s32 i;
    setup(2); run();
    CHECK(valid() && count == 1 && D_8009B210 == 0x82 && D_8009B20C[1] == 31, 40);
    for (i = 0; i < 8; i++) {
        setup(0x82); D_8009B20C[1] = counters[i]; run();
        CHECK(valid() && D_8009B210 == (spawn[i] ? 0xC2 : 0x82) &&
              (u16)D_8009B20C[1] == (u16)(counters[i] - 1), 41);
        if (spawn[i]) {
            CHECK(count == 6 && events[1].kind == 11 && events[1].a == 0 &&
                  events[2].kind == 12 && events[2].b == 0 &&
                  events[3].kind == 12 && events[3].b == 1 &&
                  events[4].kind == 13 && events[4].a == 2 && events[4].b == -1 &&
                  events[5].kind == 14 && events[5].a == 1, 42);
            CHECK(D_800E9EF0.slots[0] == &clone_objects[0] &&
                  D_800E9EF0.slots[1] == &clone_objects[1] &&
                  R32(&clone_objects[0], 4) == 0x50000003 &&
                  R32(&clone_objects[1], 4) == 0x60000003, 43);
        } else CHECK(count == 1 && clone_count == 0, 44);
    }
    return 0;
}

static s32 clone_fade(void)
{
    static const u32 colors[] = {0, 1, 3, 4, 5, 255};
    static const u32 expected[] = {0, 0, 0, 0, 0x010101, 0xFBFBFB};
    static const u16 scales[] = {4096, 0x7FFF, 0x8000, 0xFFFF};
    static const u16 scaled[] = {4224, 0x807F, 0x8080, 127};
    s32 i, j;
    for (i = 0; i < 6; i++) for (j = 0; j < 4; j++) {
        setup(0xC2);
        D_800E9EF0.slots[0] = &clone_objects[0]; D_800E9EF0.slots[1] = &clone_objects[1];
        R32(&clone_objects[0], 0xC) = 0xAA998800 | colors[i];
        R32(&clone_objects[1], 0xC) = 0x12345678;
        R16(&clone_objects[0], 0x44) = scales[j];
        run();
        CHECK(valid() && R16(&clone_objects[0], 0x44) == scaled[j] &&
              R16(&clone_objects[0], 0x46) == scaled[j] &&
              R16(&clone_objects[1], 0x44) == scaled[j] &&
              R16(&clone_objects[1], 0x46) == scaled[j], 50);
        CHECK(R32(&clone_objects[0], 0xC) == expected[i] &&
              R32(&clone_objects[1], 0xC) == expected[i], 51);
        CHECK(D_8009B210 == (i < 4 ? 3 : 0xC2) && count == (i < 4 ? 3 : 1), 52);
        if (i < 4) CHECK(events[1].kind == 15 && events[1].a == 1 &&
                         events[2].kind == 15 && events[2].a == 2, 53);
    }
    return 0;
}

static s32 readback(void)
{
    static const s32 pages[] = {0, 1, 255};
    s32 i, wrap;
    for (i = 0; i < 3; i++) for (wrap = 0; wrap < 2; wrap++) {
        setup(3); D_8009B1A0 = -32767; card_data[3] = pages[i];
        D_800EA128[20] = wrap ? 65520 : 100; D_800EA128[21] = 0xFFED;
        run();
        CHECK(valid() && count == 2 && events[1].kind == 16 && D_8009B210 == 0x83, 60);
        CHECK(R16(card_data, 0) == 0x8001 && (u16)D_801A7AD8[7].card_id == 0x8001 &&
              card_data[2] == 0xFD && card_data[3] == pages[i], 61);
        CHECK((u16)saved_rect.x == (wrap ? 40 : 156) && saved_rect.y == -19 &&
              saved_rect.w == 8 && saved_rect.h == 88 &&
              saved_readback == (u32 *)(D_8018C2D8 + 1408 * pages[i]), 62);
        CHECK(D_800EA128[3] == 0x5555 && D_800EA128[8] == 0x5555 &&
              D_800EA128[20] == (wrap ? 65520 : 100) && D_800EA128[21] == 0xFFED, 63);
    }
    return 0;
}

static s32 orientation(void)
{
    static const s8 identities[] = {-128, -1, 0, 127};
    s32 side, identity, random;
    for (side = 0; side < 2; side++)
    for (identity = 0; identity < 4; identity++)
    for (random = 0; random < 2; random++) {
        setup(0x83); D_8009B1D5 = side;
        D_8009B360[side] = identities[identity]; D_8009B360[side ^ 1] = ~identities[identity];
        random_value = random ? -1 : 2;
        run();
        CHECK(valid() && events[1].kind == 17 && events[1].a == 7 &&
              events[1].b == -3 && D_800E9EF0.slots[0] == &card_object &&
              RS16(&card_object, 0x32) == -240, 70);
        if (identity < 2) {
            CHECK(count == 3 && events[2].kind == 18 && events[2].a == 7 &&
                  events[2].b == 134 && events[2].c == 240 &&
                  D_8009B210 == 0x84 && !random_calls, 71);
            CHECK(R16(&choice_object, 0x28) == 134 && R16(&choice_object, 0x2A) == 42 &&
                  R16(&choice_object, 0x2C) == 16 && R8(&choice_object, 0x6C) == 1 &&
                  choice_object.update == (DisplayObjectCallback)func_8001EC70 &&
                  D_801A7AD8[7].flags == 0xA7A5, 72);
        } else {
            CHECK(count == 3 && events[2].kind == 27 && random_calls == 1 &&
                  D_8009B210 == 5 && D_801A7AD8[7].flags == (random ? 0xA7A5 : 0xA5A5), 73);
        }
    }
    return 0;
}

static s32 text_readiness(void)
{
    s32 is_busy, ready;
    setup(4); run();
    CHECK(valid() && count == 1 && D_8009B210 == 0x84 &&
          R16(&choice_object, 0x28) == 134 && R16(&choice_object, 0x2A) == 42 &&
          R16(&choice_object, 0x2C) == 16 && R8(&choice_object, 0x6C) == 1 &&
          choice_object.update == (DisplayObjectCallback)func_8001EC70, 80);
    for (is_busy = 0; is_busy < 2; is_busy++) for (ready = 0; ready < 2; ready++) {
        setup(0x84); busy = is_busy;
        if (ready) D_800EB0F8[0].field_30 = &choice_object;
        run();
        CHECK(valid() && events[1].kind == 19 && events[1].a == 1, 81);
        if (is_busy) CHECK(count == 2 && gDuel_wSelectedCardID == -1 &&
                           D_8009B210 == 0x84 && pumps == 0, 82);
        else {
            CHECK((u16)gDuel_wSelectedCardID == 0x9234 && D_8009B210 == 0xC4 &&
                  pumps == (ready ? 1 : 3) && count == (ready ? 4 : 6), 83);
            CHECK(events[2].kind == 20 && events[2].a == 0 && events[2].b == 33 &&
                  events[2].c == 72 && events[2].d == 110 &&
                  R8(D_800EB0F8, 0x5A) == 8 && R8(D_800EB0F8, 0x5B) == 16, 84);
        }
    }
    return 0;
}

static s32 choice_and_destruction(void)
{
    static const u16 keys[] = {0, 0x20, 0x40, 0x80, 0xC0};
    static const s8 choices[] = {0, 1, -128};
    s32 blocked, key, choice, b;
    for (blocked = 0; blocked < 2; blocked++)
    for (key = 0; key < 5; key++)
    for (choice = 0; choice < 3; choice++) {
        s32 accepted = !blocked && key >= 2;
        setup(0xC4); choice_result = blocked ? -1 : 0;
        gInput_wPad1Pressed = keys[key]; gDialog_bChoice = choices[choice];
        run();
        CHECK(valid() && events[1].kind == 22 && events[1].a == 1 &&
              count == (accepted ? 4 : 2) && D_8009B210 == (accepted ? 0xE4 : 0xC4), 90);
        if (accepted) {
            CHECK(events[2].kind == 10 && events[2].a == 7 &&
                  events[3].kind == 23 && events[3].a == 1, 91);
            CHECK(R16(&choice_object, 0x28) == 134 &&
                  RS16(&choice_object, 0x2A) == -128 && R16(&choice_object, 0x2C) == 16 &&
                  R8(&choice_object, 0x6C) == 1 &&
                  choice_object.update == (DisplayObjectCallback)func_8001EC70, 92);
            CHECK(D_801A7AD8[29].flags == (choice ? 0xB7A5 : 0xB5A5) &&
                  D_801A7AD8[7].flags == 0xA7A5, 93);
        } else CHECK(D_801A7AD8[29].flags == 0xB7A5 && choice_object.update == 0, 94);
    }
    for (b = 0; b < 2; b++) {
        setup(0xE4); busy = b; run();
        CHECK(valid() && events[1].kind == 19 && count == (b ? 2 : 3) &&
              D_8009B210 == (b ? 0xE4 : 5), 95);
        if (!b) CHECK(events[2].kind == 15 && events[2].a == 4, 96);
    }
    return 0;
}

static s32 slide_and_default(void)
{
    static const u16 phases[] = {0xFC00, 0xFFD5, 0xFFD6, 0xFFFF, 0, 0x7FFF, 0x8000, 0x7FD6};
    static const s16 next[] = {-982, -1, 0, 41, 42, -32727, -32726, -32768};
    s32 i;
    setup(5); run();
    CHECK(valid() && count == 3 && events[1].kind == 24 && events[1].a == 3 &&
          events[2].kind == 25 && events[2].a == 3 &&
          events[2].b == -123 && events[2].c == -24 && events[2].d == -1024 &&
          RS16(&card_object, 0x60) == -982 && D_8009B210 == 0x85 &&
          gDuel_wCardEffectFlags == 0xA55A, 100);
    for (i = 0; i < 8; i++) {
        setup(0x85); R16(&card_object, 0x60) = phases[i]; run();
        CHECK(valid() && count == 2 && events[1].kind == 25 &&
              events[1].b == -123 && events[1].c == -24 &&
              events[1].d == (s16)phases[i] &&
              RS16(&card_object, 0x60) == next[i], 101);
        CHECK(gDuel_wCardEffectFlags == (next[i] >= 0 ? 0 : 0xA55A) &&
              RS16(&card_object, 0x32) == (next[i] >= 0 ? -24 : 0), 102);
    }
    for (i = 6; i < 16; i++) {
        setup(0xA0 | i); run();
        CHECK(valid() && count == 1 && D_8009B210 == (0xA0 | i) &&
              gDuel_wCardEffectFlags == 0xA55A, 103);
    }
    return 0;
}

static s32 helper_reloads(void)
{
    setup(0x81); mutation = 1; run();
    CHECK(valid() && D_8009B210 == 2 && R16(&main_object, 0x60) == 0 &&
          R16(&main_object, 0x44) == 4096, 110);
    setup(0x83); mutation = 2; D_8009B360[0] = 0; run();
    CHECK(valid() && D_800E9EF0.slots[0] == &main_object &&
          RS16(&main_object, 0x32) == -240 && RS16(&card_object, 0x32) == 0, 111);
    setup(0xC4); mutation = 3; run();
    CHECK(valid() && D_8009B210 == 0xE4 && D_801A7AD8[29].flags == 0xB7A5, 112);
    setup(0xC4); mutation = 4; gInput_wPad1Pressed = 0x40; run();
    CHECK(valid() && D_8009B210 == 0x1E4, 113);
    setup(0x85); mutation = 5; R16(&card_object, 0x60) = 0x8000; run();
    CHECK(valid() && R16(&card_object, 0x60) == 26 && gDuel_wCardEffectFlags == 0, 114);
    return 0;
}

int main(int argc, char **argv)
{
    static const s32 expected_cases[] = {8, 3, 9, 9, 24, 6, 16, 5, 32, 19, 5};
    s32 result, scenario;
    if (sizeof(void *) != 4 || sizeof(long) != 4 ||
        sizeof(DuelRitualResult) != 16 || sizeof(D_800E9EF0) < 24 || argc != 2) return 120;
    scenario = argv[1][0] - '0';
    if (argv[1][1]) scenario = scenario * 10 + argv[1][1] - '0';
    switch (scenario) {
    case 0: result = initialization(); break;
    case 1: result = request_ready(); break;
    case 2: result = scale_countdown(); break;
    case 3: result = clone_creation(); break;
    case 4: result = clone_fade(); break;
    case 5: result = readback(); break;
    case 6: result = orientation(); break;
    case 7: result = text_readiness(); break;
    case 8: result = choice_and_destruction(); break;
    case 9: result = slide_and_default(); break;
    case 10: result = helper_reloads(); break;
    default: return 121;
    }
    if (result) return result;
    return executions == expected_cases[scenario] ? 0 : 122;
}
"""


def include_prefix(path: Path) -> str:
    text = path.read_text().split("void DuelEffect_ApplyRitual(void)", 1)[0]
    return re.sub(
        r'^#include "([^"]+)"',
        lambda match: '#include "' + str((path.parent / match[1]).resolve()) + '"',
        text, flags=re.MULTILINE,
    )


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86 and a host compiler with ILP32 execution",
)
class RitualControllerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="ritual-test-", dir=REPOSITORY / "tmp")
        cls.addClassCleanup(temporary.cleanup)
        cls.directory = Path(temporary.name)
        cls.witness = cls.directory / "witness.c"
        cls.witness.write_text(include_prefix(SOURCE) + WITNESS)
        cls.startup = cls.directory / "start.S"
        cls.startup.write_text(START)
        cls.binaries = {
            optimization: cls.compile(SOURCE, optimization, "actual")
            for optimization in ("-O0", "-O2")
        }

    @classmethod
    def compile(cls, source: Path, optimization: str, label: str) -> Path:
        binary = cls.directory / (label + optimization)
        result = subprocess.run(
            ["cc", "-m32", optimization, "-std=gnu99", "-ffreestanding",
             "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
             "-fno-stack-protector", "-nostdlib", f"-I{REPOSITORY}",
             str(source), str(cls.witness), str(cls.startup), "-o", str(binary)],
            cwd=REPOSITORY, env={**os.environ, "TMPDIR": str(cls.directory)},
            capture_output=True, text=True, timeout=60,
        )
        if result.returncode:
            raise AssertionError(result.stderr)
        return binary

    def witness_scenario(self, scenario: int):
        for optimization, binary in self.binaries.items():
            with self.subTest(optimization=optimization):
                result = subprocess.run(
                    [str(binary), str(scenario)], cwd=REPOSITORY,
                    capture_output=True, text=True, timeout=10,
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_initialization_failure_success_signed_argument_and_result(self):
        self.witness_scenario(0)

    def test_request_ready_and_three_tribute_record_indices(self):
        self.witness_scenario(1)

    def test_object_creation_scale_wrap_and_signed_countdown(self):
        self.witness_scenario(2)

    def test_clone_creation_delay_signed_boundary_and_attributes(self):
        self.witness_scenario(3)

    def test_clone_scale_color_clamp_and_ordered_destruction(self):
        self.witness_scenario(4)

    def test_ritual_card_readback_rectangle_and_page_stride(self):
        self.witness_scenario(5)

    def test_side_identity_random_orientation_and_state4_fallthrough(self):
        self.witness_scenario(6)

    def test_state4_callback_selected_card_and_text_readiness(self):
        self.witness_scenario(7)

    def test_choice_input_flags_orientation_and_callback_destruction(self):
        self.witness_scenario(8)

    def test_sliding_signed_wrap_completion_and_unknown_states(self):
        self.witness_scenario(9)

    def test_opaque_helpers_require_state_object_and_input_reloads(self):
        self.witness_scenario(10)

    def mutant(self, old: str, new: str, scenario: int, expected: range):
        text = SOURCE.read_text()
        self.assertEqual(text.count(old), 1, "semantic mutation must identify one operation")
        text = text.replace(old, new)
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

    def test_wrong_readback_stride_is_detected(self):
        self.mutant("1408 *", "1404 *", 5, range(60, 64))

    def test_wrong_tribute_slot_is_detected(self):
        self.mutant("&D_801A7AD8[D_800E9EF0.slots[4]->field_6A]",
                    "&D_801A7AD8[D_800E9EF0.slots[3]->field_6A]", 1, range(20, 23))

    def test_wrong_slide_increment_is_detected(self):
        self.mutant("(u16)object->field_60 + 42", "(u16)object->field_60 + 41", 9,
                    range(100, 104))


if __name__ == "__main__":
    unittest.main()
