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
- If a row claims a near-miss state, the source that produces it belongs here.
  A claimed state with no stored source is not a result, because nobody can
  reproduce it. That has happened three times so far and each one cost a full
  reconstruction to recover: `func_80169734`, which the row had at 309 of 309
  with two differing positions, `CampaignMap_UpdateLocationTransition` at 217
  of 217 with seventy, and `func_8016A37C` at 365 of 365 with five. The first
  was rebuilt and then matched; the other two are still being recovered.

At the time of writing the rows below are the unmatched functions whose claimed
state is not stored here, in the order worth recovering:

- `func_8016A37C`, claimed at 365 of 365 with five differing positions. The
  closest unstored state in the project.
- `func_80181728`, claimed within four instructions of the target, with the
  residual attributed to four callee-saved hoists.
- `FreeDuel_Init`, claimed at 446 of 468, with the variable-identity lever
  already probe-verified.
- `func_801821DC`, no claimed state.

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

`gcc_2_8_1_g0_split`, 382 of 382 instructions, 311 differing positions, opcode
distance 26.

The digit-walk loop is written with an explicit goto rather than as a do/while.
GCC 2.8.1 with -msplit-addresses hoists the high half of any global address read
inside a loop it recognises into a callee-saved register, and the target does not
do that. The goto form is not seen as a loop by the front end, so the hoist does
not happen: it takes the length from 378 to the target's 382 and the missing lui
count from five to three.

The three that remain are the select branch's reads of D_8016D401, D_8016D402 and
D_8016AB38, which the target loads separately on that path while the compiler
hoists them above the branch so one materialisation serves both.

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
    again:
        col = col + cell;
        D_8016D401 = col;
        cell = D_8016AB38[row][(s8)col];
        if (cell < 0) {
            goto again;
        }
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

`gcc_2_8_1_g0_split`, 240 of 240 instructions, 44 differing positions, opcode
distance 2.

Do not rebuild this from scratch; it has been reconstructed three times.

Residual is one extra `addiu` against one missing `addu`.

The remaining difference is where the constant 16 lives. The target keeps it
in `a3`, the register that also carries argument 4, and uses that same
register as the shift amount at the `sllv`/`srav` pair. This candidate passes
the argument as a literal and keeps a separate local for the shift, so it
materialises 16 twice. Spelling the shift with a literal too collapses it to
`sll`/`sra` by immediate and loses the variable shift, which is worse: 237
instructions at distance 3. Passing the local for either argument position is
also worse (`n, n` 238 at distance 10; `n, 16` 239 at distance 9; `16, n` and
`m, n` 239 at distance 3).

**Do not measure this function under `no_sched1`.** That profile reports an
exact position count on the older 239-instruction candidate but at opcode
distance 16. See the "A matching count under `no_sched1` can be a false
positive" section in `README.md`.

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
                      16, 16, u & 0xFF, v & 0xFF, 10, 640, p->pal + 232);
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

## main_menu `func_80180390` at 0x80180390

`gcc_2_8_1_g0_split`, 498 instructions against 495, 476 differing positions,
opcode distance 21.

The six consecutive bytes from 0x80184598 to 0x8018459D are one struct, not
six scalar globals. That is what lets the compiler hold the high half of the
address in a callee-saved register across the poll calls and fold the low half
plus the member offset into each access, which is the form the target uses and
the one the row had recorded as unreachable.

