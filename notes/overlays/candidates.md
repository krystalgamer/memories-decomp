# Overlay near-miss candidates

Candidate sources for unmatched overlay functions live in `tmp/`, which is not
tracked, so they are lost when a session ends. Every run that wants to continue
a near miss has had to rebuild its candidate from the prose in the function's
inventory row first. That has happened at least six times, three of them on
`func_80168CDC` alone, and each rebuild costs several minutes and can drift
from the state the row describes.

This file is the durable copy. A candidate belongs here once it is close enough
that the next run would otherwise rebuild it: roughly, when it reaches the
target's instruction count, or when its recorded state is the base for further
measurement.

Rules:

- Store the exact source that produces the recorded state, not a tidied version.
  These are measurements, and reformatting them can change the output.
- Record the profile and the measured result in the heading, and re-verify with
  `overlay_diff.py` before trusting a stored candidate. If it no longer
  reproduces, say so in the function's inventory row rather than silently
  editing it here.
- Delete an entry once the function matches and its source is promoted into
  `src/overlays/`.
- The inventory row remains the place for findings, negatives and levers. This
  file holds only code.

## password `func_80168AB4` at 0x80168AB4

`gcc_2_8_1_g0_split`, 138 of 138 instructions, 10 differing positions, opcode
distance 0. The instruction mix is already exact, so no change of source shape
can improve this; only register choice and scheduling remain.

Residual is the snap-block rotation: the target opens the block with the move
of zero into `a2` and closes it with the store of 2 into `+0x60`, while this
build opens with that store and leaves the argument last.

```c
#include "../../src/types.h"

extern s8 D_8016D42C;
extern void func_800429D8(u8 *);
extern void func_8004036C(u8 *);
extern void func_80042A78(u8 *);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_80040510(u8 *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80042BC0(void);
extern u8 *TextBox_GetGlyphAt(s32, s32, s32);
extern u8 *func_80168CDC(s32, u8 *);
extern void func_801688AC(void);

void func_80168AB4(u8 *w)
{
    u8 *o;
    u8 *node;
    s32 n;

    if ((w[0x6C] & 0x80) == 0) {
        w[0x6C] |= 0x80;
        func_800429D8(w);
        *(s16 *)(w + 0x36) =
            ((*(s16 *)(w + 0x44) - *(s16 *)(w + 0x30)) << 8) / *(s16 *)(w + 0x60);
        *(s16 *)(w + 0x38) =
            ((*(s16 *)(w + 0x46) - *(s16 *)(w + 0x32)) << 8) / *(s16 *)(w + 0x60);
    }
    if ((w[0x6C] & 0x40) != 0) {
        n = *(u16 *)(w + 0x60) - 1;
        *(s16 *)(w + 0x60) = n;
        if ((s16)n <= 0) {
            func_8004036C(w);
        }
        return;
    }
    func_80042A78(w);
    n = *(u16 *)(w + 0x60) - 1;
    *(s16 *)(w + 0x60) = n;
    if ((s16)n > 0) {
        o = func_800400AC(func_8004002C(), 1);
        func_80040510(o, *(s16 *)(w + 0x30), *(s16 *)(w + 0x32), 16, 16,
                      w[0x5C], w[0x5D], w[0x66], *(u16 *)(w + 0x40),
                      *(u16 *)(w + 0x42));
        *(u32 *)(o + 0x0C) = 0x606060;
        *(s16 *)(o + 0x60) = 6;
        *(void **)(o + 0x24) = func_80042BC0;
        *(u32 *)(o + 4) |= 0x50000000;
        return;
    }
    w[0x6C] |= 0x40;
    *(u32 *)(w + 0x30) = *(u32 *)(w + 0x44);
    *(s16 *)(w + 0x60) = 2;
    node = TextBox_GetGlyphAt(3, D_8016D42C << 4, 0);
    if (node == 0) {
        return;
    }
    o = func_80168CDC(3, node);
    *(void **)(o + 0x24) = func_801688AC;
    o[0x6C] = 5;
}
```

