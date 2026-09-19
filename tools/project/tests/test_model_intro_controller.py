"""Exercise the retail intro-controller contract independently of MIPS matching."""

from __future__ import annotations

from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/model_intro_controller.c"
# The unit opens with the mode 15 controller; the intro contract starts at the
# intro controller's own comment and keeps its own include block.
SOURCE_START = "/*\n * Scene mode 19's intro controller."
SOURCE_MARKER = (
    '\n#include "../unmatched.h"\n'
    '#include "../psyq/libgte.h"\n'
    '#include "../psyq/libgpu.h"\n'
)
SCRATCH = REPOSITORY / "tmp/astra/model-update-witness"
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

# Numeric field offsets and the state table below come from the retail
# instruction contract. Game C is compiled separately, never included here.
WITNESS = r"""
#include "src/types.h"
#include "src/psyq/libgte.h"
#include "src/game/model.h"
#include "src/game/model_graphics_state.h"
#include "src/game/model_slot_properties.h"
#include "src/game/model_slot_state_updates.h"
#include "src/game/model_state_setters.h"
#include "src/game/model_transfer_state.h"
#include "src/game/model_effect_state.h"
#include "src/game/func_80058E1C.h"
#include "src/game/model_control_slot_animation.h"
#include "src/game/func_8005D994.h"
#include "src/game/file_transfer.h"
#include "src/game/high_memory_addresses.h"
#include "src/game/fade.h"
#include "src/game/sound.h"

typedef char ilp32[sizeof(void *) == 4 && sizeof(int) == 4 ? 1 : -1];
typedef char vector_size[sizeof(SVECTOR) == 8 ? 1 : -1];
typedef char slot_size[sizeof(ModelSlot) == 0xE20 ? 1 : -1];
enum {
    GUARD = 32, SLOT_SIZE = 0xE20, SLOTS_SIZE = 3 * SLOT_SIZE + 64,
    GLOBALS_SIZE = 100, STATE = 32, LATCH = 33, FLAGS = 36, SECONDARY = 40,
    FIRST = 44, SECOND = 48, DATA1 = 52, DATA2 = 56, OFFSET = 60,
    INITIAL = 1, OLD_FIRST, OLD_SECOND, NEW_FIRST, NEW_SECOND,
    LOAD, POLL, EFFECT, PENDING, ANIMATE, VELOCITY, PROPERTIES, AUDIO,
    AUDIO_READY, PATH, TRANSFER_READY, TIMING, FADE, KIND_COUNT,
    MISMATCH = 73
};
u8 fixture_slots[SLOTS_SIZE] __attribute__((aligned(16)));
u8 fixture_globals[GLOBALS_SIZE] __attribute__((aligned(16)));
__asm__(".globl D_800F2C40\n.set D_800F2C40, fixture_slots+32\n"
        ".globl D_8009AF9A\n.set D_8009AF9A, fixture_globals+32\n"
        ".globl D_8009AFA0\n.set D_8009AFA0, fixture_globals+33\n"
        ".globl D_8009B0F4_abs\n.set D_8009B0F4_abs, fixture_globals+36\n"
        ".globl D_8009B134_abs\n.set D_8009B134_abs, fixture_globals+40\n"
        ".globl D_80010014\n.set D_80010014, fixture_globals+44\n"
        ".globl D_80010018\n.set D_80010018, fixture_globals+48\n"
        ".globl D_80010024\n.set D_80010024, fixture_globals+52\n"
        ".globl D_80010028\n.set D_80010028, fixture_globals+56\n"
        ".globl D_8009AFFC\n.set D_8009AFFC, fixture_globals+60\n");
void func_8004FE2C(void);
static s32 old_first(s32, s32), old_second(s32, s32);
static s32 new_first(s32, s32), new_second(s32, s32);

static u8 expected_slots[SLOTS_SIZE], expected_globals[GLOBALS_SIZE];
static u32 cases, calls, offsets, indirects, scripted_cases;
static u8 state_coverage[256];
static u32 initial_state;
static s32 results[KIND_COUNT];

typedef struct {
    s32 kind, region;
    u32 offset, width, value;
} Patch;
static Patch patches[24];
static u32 patch_count;
typedef struct {
    s32 kind, args[7], has_offset;
    u8 offset[8], slots[SLOTS_SIZE], globals[GLOBALS_SIZE];
} Event;
static Event events[20];
static u32 event_count, event_cursor;

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
    for (i = 0; i < size; i++) out[i] = seed + i * 37 + (i >> 4);
}
static u32 get16(const u8 *p) { return p[0] | ((u32)p[1] << 8); }
static u32 get32(const u8 *p) { return get16(p) | (get16(p + 2) << 16); }
static void put16(u8 *p, u32 n) { p[0] = n; p[1] = n >> 8; }
static void put32(u8 *p, u32 n) { put16(p, n); put16(p + 2, n >> 16); }
static s32 signed_byte(u32 b) { return b < 128 ? b : (s32)b - 256; }
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
    write_text("entry state byte: ", 18); number(initial_state);
    write_text("event: ", 7); number(event_cursor);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(MISMATCH) : "memory");
    __builtin_unreachable();
}

/* Scripted external side effects are applied only after an entry observation.
 * Expected and actual executions have disjoint memory, sharing only the script. */
static void effects(s32 kind, u8 *globals, u8 *slots)
{
    u32 i;
    for (i = 0; i < patch_count; i++) {
        Patch *p = &patches[i];
        u8 *out = (p->region ? slots + GUARD : globals) + p->offset;
        if (p->kind != kind) continue;
        if (p->width == 1) *out = p->value;
        if (p->width == 2) put16(out, p->value);
        if (p->width == 4) put32(out, p->value);
    }
}
static void patch(s32 kind, s32 region, u32 offset, u32 width, u32 value)
{
    Patch *p;
    if (patch_count == 24) fail(1);
    p = &patches[patch_count++];
    p->kind = kind; p->region = region; p->offset = offset;
    p->width = width; p->value = value;
}
static s32 expect(s32 kind, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g,
                  const u8 *offset)
{
    Event *event;
    if (event_count == 20) fail(2);
    event = &events[event_count++];
    event->kind = kind;
    event->args[0] = a; event->args[1] = b; event->args[2] = c;
    event->args[3] = d; event->args[4] = e; event->args[5] = f;
    event->args[6] = g; event->has_offset = offset != 0;
    if (offset) bytes(event->offset, offset, 8);
    bytes(event->slots, expected_slots, SLOTS_SIZE);
    bytes(event->globals, expected_globals, GLOBALS_SIZE);
    effects(kind, expected_globals, expected_slots);
    return results[kind];
}
static s32 observe(s32 kind, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g,
                   const u8 *offset)
{
    s32 args[7];
    Event *event;
    if (event_cursor >= event_count) fail(3);
    event = &events[event_cursor++];
    args[0] = a; args[1] = b; args[2] = c; args[3] = d;
    args[4] = e; args[5] = f; args[6] = g;
    if (kind != event->kind || !equal(args, event->args, sizeof(args))) fail(4);
    if ((offset != 0) != event->has_offset) fail(5);
    if (offset && !equal(offset, event->offset, 8)) fail(6);
    if (!equal(fixture_slots, event->slots, SLOTS_SIZE)) fail(7);
    if (!equal(fixture_globals, event->globals, GLOBALS_SIZE)) fail(8);
    calls++;
    if (offset) offsets++;
    if (kind >= OLD_FIRST && kind <= NEW_SECOND) indirects++;
    effects(kind, fixture_globals, fixture_slots);
    return results[kind];
}

void func_80059AEC(s32 a) { observe(INITIAL,a,0,0,0,0,0,0,0); }
static s32 old_first(s32 a, s32 b) { return observe(OLD_FIRST,a,b,0,0,0,0,0,0); }
static s32 old_second(s32 a, s32 b) { return observe(OLD_SECOND,a,b,0,0,0,0,0,0); }
static s32 new_first(s32 a, s32 b) { return observe(NEW_FIRST,a,b,0,0,0,0,0,0); }
static s32 new_second(s32 a, s32 b) { return observe(NEW_SECOND,a,b,0,0,0,0,0,0); }
s32 Model_LoadMonsterMerge(s32 a,s32 b,s32 c,s32 d,s32 e,s32 f,s32 g)
{ return observe(LOAD,a,b,c,d,e,f,g,0); }
void func_80056828(s32 a) { observe(POLL,a,0,0,0,0,0,0,0); }
void func_8005F3B8(s32 a,s32 b,s32 c,s32 d,SVECTOR *v)
{ observe(EFFECT,a,b,c,d,0,0,0,(const u8 *)v); }
void func_800597C8(s32 a,s32 b,s32 c) { observe(PENDING,a,b,c,0,0,0,0,0); }
void Model_ControlSlotAnimation(s32 a,s32 b,s32 c) { observe(ANIMATE,a,b,c,0,0,0,0,0); }
void func_80059700(s32 a,s32 b) { observe(VELOCITY,a,b,0,0,0,0,0,0); }
void func_80059590(s32 a,s32 b,s32 c,s32 d,s32 e)
{ observe(PROPERTIES,a,b,c,d,e,0,0,0); }
void func_80047314(u32 a) { observe(AUDIO,a,0,0,0,0,0,0,0); }
s32 func_8004703C(void) { return observe(AUDIO_READY,0,0,0,0,0,0,0,0); }
void func_8005D994(s32 a,s32 b,s32 c,s32 d,SVECTOR *v,s32 f)
{ observe(PATH,a,b,c,d,0,f,0,(const u8 *)v); }
s32 func_8005FB08(void) { return observe(TRANSFER_READY,0,0,0,0,0,0,0,0); }
s32 func_80058E1C(void) { return observe(TIMING,0,0,0,0,0,0,0,0); }
void func_800156DC(void) { observe(FADE,0,0,0,0,0,0,0,0); }

static void pending(s32 a, s32 b) { expect(PENDING,0,a,b,0,0,0,0,0); }
static void velocity(s32 a) { expect(VELOCITY,0,a,0,0,0,0,0,0); }
static void increment(void) { expected_globals[STATE]++; }
static s32 target(u32 address, s32 second)
{
    if (address == (u32)(second ? old_second : old_first) - 4)
        return second ? OLD_SECOND : OLD_FIRST;
    if (address == (u32)(second ? new_second : new_first) - 4)
        return second ? NEW_SECOND : NEW_FIRST;
    fail(9);
}
static s32 indirect(s32 kind, s32 second, s32 mode)
{
    return expect(kind, get32(expected_globals + (second ? DATA2 : DATA1)),
                  mode,0,0,0,0,0,0);
}

static void oracle(void)
{
    /* Fresh stack copy, including padding, before the initial callback. */
    u8 offset[8], *s = expected_slots + GUARD, *g = expected_globals;
    s32 first = target(get32(g + FIRST), 0);
    s32 second = target(get32(g + SECOND), 1);
    s32 state, remaining, timing;
    u32 i;
    static const u16 threshold[14] = {
        0,0,0x780,0x780,0x1E0,0x1E0,0x1E0,0x1E0,0x1E0,0,
        0x8C0,0xE60,0xE60,0xE60
    };
    static const s16 x[4] = {-2500,2500,-500,500};
    static const s16 y[4] = {-1500,-1500,1500,1500};
    static const s16 angles[4] = {-0x100,0x100,-0x200,0x200};
    bytes(offset, g + OFFSET, 8);
    expect(INITIAL,1,0,0,0,0,0,0,0);
    if ((g[STATE] == 8 || g[STATE] == 9) && s[0xE15] == 0) {
        g[LATCH] = 0;
        if (indirect(first,0,-1) == 2) s[0xE15]++;
    }
    if (signed_byte(g[STATE]) >= 10 && s[SLOT_SIZE + 0xE15] == 0) {
        g[LATCH] = 0;
        if (indirect(second,1,-1) == 2) {
            velocity(0);
            expect(FADE,0,0,0,0,0,0,0,0);
            g[STATE] = 15;
        }
    }
    /* The table covers signed states -1..15; 127+1 wraps out of it. */
    state = signed_byte(g[STATE]);
    if (state < -1 || state > 15) return;
    if (state == -1) {
        if (s[0xE14] != 255) expect(POLL,0,0,0,0,0,0,0,0);
        else if (!(get32(g + FLAGS) & 0x02000030) && !get32(g + SECONDARY))
            expect(LOAD,0,0x309,0,0,0,0,4,0);
        if (!s[0xE1F]) return;
        expect(EFFECT,0,10000,0xE00,0,0,0,0,0);
        pending(1,0);
        if (s[0xE0F]) expect(ANIMATE,0,0,0,0,0,0,0,0);
        velocity(1);
        expect(PROPERTIES,0,5,0,0,0,0,0,0);
        s[0xBF6] = 1; s[0xBF4] = 2;
        expect(AUDIO,0x7310,0,0,0,0,0,0,0);
        for (i = 0; i < 3; i++) put32(s + 0xD08 + 4 * i, ~0u);
        g[LATCH] = 0;
        indirect(first,0,0);
        indirect(second,1,0);
        s[0xE15] = s[SLOT_SIZE + 0xE15] = 0;
        increment();
        return;
    }
    if (state == 0) {
        if (expect(AUDIO_READY,0,0,0,0,0,0,0,0) & 128) {
            put16(offset + 2,-2000);
            pending(0,10);
            expect(PATH,0,2000,0x100,0x100,0,110,0,offset);
            increment();
        }
        return;
    }
    if (state == 1) {
        if (s[0xDC0] < 128) {
            for (i = 0; i < 3; i++) s[0xDC0 + i] = (s[0xDC0 + i] + 2) & 255;
        } else {
            expect(PROPERTIES,0,0,128,128,128,0,0,0);
            increment();
        }
        return;
    }
    if (state == 2 && !expect(TRANSFER_READY,0,0,0,0,0,0,0,0)) return;
    if (state == 9) {
        if (s[0xBF5] == 2) increment();
        return;
    }
    if (state == 14) {
        remaining = get16(s + 0x7C4 + s[0xBF5] * 0x76) * 16;
        timing = expect(TIMING,0,0,0,0,0,0,0,0);
        remaining -= s[0xE0D] * timing;
        if ((s32)get16(s + 0xE06) >= remaining) velocity(0);
        return;
    }
    if (state == 15) { g[STATE] = 254; return; }
    if (get16(s + 0xE06) < threshold[state]) return;
    if (state == 2 || state == 3) pending(0,0);
    if (state >= 4 && state <= 7) {
        pending(0,0);
        put16(offset,x[state-4]); put16(offset+2,y[state-4]);
        put16(offset+4,1000);
        expect(EFFECT,0,3000,angles[state-4],-0x100,0,0,0,offset);
    }
    if (state == 8) {
        put16(offset+2,-2000);
        expect(EFFECT,0,3500,0x40,0x200,0,0,0,offset);
    }
    if (state == 10) {
        put16(offset+2,-1500);
        expect(PATH,0,3500,0,-0x80,0,40,0,offset);
    }
    if (state >= 11 && state <= 13) {
        if (state == 13) put16(offset+2,-1000);
        else put16(offset+4,-1000);
        expect(EFFECT,0,state == 13 ? 3000 : 5000,
               state == 13 ? 0 : state == 11 ? 0x200 : -0x200,
               state == 13 ? -0x80 : -0x100,0,0,0,offset);
        pending(0,190);
    }
    increment();
}

typedef struct {
    u32 state, e06, e14, ready, e0f, gate0, gate1, row, max, e0d;
    u32 flags, secondary, colors[3];
} Input;
static Input input;
static void defaults(u32 state)
{
    u32 i;
    input.state = state; input.e06 = 0; input.e14 = 0; input.ready = 0;
    input.e0f = 0; input.gate0 = input.gate1 = 1; input.row = 0;
    input.max = 100; input.e0d = 3; input.flags = input.secondary = 0;
    input.colors[0] = 0; input.colors[1] = 254; input.colors[2] = 255;
    patch_count = 0;
    for (i = 0; i < KIND_COUNT; i++) results[i] = 0;
}
static void run_case(void)
{
    static const u8 original_offset[8] = {0,0,0x24,0xFA,0,0,0,0};
    u8 *s = fixture_slots + GUARD, *g = fixture_globals;
    u32 i;
    cases++; initial_state = input.state; state_coverage[input.state] = 1;
    if (patch_count) scripted_cases++;
    fill(fixture_slots, SLOTS_SIZE, cases + 0x51);
    fill(fixture_globals, GLOBALS_SIZE, cases + 0x87);
    g[STATE] = input.state; g[LATCH] = 0xC3;
    put32(g + FLAGS,input.flags); put32(g + SECONDARY,input.secondary);
    put32(g + FIRST,(u32)old_first - 4); put32(g + SECOND,(u32)old_second - 4);
    put32(g + DATA1,0x12345678); put32(g + DATA2,0x789ABCDE);
    bytes(g + OFFSET,original_offset,8);
    s[0xE14] = input.e14; s[0xE1F] = input.ready; s[0xE0F] = input.e0f;
    s[0xE15] = input.gate0; s[SLOT_SIZE + 0xE15] = input.gate1;
    s[0xBF5] = input.row; put16(s + 0xE06,input.e06); s[0xE0D] = input.e0d;
    for (i = 0; i < 10; i++) put16(s + 0x7C4 + i * 0x76,0x4321 + i);
    put16(s + 0x7C4 + input.row * 0x76,input.max);
    for (i = 0; i < 3; i++) s[0xDC0 + i] = input.colors[i];
    bytes(expected_slots,fixture_slots,SLOTS_SIZE);
    bytes(expected_globals,fixture_globals,GLOBALS_SIZE);
    event_count = event_cursor = 0;
    oracle();
    func_8004FE2C();
    if (event_count != event_cursor) fail(10);
    if (!equal(expected_slots,fixture_slots,SLOTS_SIZE)) fail(11);
    if (!equal(expected_globals,fixture_globals,GLOBALS_SIZE)) fail(12);
}

static void boundaries(void)
{
    static const u32 values[] = {
        0,0x1DF,0x1E0,0x1E1,0x77F,0x780,0x781,
        0x8BF,0x8C0,0x8C1,0xE5F,0xE60,0xE61,0xFFFF
    };
    static const s32 returns[] = {0,1,-1,2};
    static const u32 audio[] = {0,1,0x7F,0x80,0x81,0xFFFFFFFF};
    static const u32 colors[] = {0,126,127,128,254,255};
    static const u32 flags[] = {0,0x10,0x20,0x02000000,0xFDFFFFCF,~0u};
    static const u32 secondary[] = {0,1,~0u};
    static const u32 presence[] = {0,1,0x80};
    u32 st,v,r,a,b,c,d;
    for (st = 0; st < 14; st++)
      for (v = 0; v < 14; v++)
       for (r = 0; r < 4; r++) {
        defaults(st); input.e06 = values[v];
        results[TRANSFER_READY] = returns[r]; results[AUDIO_READY] = 0x80;
        run_case();
       }
    for (r = 0; r < 6; r++) {
        defaults(0); results[AUDIO_READY] = audio[r]; run_case();
    }
    for (a = 0; a < 256; a++)
     for (b = 0; b < 6; b++) {
        defaults(1); input.colors[0] = a;
        input.colors[1] = colors[b]; input.colors[2] = colors[(b+3)%6];
        run_case();
     }
    for (a = 0; a < 2; a++)
     for (b = 0; b < 6; b++)
      for (c = 0; c < 3; c++)
       for (d = 0; d < 9; d++) {
        defaults(255); input.e14 = a ? 255 : 0; input.flags = flags[b];
        input.secondary = secondary[c]; input.ready = presence[d/3];
        input.e0f = presence[d%3]; run_case();
       }
    for (a = 0; a < 10; a++) {
        defaults(9); input.row = a; run_case();
    }
}

static void timing_boundaries(void)
{
    static const u32 maxima[] = {0,1,0x7FFF,0xFFFF};
    static const u32 multipliers[] = {0,1,255};
    static const s32 rates[] = {-3,0,1,7,256};
    static const u32 rows[] = {0,2,9};
    u32 a,b,c,d;
    s32 remaining, delta, value;
    for (a = 0; a < 4; a++)
     for (b = 0; b < 3; b++)
      for (c = 0; c < 5; c++)
       for (d = 0; d < 3; d++)
        for (delta = -1; delta <= 1; delta++) {
            defaults(14); input.max = maxima[a]; input.e0d = multipliers[b];
            input.row = rows[d]; results[TIMING] = rates[c];
            remaining = (s32)input.max * 16 - (s32)input.e0d * rates[c];
            value = remaining + delta;
            input.e06 = value < 0 ? 0 : value > 65535 ? 65535 : value;
            run_case();
        }
}

static void callback_mutations(void)
{
    static const u32 phases[] = {255,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,127,128};
    static const s32 boundaries[] = {0x1E0,0x780,0x8C0,0xE60};
    u32 i,j,k;
    /* Initial helper selects a fresh dispatch state, but cannot replace the
     * already captured function entries or eight-byte offset snapshot. */
    for (i = 0; i < 19; i++) {
        defaults(128); input.e06 = 0xFFFF; input.ready = 1;
        input.gate0 = input.gate1 = 0;
        results[AUDIO_READY] = 128; results[TRANSFER_READY] = 1;
        patch(INITIAL,0,STATE,1,phases[i]);
        patch(INITIAL,0,FIRST,4,(u32)new_first-4);
        patch(INITIAL,0,SECOND,4,(u32)new_second-4);
        patch(INITIAL,0,DATA1,4,0xC0011234);
        patch(INITIAL,0,DATA2,4,0xC0025678);
        patch(INITIAL,0,OFFSET,4,0x01234567);
        patch(INITIAL,0,OFFSET+4,4,0x89ABCDEF);
        patch(INITIAL,1,0xE0F,1,0x80);
        patch(INITIAL,1,2*SLOT_SIZE+0x731,1,0xD5);
        run_case();
    }
    /* The first gate can enable the second; all later dispatch/gate reads and
     * the E15 increment are live, while both indirect targets remain captured. */
    for (i = 0; i < 3; i++)
     for (j = 0; j < 4; j++) {
        defaults(8); input.gate0 = input.gate1 = 0;
        results[OLD_FIRST] = 2; results[OLD_SECOND] = j;
        patch(OLD_FIRST,0,STATE,1,10+i);
        patch(OLD_FIRST,1,0xE15,1,255);
        patch(OLD_FIRST,0,SECOND,4,(u32)new_second-4);
        patch(OLD_FIRST,0,DATA2,4,0xC0123456);
        patch(OLD_SECOND,0,STATE,1,j == 2 ? 4 : 15);
        patch(OLD_SECOND,1,SLOT_SIZE+0xE15,1,0x81);
        patch(VELOCITY,0,STATE,1,127);
        patch(FADE,0,STATE,1,128);
        run_case();
     }
    for (i = 0; i < 2; i++) {
        defaults(8); input.gate0 = input.gate1 = 0;
        patch(OLD_FIRST,0,STATE,1,i ? 10 : 255);
        patch(OLD_FIRST,1,SLOT_SIZE+0xE15,1,1);
        run_case();
    }
    /* Loader/poller ready reads, pending-animation E0F reads, late data
     * arguments, callback-written phase increments, and forced final clears. */
    for (i = 0; i < 2; i++)
     for (j = 0; j < 2; j++) {
        defaults(255); input.e14 = i ? 255 : 0; input.ready = j;
        input.e0f = j;
        patch(i ? LOAD : POLL,1,0xE1F,1,!j);
        patch(PENDING,1,0xE0F,1,!j);
        patch(AUDIO,0,DATA1,4,0x12340011);
        patch(AUDIO,0,LATCH,1,255);
        patch(AUDIO,1,0xD08,4,0x12345678);
        patch(OLD_FIRST,0,DATA2,4,0x56780022);
        patch(OLD_FIRST,0,STATE,1,127);
        patch(OLD_FIRST,1,0xE15,1,0xAB);
        patch(OLD_FIRST,0,SECOND,4,(u32)new_second-4);
        patch(OLD_SECOND,1,SLOT_SIZE+0xE15,1,0xCD);
        patch(OLD_SECOND,0,STATE,1,255);
        run_case();
     }
    /* Post-helper threshold reads must not use the entry E06. */
    for (i = 0; i < 2; i++)
     for (j = 0; j < 4; j++) {
        defaults(2); input.e06 = i ? 0x780 : 0x77F;
        results[TRANSFER_READY] = j;
        patch(TRANSFER_READY,1,0xE06,2,i ? 0x77F : 0x780);
        patch(TRANSFER_READY,0,STATE,1,127);
        run_case();
     }
    /* State increments reload after the final effect/pending callback.
     * Changes to the offset source cannot rewrite the local snapshot. */
    for (i = 0; i < 14; i++)
     for (j = 0; j < 2; j++) {
        defaults(i); input.e06 = 0xFFFF; input.colors[0] = 128; input.row = 2;
        results[AUDIO_READY] = 128; results[TRANSFER_READY] = 1;
        for (k = PENDING; k <= AUDIO_READY; k++) {
            patch(k,0,STATE,1,j ? 255 : 127);
        }
        patch(PATH,0,STATE,1,j ? 255 : 127);
        patch(EFFECT,0,STATE,1,j ? 255 : 127);
        patch(PENDING,0,OFFSET,4,0xFACE1234);
        patch(PENDING,0,OFFSET+4,4,0xABCD9876);
        run_case();
     }
    /* Row and max are captured before TIMING; E0D and E06 are read after.
     * The changed row remains valid and intentionally has a different max. */
    for (i = 0; i < 4; i++)
     for (j = 0; j < 3; j++) {
        defaults(14); input.row = 2; input.max = 100; input.e0d = 1;
        input.e06 = 0; results[TIMING] = 10;
        patch(TIMING,1,0xBF5,1,9);
        patch(TIMING,1,0x7C4+2*0x76,2,0xFFFF);
        patch(TIMING,1,0x7C4+9*0x76,2,0);
        patch(TIMING,1,0xE0D,1,i == 3 ? 255 : i);
        patch(TIMING,1,0xE06,2,i == 3 ? j : 1600-i*10+j-1);
        patch(TIMING,0,STATE,1,128);
        run_case();
     }
    /* Initial-helper field reloads around every motion threshold. */
    for (i = 0; i < 4; i++)
     for (j = 0; j < 2; j++) {
        defaults(i == 0 ? 4 : i == 1 ? 3 : i == 2 ? 10 : 11);
        input.e06 = boundaries[i] - !j;
        patch(INITIAL,1,0xE06,2,boundaries[i] - j);
        run_case();
     }
}

int main(void)
{
    static const s32 returns[] = {-1,0,1,2,3};
    u32 state, gate, r, covered = 0;
    for (state = 0; state < 256; state++)
     for (gate = 0; gate < 2; gate++)
      for (r = 0; r < 5; r++) {
        defaults(state); input.gate0 = input.gate1 = gate;
        results[OLD_FIRST] = results[OLD_SECOND] = returns[r];
        run_case();
      }
    boundaries();
    timing_boundaries();
    callback_mutations();
    for (state = 0; state < 256; state++) covered += state_coverage[state];
    write_text("state bytes: ",13); number(covered);
    write_text("cases: ",7); number(cases);
    write_text("callbacks: ",11); number(calls);
    write_text("offset snapshots: ",18); number(offsets);
    write_text("indirect calls: ",16); number(indirects);
    write_text("mutation scenarios: ",20); number(scripted_cases);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host compiler and ILP32 execution",
)
class ModelIntroControllerTests(unittest.TestCase):
    @staticmethod
    def source_text() -> str:
        text = SOURCE.read_text()
        if text.count(SOURCE_START) != 1 or text.count(SOURCE_MARKER) != 1:
            raise AssertionError("model intro source boundary changed")
        text = text[text.index(SOURCE_START):]
        return text.split(SOURCE_MARKER, 1)[0] + "\n"

    def build_and_run(
        self, optimization: str, mutation: str | None = None
    ) -> subprocess.CompletedProcess[str]:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        SCRATCH.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="run-", dir=SCRATCH) as temporary:
            directory = Path(temporary)
            text = self.source_text()
            if mutation is not None:
                replacements = {
                    "terminal-state": ("D_8009AF9A = -2;", "D_8009AF9A = -3;"),
                    "unsequenced-timing": (
                        "s32 timing = func_80058E1C();\n"
                        "        remaining -= slot->field_E0D * timing;",
                        "remaining -= slot->field_E0D * func_80058E1C();",
                    ),
                }
                original, replacement = replacements[mutation]
                self.assertEqual(text.count(original), 1)
                text = text.replace(original, replacement)
            text = re.sub(
                r'^#include "([^"]+)"',
                lambda match: '#include "' + str(
                    (SOURCE.parent / match.group(1)).resolve()
                ) + '"',
                text, flags=re.MULTILINE,
            )
            source = directory / "intro.c"
            source.write_text(text)
            fixture = directory / "fixture.c"
            fixture.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            flags = [
                "-std=gnu89", "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", optimization, "-I", str(REPOSITORY),
            ]
            objects = []
            for name, path in (("game", source), ("fixture", fixture), ("start", start)):
                obj = directory / (name + ".o")
                result = subprocess.run(
                    ["cc", *flags, "-c", str(path), "-o", str(obj)],
                    text=True, capture_output=True, timeout=60,
                )
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objects.append(str(obj))
            binary = directory / "witness"
            result = subprocess.run(
                ["cc", *flags, *objects, "-o", str(binary)],
                text=True, capture_output=True, timeout=60,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run([str(binary)], text=True, capture_output=True, timeout=120)

    def test_retail_intro_contract(self) -> None:
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "state bytes: 256\n"
                    "cases: 5853\n"
                    "callbacks: 11109\n"
                    "offset snapshots: 392\n"
                    "indirect calls: 1073\n"
                    "mutation scenarios: 93\n",
                )

    def test_actual_source_mutation_is_rejected(self) -> None:
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization, "terminal-state")
                self.assertEqual(result.returncode, 73, result.stdout + result.stderr)
                self.assertIn("witness mismatch: 12\n", result.stdout)

    def test_unsequenced_timing_mutation_is_rejected(self) -> None:
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization, "unsequenced-timing")
                self.assertEqual(result.returncode, 73, result.stdout + result.stderr)
                self.assertIn("witness mismatch: 3\n", result.stdout)
                self.assertIn("case: 5834\n", result.stdout)
                self.assertIn("entry state byte: 14\n", result.stdout)


if __name__ == "__main__":
    unittest.main()