```c
#include "../../src/types.h"

extern u8 *D_80184560;
extern u8 *gMain_apMenuEntries[];
extern u8 gMain_bMenuID;
extern u8 D_80184595;
extern u8 D_80184596;
extern u8 D_80184597;

typedef struct {
    s8 f98;
    u8 f99;
    u8 f9A;
    u8 f9B;
    u8 f9C;
    u8 f9D;
} MenuFlags;

extern MenuFlags D_80184598;
extern u16 D_8009B0D8;
extern u16 D_8009B394;
extern u16 D_8009B398;
extern u8 D_8009B3EA;
extern u8 D_8009B3ED;

extern void func_80180D2C(s32);
extern void func_80180E6C(u8 *);
extern void func_80040410(u8 *, s32);
extern void Input_ResetPads(void);
extern s32 SaveData_PollLoad(void);
extern void SaveData_RequestLoad(void);
extern s32 func_8003FCD8(void);
extern s32 func_8003FD14(void);
extern s32 func_8003F70C(void);
extern void func_8003F87C(void);
extern void SD_SEPlay(s32, s32, s32);
extern s32 rsin(s32);

s32 func_80180390(void)
{
    u8 *entry;
    u8 **slot;
    s32 step;
    s32 level;
    s32 value;
    s32 frame;
    s32 delta;
    s32 moved;
    s32 i;
    s32 base;
    s32 count;

    if (D_80184598.f9B != 0) {
        value = SaveData_PollLoad();
        if (value != 0) {
            if (value == 1) {
                Input_ResetPads();
                func_80180D2C(1);
            } else {
                Input_ResetPads();
            }
            D_80184598.f9B = 0;
        }
        return -1;
    }

    if (D_80184598.f9C != 0) {
        value = func_8003FCD8();
        if (value != 0) {
            if (value == 1) {
                Input_ResetPads();
                func_80180D2C(1);
            } else {
                Input_ResetPads();
            }
            D_80184598.f9C = 0;
        }
        return -1;
    }

    if (D_80184598.f9D != 0) {
        value = func_8003FD14();
        if (value != 0) {
            if (value == 1) {
                Input_ResetPads();
                func_80180D2C(1);
            } else {
                Input_ResetPads();
            }
            D_80184598.f9D = 0;
        }
        return -1;
    }

    if (D_80184598.f9A != 0) {
        if (func_8003F70C() == 0) {
            return -1;
        }
        Input_ResetPads();
        D_80184598.f9A = 0;
        return -1;
    }

    step = D_80184598.f98;
    if (step != 0) {
        level = D_80184597 + (step << 3);
        D_80184597 = level;
        if (step > 0) {
            if ((s8)level < 0) {
                goto fade_done;
            }
        }
        if (step >= 0) {
            return -1;
        }
        if ((u8)level != 0) {
            return -1;
        }
    fade_done:
        if (D_80184598.f98 < 0) {
            entry = D_80184560;
            entry[0xE] = 0x80;
            entry[0xD] = 0x80;
            entry[0xC] = 0x80;
            *(u16 *)(entry + 8) |= 0x40;
            D_80184560[0x6C] = 0x3C;
            *(s16 *)(D_80184560 + 0x36) = 0;
        }
        D_80184598.f98 = 0;
        return -1;
    }

    entry = D_80184560;
    if (entry != 0 && (*(u16 *)(entry + 8) & 0x40) != 0) {
        if (entry[0x6C] != 0) {
            entry[0x6C] = entry[0x6C] - 1;
        } else {
            value = entry[0xE] + entry[0x60];
            entry[0xE] = value;
            entry[0xD] = value;
            entry[0xC] = value;
            entry = D_80184560;
            value = entry[0xC];
            if ((u32)(value - 0x41) >= 0x3F) {
                if ((s8)value < 0) {
                    entry[0x6C] = 0x3C;
                }
                entry = D_80184560;
                value = *(s16 *)(entry + 0x60);
                *(s16 *)(entry + 0x60) = -value;
            }
        }
        if ((D_8009B398 & 0x800) != 0) {
            SD_SEPlay(7, 0xFF, 0);
            entry = D_80184560;
            *(u16 *)(entry + 8) &= 0xFFBF;
            func_80180D2C(0);
            D_80184598.f98 = 1;
            return -1;
        }
        entry = D_80184560;
        value = *(u16 *)(entry + 0x36) + D_8009B0D8;
        *(s16 *)(entry + 0x36) = value;
        if ((s16)value >= 0xBB8) {
            return -2;
        }
        return -1;
    }

    if (D_80184598.f99 != 0) {
        moved = 0;
        slot = gMain_apMenuEntries;
        i = 0;
    entry_loop:
        entry = *slot;
        if (entry == 0) {
            goto next_entry;
        }
        if (*(s16 *)(entry + 0x60) <= 0) {
            goto next_entry;
        }
        *(s16 *)(entry + 0x60) = *(u16 *)(entry + 0x60) - 1;
        if ((u32)gMain_bMenuID < 5) {
            if (i >= 5) {
                goto hide_entry;
            }
        } else {
            if (i < 5) {
                goto hide_entry;
            }
        }
        entry = *slot;
        delta = *(s16 *)(entry + 0x38) - *(s16 *)(entry + 0x36);
        frame = 0x10 - *(s16 *)(entry + 0x60);
        value = *(u16 *)(entry + 0x38);
        if (frame != 0x10) {
            value = rsin(frame << 6) * delta / 0x1000;
            entry = *slot;
            value = *(u16 *)(entry + 0x36) + value;
        }
        *(s16 *)(entry + 0x30) = value;
        if ((frame & 1) != 0) {
            func_80180E6C(*slot);
        }
        entry = *slot;
        *(u16 *)(entry + 8) = *(u16 *)(entry + 8) | 0x40;
        goto tick_entry;
    hide_entry:
        entry = *slot;
        *(u16 *)(entry + 8) = *(u16 *)(entry + 8) & 0xFFBF;
    tick_entry:
        moved++;
        func_80040410(*slot, (i << 1) | (gMain_bMenuID != i));
    next_entry:
        i++;
        slot++;
        if (i < 0xB) {
            goto entry_loop;
        }
        if (moved != 0) {
            return -1;
        }
        value = D_80184596;
        D_80184598.f99 = 0;
        if (value == 0) {
            return -1;
        }
        if (D_80184595 != 0) {
            if ((u32)gMain_bMenuID < 5) {
                for (i = 0; i < 0xB; i++) {
                    entry = gMain_apMenuEntries[i];
                    if (entry != 0) {
                        *(u16 *)(entry + 8) &= 0xFFBF;
                    }
                }
                D_80184598.f98 = -1;
            } else {
                func_80180D2C(0);
                gMain_bMenuID = 1;
            }
            D_80184595 = 0;
            return -1;
        }
        if (gMain_bMenuID != 1) {
            return gMain_bMenuID;
        }
        func_80180D2C(0);
        gMain_bMenuID = 5;
        return -1;
    }

    if ((D_8009B394 & 0x5000) != 0) {
        if ((u32)gMain_bMenuID < 5) {
            base = 0;
            count = 5;
        } else {
            base = 5;
            count = 6;
        }
        func_80040410(gMain_apMenuEntries[gMain_bMenuID], (gMain_bMenuID << 1) | 1);
        if ((D_8009B394 & 0x1000) != 0) {
            value = gMain_bMenuID - base + count - 1;
        } else {
            value = gMain_bMenuID - base + count + 1;
        }
        gMain_bMenuID = value % count + base;
        func_80040410(gMain_apMenuEntries[gMain_bMenuID], gMain_bMenuID << 1);
        SD_SEPlay(6, 0xFF, 0);
        return -1;
    }

    if ((D_8009B398 & 0x8E0) == 0) {
        return -1;
    }
    if ((D_8009B398 & 0x20) != 0) {
        if ((u32)gMain_bMenuID < 5) {
            SD_SEPlay(9, 0xFF, 0);
            return -1;
        }
        SD_SEPlay(8, 0xFF, 0);
        D_80184595 = 1;
    } else {
        SD_SEPlay(7, 0xFF, 0);
        switch (gMain_bMenuID) {
        case 1:
            SaveData_RequestLoad();
            D_80184598.f9B = D_80184598.f9B + 1;
            return -1;
        case 3:
            D_8009B3ED = 0;
            D_8009B3EA = 0;
            D_80184598.f9C = D_80184598.f9C + 1;
            return -1;
        case 2:
            D_8009B3ED = 0;
            D_8009B3EA = 0;
            D_80184598.f9D = D_80184598.f9D + 1;
            return -1;
        case 0xA:
            func_8003F87C();
            D_80184598.f9A = D_80184598.f9A + 1;
            return -1;
        }
    }
    func_80180D2C(1);
    return -1;
}
```