## password `func_8016913C` at 0x8016913C

`gcc_2_8_1_g0_split`, 378 of 382 instructions, 321 differing positions, and 9
unconditional jumps against the target's 9.

Block placement is correct: the select branch is written as a trailing label so
the compiler sinks it past the join, which is what makes the jump count agree.
An earlier shape that kept select as an `else` arm built 380 with 320 positions
but 11 jumps, so it scored better on count while being structurally wrong.

```c
#include "../../src/types.h"

typedef struct {
    u8 pad0[48];
    s16 f30;
    s16 f32;
    u8 pad34[2];
    s16 f36;
    s16 f38;
    u8 pad3A[2];
    u16 f3C;
    u8 pad3E[56];
    u8 f5E;
    u8 pad5F;
    s16 f60;
} W;

extern u16 D_8016D4D4;
extern W *D_8016D404;
extern s16 D_8016D434;
extern s16 D_8016D436;
extern s8 D_8016D401;
extern u8 D_8016D402;
extern u8 D_8016D426;
extern s8 D_8016D42C;
extern u16 *D_8016D418;
extern u8 D_8016D400;
extern s8 D_8016AB38[][15];
extern u8 D_8016ABC0[][2];
extern volatile u16 D_8009B3A4[];
extern volatile u16 D_8009B394[];
extern volatile u16 D_8009B398[];

extern void func_80042A78(W *);
extern void func_800429D8(W *);
extern void func_8003FEE0(s32);
extern s32 NameEntry_AdjustLength(s32, s32);
extern u8 *TextBox_GetGlyphAt(s32, s32, s32);
extern u8 *func_80168CDC(s32, u8 *);
extern void func_80168708(void);
extern void func_80168AB4(void);

void func_8016913C(void)
{
    W *w;
    s32 home;
    s32 delta;
    s32 n;
    s32 cell;
    s32 col;
    s32 row;
    s32 flag;
    s32 x;
    s32 y;
    u8 *node;
    u8 *obj;
    s32 kind;
    s32 second;
    s32 gx;
    s32 gy;
    s32 d;

    w = D_8016D404;
    if ((D_8016D4D4 & 0x4000) != 0) {
        home = w->f3C - 16;
        delta = w->f5E - home;
        if (delta != 0) {
            w->f3C = (delta >= 0) ? (w->f3C + 2) : (w->f3C - 2);
        }
        func_80042A78(w);
        n = w->f60 - 1;
        w->f60 = n;
        if ((s16)n != 0) {
            return;
        }
        w->f3C = w->f5E + 16;
        w->f30 = D_8016D434;
        w->f32 = D_8016D436;
        D_8016D4D4 &= 0xBFFF;
    }
    if ((D_8009B3A4[0] & 0xF000) != 0) {
        if ((D_8009B3A4[0] & 0x2000) != 0) {
            D_8016D401 = D_8016D401 + 1;
            if (D_8016D401 >= 15) {
                D_8016D401 = 0;
            }
        } else if ((D_8009B3A4[0] & 0x8000) != 0) {
            D_8016D401 = D_8016D401 - 1;
            if (D_8016D401 < 0) {
                D_8016D401 = 14;
            }
        }
        if ((D_8009B3A4[0] & 0x5000) != 0) {
            if (D_8016D401 >= 11) {
                flag = (D_8009B3A4[0] & 0x4000) != 0;
                D_8016D401 = 11;
                D_8016D402 = D_8016ABC0[(s8)D_8016D402][flag];
                D_8016D426 = D_8016D402;
            } else if ((D_8009B3A4[0] & 0x1000) != 0) {
                D_8016D402 = D_8016D402 - 1;
                if ((s8)D_8016D402 < 0) {
                    D_8016D402 = 8;
                }
                D_8016D426 = D_8016D402;
            } else {
                D_8016D402 = D_8016D402 + 1;
                if ((s8)D_8016D402 >= 9) {
                    D_8016D402 = 0;
                }
                D_8016D426 = D_8016D402;
            }
        }
    } else if ((D_8009B398[0] & 0x800) != 0) {
        D_8016D401 = 14;
        D_8016D402 = 8;
    } else {
        goto select;
    }
    func_8003FEE0(47);
    row = (s8)D_8016D402;
    col = (s8)D_8016D401;
    cell = D_8016AB38[row][col];
    if (cell < 0) {
        do {
            col = col + cell;
            D_8016D401 = col;
            cell = D_8016AB38[row][(s8)col];
        } while (cell < 0);
    }
    D_8016D402 = D_8016D426;
    w->f5E = 0;
    if (cell != 0) {
        if ((cell & 0x40) != 0) {
            w->f5E = 20;
        }
        D_8016D402 = cell & 0xF;
    }
    x = (s8)D_8016D401 * 20 + 22;
    if ((s8)D_8016D401 >= 11) {
        x = (s8)D_8016D401 * 20 + 42;
    }
    D_8016D434 = x;
    y = (s8)D_8016D402 * 18 + 24;
    func_800429D8(w);
    D_8016D436 = y;
    w->f60 = 8;
    w->f36 = ((D_8016D434 - w->f30) << 8) / 8;
    w->f38 = ((D_8016D436 - w->f32) << 8) / 8;
    D_8016D4D4 |= 0x4000;


select:
    if ((D_8009B394[0] & 0xC0) == 0) {
        if ((D_8009B394[0] & 0x20) != 0) {
            if (NameEntry_AdjustLength(-1, 6) == 0) {
                func_8003FEE0(9);
            }
        }
        return;
    }
    kind = 0;
    second = kind;
    row = (s8)D_8016D402;
    col = (s8)D_8016D401;
    n = ((u8 *)D_8016AB38)[row * 15 + col] & 0xF;
    gx = kind;
    if (n == 4) {
        if (col != 11) {
            d = 1;
            gx = 20;
        } else {
            d = -1;
        }
        if (NameEntry_AdjustLength(d, 6) == 0) {
            func_8003FEE0(9);
        }
        gy = 36;
    } else if (n == 6) {
        second = 2;
        gy = 72;
        D_8016D400 |= 0x40;
    } else {
        kind = 1;
        gx = (s8)D_8016D401 * 20;
        gy = ((s8)D_8016D402 * 9) << kind;
        func_8003FEE0(41);
    }
    node = TextBox_GetGlyphAt(kind, gx, gy);
    obj = func_80168CDC(kind, node);
    obj[0x6C] = 1;
    *(void **)(obj + 0x24) = func_80168708;
    if (node == 0) {
        *(u16 *)(obj + 8) &= 0xFFBF;
    }
    if (second != 0) {
        *(s16 *)(obj + 0x48) = 20;
        node = TextBox_GetGlyphAt(kind, gx + 20, gy);
        obj = func_80168CDC(kind, node);
        obj[0x6C] = 1;
        *(void **)(obj + 0x24) = func_80168708;
        *(s16 *)(obj + 0x48) = 0;
    }
    if (kind == 1) {
        u16 *slot;
        D_8016D400 |= 0x80;
        slot = &D_8016D418[D_8016D42C];
        if (node != 0) {
            *slot = *(u16 *)node;
        } else {
            *slot = 0;
        }
        obj = func_80168CDC(1, node);
        *(s16 *)(obj + 0x60) = 8;
        *(void **)(obj + 0x24) = func_80168AB4;
        *(s16 *)(obj + 0x46) = 204;
        obj[0x6C] = 6;
        *(s16 *)(obj + 0x44) = (D_8016D42C << 4) + 112;
    }
    return;
}
```

