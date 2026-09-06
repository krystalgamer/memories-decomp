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

`gcc_2_8_1_g0_split`, 138 of 138 instructions, 10 differing positions.

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

`gcc_2_8_1_g0_split`, 380 of 382 instructions, 332 differing positions.

Instructions 0 to 54 already agree. The first real difference is at 56, where
the target fills the branch delay slot with the `lui` for `D_8016D401` and this
build emits a `nop`, which shifts everything after it and accounts for most of
the differing positions. The remaining work is that slot and the block layout,
not the semantics.

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
            } else if ((D_8009B3A4[0] & 0x1000) != 0) {
                D_8016D402 = D_8016D402 - 1;
                if ((s8)D_8016D402 < 0) {
                    D_8016D402 = 8;
                }
            } else {
                D_8016D402 = D_8016D402 + 1;
                if ((s8)D_8016D402 >= 9) {
                    D_8016D402 = 0;
                }
            }
            D_8016D426 = D_8016D402;
        }
    } else if ((D_8009B398[0] & 0x800) != 0) {
        D_8016D401 = 14;
        D_8016D402 = 8;
    } else {
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
}
```

## password `func_8016A080` at 0x8016A080

`gcc_2_8_1_g0_split`, 161 of 161 instructions, 9 differing positions.

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
