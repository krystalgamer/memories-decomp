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