## overworld `CampaignMap_UpdateLocationTransition` at 0x80168AA8

`gcc_2_8_1_g0_split`, 209 instructions against 217, 182 differing positions,
opcode distance 14. Matches in both overworld modules from one source.

This is a fresh reconstruction from the disassembly and the inventory row. The
row describes a candidate at 217 of 217 with seventy positions, but that one was
never stored and is gone, so this is the first reproducible state.

Residual is extra `andi` x1 and `j` x1 against missing `lui` x3, `bne` x1,
`addu` x2, `nop` x1, `sltiu` x1, `lhu` x2 and `sll` x2.

Three differences are already located by reading the columns. The target
re-reads the state byte at index 64 before the 0x40 test where this build reuses
the value it already holds. It loads the map object inside the 0x40 block, at 69,
where this build hoists the load above the test. And it reloads the object's
+0x48 halfword at 87 to form +0x4A, where this build reuses the register it just
stored.

The five camera accumulators are confirmed against the disassembly: +0x5E4 by
+0x5F0 into camera+2, +0x5E8 by +0x5F4 into camera+4, +0x610 by +0x614 into
camera+0, +0x5CC by +0x5DC into camera+0x1C, and +0x5D0 by +0x5E0 into
camera+0x24, the last two as 32-bit stores.