## password `func_8016A080` at 0x8016A080

`gcc_2_8_1_g0_split`, 161 of 161 instructions, 9 differing positions, opcode
distance 0. The instruction mix is already exact, so no change of source shape
can improve this; only register choice and scheduling remain.

Residual is the flag block's `v0`/`v1` pairing, closed against source shape and
the profile axis in the inventory row. Jump counts agree at zero, so block
placement is not the problem.

```c
#include "../../src/types.h"

extern u8 D_8016D410[];
extern u16 D_8016D424;
extern s32 D_8016D428;
extern u8 *D_8016D430;
extern u8 *D_8016D420;
extern u8 *D_8016D440[];
extern u8 D_800EA0E8[];
extern void func_80169C30(void);
extern void func_80169D10(void);
extern void func_80169D84(s32, s32);
extern void func_80029574(s32);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800404CC(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_800428EC(u8 *, s32);
extern void func_8016A02C(s32);
extern void func_80040510(u8 *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80042918(u8 *);
extern void func_8016A00C(u8 *);
extern void func_80040468(u8 *, s32, s32, s32, s32, s32);
extern void func_8003FF08(s32);
extern void func_80015A00(void);
extern void func_80169E20(void);
extern void func_80169F38(void);

void func_8016A080(void)
{
    s32 i;
    u8 *o;
    u8 *bg;
    u8 **slot;
    void *hook;
    u8 *p;

    i = 7;
    p = D_8016D410 + i;
    D_8016D428 = 0;
    D_8016D424 = 0;
    do {
        *p = 0;
        i--;
        p--;
    } while (i >= 0);
    func_80169C30();
    func_80169D10();
    func_80169D84(226, 1);
    D_8016D430 = D_800EA0E8;
    func_80029574(0);
    bg = D_8016D430;
    *(s16 *)(bg + 40) = 320;
    *(s16 *)(bg + 42) = 256;
    *(s16 *)(bg + 44) = 512;
    *(s16 *)(bg + 46) = 240;
    o = func_800400AC(func_8004002C(), 2);
    func_800404CC(o, 152, 40, 0, 2, 3, 31, 257);
    func_800428EC(o, -8);
    *(u16 *)(o + 8) |= 8;
    func_8016A02C(1);
    o = func_800400AC(func_8004002C(), 1);
    func_80040510(o, 256, 120, 32, 32, 16, 128, 30, 256, 240);
    {
        volatile u32 *pf = (volatile u32 *)(o + 4);
        u32 v;
        *(s16 *)(o + 72) = 13;
        *(s16 *)(o + 74) = 13;
        v = *(u32 *)(o + 4) & ~0x08000000;
        *pf = v;
        *(u32 *)(o + 4) = v | 0x50000000;
    }
    func_80042918(o);
    func_800428EC(o, 10);
    *(void **)(o + 36) = func_80169E20;
    func_8016A00C(o);
    hook = func_80169F38;
    slot = D_8016D440;
    *(u32 *)(o + 48) = *(u32 *)(o + 24);
    D_8016D420 = o;
    i = 0;
    do {
        o = func_800400AC(func_8004002C(), 2);
        func_80040468(o, 3, 1, i, 11, 524);
        *(u32 *)(o + 4) |= 0x40000000;
        func_80042918(o);
        func_800428EC(o, 8);
        *(void **)(o + 36) = hook;
        *(u16 *)(o + 8) |= 0x28;
        *slot = o;
        i++;
        slot++;
    } while (i < 4);
    func_8003FF08(29520);
    func_80015A00();
}
```

## password `func_801681A0` at 0x801681A0

`gcc_2_8_1_g0_split`, 150 instructions against 147, 109 differing positions, opcode
distance 9.

Remaining mix difference is two `srl` where the target has `srlv`, plus one
`lhu` and the `lw`/`sw` pair around the spilled `x + 3` corner. The target shifts by the register holding the fourth argument's 1. Naming a
local for that 1 is inert and has been removed from this candidate, since the
compiler folds it either way and its presence suggested a lever that is not
there.

```c
#include "../../src/types.h"

typedef struct {
    u8 pad0[20];
    s16 f14;
    u8 pad16[26];
    s16 f30;
    s16 f32;
    u8 pad34[8];
    u16 f3C;
    u16 f3E;
} Record;

extern void func_8005B260(void *, void *, u16, s32);

void func_801681A0(Record *r, void *ot)
{
    u8 *poly;
    u8 *line;
    s32 pri;
    s32 x;
    s32 y;
    u32 w;
    u32 h;
    s32 xm1;
    s32 xp3;
    s32 right;
    s32 bottom;

    poly = (u8 *)0x1F800000;
    line = (u8 *)0x1F800040;
    *(u32 *)(poly + 4) = 0x0000FF00;
    *(u32 *)(line + 4) = 0x0000FF00;
    *(u32 *)(line + 12) = 0;
    pri = r->f14;
    x = r->f30;
    y = r->f32;
    w = r->f3C;
    h = r->f3E;
    poly[3] = 5;
    poly[7] = 0x48;
    *(u32 *)(poly + 20) = 0x55555555;
    line[3] = 4;
    line[7] = 0x50;
    xm1 = x - 1;
    xp3 = x + 3;
    *(s16 *)(poly + 12) = xm1;
    *(s16 *)(poly + 8) = xm1;
    *(s16 *)(poly + 18) = y - 1;
    *(s16 *)(poly + 14) = y - 1;
    *(s16 *)(poly + 10) = y + 3;
    *(s16 *)(poly + 16) = xp3;
    func_8005B260(poly, ot, pri, 1);
    right = x + w;
    *(s16 *)(poly + 12) = right + 1;
    *(s16 *)(poly + 8) = right + 1;
    *(s16 *)(poly + 16) = right - 3;
    func_8005B260(poly, ot, pri, 1);
    bottom = y + h;
    *(s16 *)(poly + 18) = bottom + 1;
    *(s16 *)(poly + 14) = bottom + 1;
    *(s16 *)(poly + 10) = bottom - 3;
    func_8005B260(poly, ot, pri, 1);
    *(s16 *)(poly + 12) = xm1;
    *(s16 *)(poly + 8) = xm1;
    *(s16 *)(poly + 16) = xp3;
    func_8005B260(poly, ot, pri, 1);
    w = w >> 1;
    w = x + w;
    *(s16 *)(line + 16) = w;
    *(s16 *)(line + 8) = w;
    *(s16 *)(line + 10) = y + 2;
    *(s16 *)(line + 18) = 0;
    func_8005B260(line, ot, pri, 1);
    bottom -= 2;
    *(s16 *)(line + 10) = bottom;
    *(s16 *)(line + 18) = 192;
    func_8005B260(line, ot, pri, 1);
    y = y + (h >> 1);
    *(s16 *)(line + 18) = y;
    *(s16 *)(line + 10) = y;
    x += 2;
    *(s16 *)(line + 8) = x;
    *(s16 *)(line + 16) = 0;
    func_8005B260(line, ot, pri, 1);
    *(s16 *)(line + 8) = right - 2;
    *(s16 *)(line + 16) = 320;
    func_8005B260(line, ot, pri, 1);
}
```