```c
#include "../../src/types.h"

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[62];
    u16 f72;
    u16 f74;
} MapObject;

typedef struct {
    u8 pad0[96];
    s16 f96;
} Marker;

typedef struct {
    u8 pad0[12];
    u16 f12;
    u16 f14;
} Location;

extern u8 D_801695EC;
extern Marker *D_801695C8;
extern MapObject *D_801695D8;
extern u8 D_8016960C;
extern u8 D_80169618;
extern s32 D_80169608;
extern s32 D_801695D4;
extern Location D_801691A8[];
extern u8 D_800F2848[];

extern s32 D_801695E4;
extern s32 D_801695F0;
extern s32 D_801695E8;
extern s32 D_801695F4;
extern s32 D_80169610;
extern s32 D_80169614;
extern s32 D_801695CC;
extern s32 D_801695DC;
extern s32 D_801695D0;
extern s32 D_801695E0;

extern void func_80043178(Marker *);
extern void func_801688BC(s32);
extern void func_8004318C(Marker *, s32, s32, s32);
extern void func_801681E8(s32);
extern void func_8001352C(void);

s32 CampaignMap_UpdateLocationTransition(void)
{
    MapObject *obj;
    Marker *marker;
    Location *entry;
    Location *table;
    u8 *camera;
    u8 flags;
    u8 raise;
    s32 step;
    s32 timer;
    s32 quotient;

    camera = D_800F2848;
    flags = D_801695EC;
    if ((flags & 0x80) == 0) {
        marker = D_801695C8;
        D_801695EC = flags | 0x80;
        if (marker != 0) {
            func_80043178(marker);
            marker->f96 = 0;
        }
        D_801695D4 = D_80169608;
        func_801688BC(D_8016960C);
        if (D_8016960C < 10) {
            if (D_80169618 >= 10) {
                obj = D_801695D8;
                obj->f72 = 180;
                obj->f74 = 340;
                flags = D_801695EC;
                raise = obj->f8 | 0x40;
                D_801695EC = flags | 0x60;
                obj->f8 = raise;
            }
        } else if (D_80169618 < 10) {
            D_801695EC |= 0x40;
        }
    }
    flags = D_801695EC;
    if ((flags & 0x40) != 0) {
        obj = D_801695D8;
        if ((flags & 0x20) == 0) {
            obj->f72 = obj->f72 + 7;
        } else {
            step = obj->f72 - 7;
            obj->f72 = step;
            if ((s16)step < 32) {
                obj->f72 = 32;
            }
        }
        obj->f74 = obj->f72 + 160;
    }
    marker = D_801695C8;
    if (marker != 0) {
        timer = marker->f96;
        if ((s16)timer < 2048) {
            quotient = 2048 / D_80169608;
            table = D_801691A8;
            entry = table + D_8016960C;
            timer = (u16)marker->f96 + quotient;
            marker->f96 = timer;
            func_8004318C(marker, entry->f12, entry->f14, (s16)timer);
        }
    }
    D_801695E4 = D_801695E4 + D_801695F0;
    *(s16 *)(camera + 2) = D_801695E4 >> 16;
    D_801695E8 = D_801695E8 + D_801695F4;
    D_80169610 = D_80169610 + D_80169614;
    *(s16 *)(camera + 4) = D_801695E8 >> 16;
    *(s16 *)(camera + 0) = D_80169610 >> 16;
    D_801695CC = D_801695CC + D_801695DC;
    *(s32 *)(camera + 28) = D_801695CC >> 16;
    D_801695D0 = D_801695D0 + D_801695E0;
    *(s32 *)(camera + 36) = D_801695D0 >> 16;
    D_801695D4 = D_801695D4 - 1;
    if (D_801695D4 == 0) {
        func_801681E8(D_8016960C);
        if ((D_801695EC & 0x20) != 0) {
            obj = D_801695D8;
            obj->f72 = 32;
            obj->f74 = 192;
        }
        marker = D_801695C8;
        if (marker != 0) {
            table = D_801691A8;
            entry = table + D_8016960C;
            marker->f96 = entry->f12;
            *(s16 *)((u8 *)marker + 50) = entry->f14;
        }
    }
    func_8001352C();
    return D_801695D4;
}
```