## password `func_80168CDC` at 0x80168CDC

`gcc_2_8_1_g0_split`, 239 of 240 instructions, opcode distance 9.

Do not rebuild this from scratch; it has been reconstructed three times.

Residual is extra `nop` x4 against missing `addiu` x3 and `addu` x2.

The residual decomposes into a cause and a symptom. The five missing
arithmetic instructions are the cause: the target recomputes an address at
each use where this candidate hoists it into a register. The four extra
`nop`s are the symptom -- with five fewer instructions available to fill
delay slots, the scheduler emits padding instead. Chasing the `nop`s
directly will not work; they close on their own once the arithmetic
appears.

**Do not measure this function under `no_sched1`.** That profile reports
240/240, an exact position count, but its opcode distance is 16 -- it
reaches the right total by way of a worse instruction mix. See the
"A matching count under `no_sched1` can be a false positive" section in
`README.md`.

```c
#include "../../src/types.h"

typedef struct { u16 code; u8 pad2[10]; s16 x; s16 y; } Widget;
typedef struct { u8 pad0[60]; s16 ox; s16 unk3E; s16 oy; u8 pad42[18]; u8 pal; u8 pad55[15]; } Panel;
typedef struct { u8 pad0[72]; u32 f72; void *f76; u8 pad80[23]; u8 f103; u8 pad104[2]; u8 f106; } Obj;
typedef struct { u8 pad0[48]; s16 x; s16 y; } Fixed;

extern Panel D_800EB0F8[];
extern Fixed *D_8016D404;
extern u8 D_8016D408;
extern void *func_8004002C(void);
extern Obj *func_800400AC(void *, s32);
extern void func_80040510(Obj *, s32, s32, s32, s32, s32, s32, s32, s32, s32);

Obj *func_80168CDC(s32 slot, Widget *w)
{
    s32 shift;
    s32 u;
    s32 v;
    s32 code;
    Obj *obj;
    Panel *p;
    s32 i;

    shift = 0;
    u = shift;
    v = shift;
    code = w->code;
    obj = func_800400AC(func_8004002C(), 1);
    if (w != 0) {
        s32 n = 16;
        s32 m = n;
        p = &D_800EB0F8[slot];
        switch (code) {
        case 0x8171: case 0x8173: code = 0x8183; break;
        case 0x8172: case 0x8174: code = 0x8184; break;
        }
        if ((u32)(code - 0x824F) < 76) {
            if ((u32)(code - 0x8259) < 7) { u = 0; v = 120; }
            else if ((u32)(code - 0x827A) < 7) { v = 120; }
            else {
                u = (code & 0xF) << 4;
                v = (((code - 0x8240) >> 4) << 4) + 72;
            }
        } else {
            s32 tbl[30] = {
                0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166, 0x8169,
                0x816A, 0x8196, 0x817B, 0x8143, 0x817C, 0x8144, 0x815E, 0x8146,
                0x8147, 0x8183, 0x8181, 0x8184, 0x8148, 0x8140, 0x83BF, 0x83C0,
                0x81C1, 0x81A9, 0x81A8, 0x81BC, 0x81BD, -1,
            };
            i = 0;
            for (;;) {
                if (tbl[i] == code) {
                    if (i < 15) { u = i << 4; v = 72; }
                    else if (i < 22) { u = (i << 4) - 96; v = 88; }
                    else {
                        switch (code) {
                        case 0x83BF: u = 208; v = 152; break;
                        case 0x83C0: u = 224; v = 152; break;
                        case 0x81C1: u = 240; v = 152; break;
                        case 0x81A9: u = 176; v = 120; break;
                        case 0x81A8: u = 192; v = 120; break;
                        case 0x81BC: u = 224; v = 120; shift = 2; break;
                        case 0x81BD: u = 240; v = 120; shift = -2; break;
                        }
                    }
                    goto draw;
                }
                if (tbl[i] < 0) { break; }
                i++;
            }
            u = 0;
            v = 120;
        }
draw:
        func_80040510(obj, p->ox + w->x + ((shift << n) >> n), p->oy + w->y,
                      n, m, u & 0xFF, v & 0xFF, 10, 640, p->pal + 232);
        obj->f106 = D_8016D408;
        D_8016D408 = D_8016D408 + 1;
    } else {
        s32 n = 16;
        func_80040510(obj, D_8016D404->x, D_8016D404->y, n, n, 128, 128, 23,
                      256, 240);
    }
    obj->f72 = 0x80008;
    obj->f103 = slot;
    obj->f76 = w;
    return obj;
}
```
