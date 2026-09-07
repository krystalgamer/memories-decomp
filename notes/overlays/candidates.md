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
  two were rebuilt and then matched; the third is still being recovered.

At the time of writing the rows below are the unmatched functions whose claimed
state is not stored here, in the order worth recovering:

- `func_8016A37C`, claimed at 365 of 365 with five differing positions. The
  closest unstored state in the project.
- `func_80181728`, claimed within four instructions of the target, with the
  residual attributed to four callee-saved hoists.
- `FreeDuel_Init`, claimed at 446 of 468, with the variable-identity lever
  already probe-verified.
- `func_801821DC`, no claimed state.

## password `func_8016913C` at 0x8016913C

`gcc_2_8_1_g0_split`, 381 instructions against 382, opcode distance 15.

Two content faults were read off the target's opcode classes rather than off
its positions, and both are about where a value is written rather than what
the code says.

The frame countdown is a `u16` local. The target stores the raw decremented
value with `sh` and then tests it with `sll v0,v0,0x10` followed by `bnez`,
which is the canonical way to ask whether the low sixteen bits are non-zero.
An `s32` local tests all thirty-two and needs no shift. Worth one of the
distance.

The `0x4000` flag is masked into a local before it is tested. Written as
`flag = (D_8009B3A4[0] & 0x4000) != 0` the compiler recognises a single-bit
test and folds it to `srl` by fourteen and `andi` by one. The target instead
has `andi` by `0x4000` followed by `sltu s0,zero,v0`, which is what a
comparison against a value it cannot see as one bit produces. Naming the
masked value defeats the fold. Worth two more.


The two `NameEntry_AdjustLength` sites always play a sound; they do not test
its result to decide whether to play one. The target reads
`bnez v0` selecting `a0 = 12` over `a0 = 9` and then falls into a single
`jal SD_SEPlayFull`, so the shape is
`SD_SEPlayFull(NameEntry_AdjustLength(...) ? 12 : 9)` rather than
`if (NameEntry_AdjustLength(...) == 0) SD_SEPlayFull(9)`. The stored candidate
had dropped the `12` path at both sites, which cost three instructions and
three of the distance. Correcting both is worth more than correcting either:
21 to 20 and 19 alone, 18 together.

This was found by `tools/project/overlay_arity_audit.py`, which reported that
the candidate did not make the target's last two calls, to
`NameEntry_AdjustLength` and `SD_SEPlayFull`. Reading the source against that
report is what exposed the dropped `12` path, since the calls were present but
conditional.

The audit still reports the same two calls as missing after the fix, so the
placement difference is a second and still-open fault rather than a symptom of
the first. The target lays this block out at the very end, after the main body
jumps over it to the epilogue at index 358; this candidate emits it inline at
the `select:` gate. Moving it behind a forward `goto` at the end of the
function was measured and is worse, 19 and 379 instructions, so the placement
is not controlled by statement position here.

Four spellings of the ternary at each site, sixteen cells, are all flat at 18,
and so are all 29 profiles. `gcc_2_8_1_g0_no_sched2_split` is worth noting as
the only near neighbour: distance 19 but 381 instructions and 334 differing
positions against this profile's 380 and 354.

The widget struct's tail fields sit at +0x5E and +0x60, so the padding
between +0x3E and them is 32 bytes, not 56. The earlier candidate put them
at +0x76 and +0x78, twenty-four bytes too far, and the field at +0x60 is
read unsigned. Neither the opcode distance nor the position count could see
that -- a `sh` is a `sh` at any offset, and those positions already differed
on their base register -- but the register-blind instruction multiset does,
and it falls by fourteen.

Three more source shapes were worth measuring. The frame counter's test
drops the `(s16)` cast, which is worth two of the distance. The cursor's
forward wrap compares `== 15` rather than `>= 15`, worth one. And the cell
lookup takes the table entry whole rather than masking it with `0xF`, worth
two more; the mask was never in the target.

Note that the differing-position count moves the wrong way across these,
311 to 352, because removing instructions displaces everything after them.
The distance and the register-blind measure both improve.

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
    u8 pad3E[32];
    u8 f5E;
    u8 pad5F;
    u16 f60;
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
    u16 cnt;
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
        cnt = w->f60 - 1;
        w->f60 = cnt;
        if (cnt != 0) {
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
            if (D_8016D401 == 15) {
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
                n = D_8009B3A4[0] & 0x4000;
                flag = (n != 0);
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
            func_8003FEE0(NameEntry_AdjustLength(-1, 6) != 0 ? 12 : 9);
        }
        return;
    }
    kind = 0;
    second = kind;
    row = (s8)D_8016D402;
    col = (s8)D_8016D401;
    n = D_8016AB38[row][col];
    gx = kind;
    if (n == 4) {
        if (col != 11) {
            d = 1;
            gx = 20;
        } else {
            d = -1;
        }
        func_8003FEE0(NameEntry_AdjustLength(d, 6) != 0 ? 12 : 9);
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
## main_menu `func_80180390` at 0x80180390

`gcc_2_8_1_g0_split`, 493 instructions against 495, opcode distance 14.

The cross-jump the previous entry decoded can be reproduced from the source,
and reproducing it properly is worth more than the conditional expression
that stood in for it.

That conditional expression fixed the call count but introduced a second
`sltiu` against 5. Listing every `slti` and `sltiu` on both sides shows the
target testing the menu id once in that region and this candidate testing it
twice, so the expression was buying the right call count at the cost of a
comparison the target does not make.

Routing the `SD_SEPlay(6, ...)` block's `return -1` through a shared label at
the end of the function is what actually lets GCC merge the two call sites,
because cross-jumping needs the two tails to reach the same place rather than
merely to look alike. With the two sounds written as separate branches again
and only that one `goto` added, the distance falls from 21 to 14 and the call
count stays at 29.

Which return goes through the label matters and is not symmetric: routing the
ninth sound's return through it as well gives 17, and routing only the ninth
gives 17 too. Only the sixth belongs there.

The structural defect is fixed. This candidate used to emit thirty calls
against the target's twenty-nine, which made every scheduling measurement on
it meaningless. Counting the target's calls by callee shows only four to
`SD_SEPlay` where the source had five, and reading the argument register at
each of the four gives 7, 6, 8 and 7 -- there is no call with 9 at all.

The target reaches the ninth sound by cross-jumping. At the menu-id test it
issues `sltiu v0,v0,5` then `bnez` into the middle of the `SD_SEPlay(6, ...)`
call site, with `li a0,9` in the branch delay slot, so one call site serves
both. The two blocks have identical tails -- a call and `return -1` -- which
is what makes them mergeable.

Writing the two sounds as one call with the id chosen by a conditional
expression reproduces the single call site and takes the distance from 21 to
15, with the call count now exactly 29. Putting the `>= 5` case first in an
`else` reaches 19, so the conditional-expression form is the better of the
two restructurings and both beat the original.
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
        goto ret_m1;
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
ret_m1:
    return -1;
    return -1;
}
```

## password `func_8016A37C` at 0x8016A37C

`gcc_2_8_1_g0_split`, 365 of 365 instructions, 94 differing positions,
opcode distance 0.

Two of those nine came from state 4, which is now correct apart from a
single position. Both are about *pseudo identity* rather than about what the
code says, and neither is visible in the source's meaning:

- The widget is read straight out of `D_8016D418` inside the `0x8000` arm
  rather than through the `widget` local that the following statements use.
  Worth six positions. Introducing a local there gives the value a longer
  live range than the target's, which changes its class.
- State 4 takes its *own* `flags` local instead of sharing the one that
  states 1, 2 and 3 use. Worth three more. The sharing is what couples the
  four live ranges into one pseudo and forces a common allocation.

The direction matters and is not uniform: giving state 3 its own local is
worth *minus* four, and giving every state its own is minus seven. Only
state 4 wants to be separate. All eight combinations were measured.

The instruction mix is exact, so only register choice and scheduling remain.

Six earlier findings still hold: the five-way dispatch is a `switch` whose
range check must not be duplicated; the starchip counter is unsigned; state 1
falls through into state 2 with an explicit `else` around its early exits; the
cursor's decrement arm stores before testing; the two message-box calls are one
call with the id chosen first; and `func_8002CCA8` takes the card id plus 1024.

Four more were needed to reach the exact mix.

`D_801D07E0` is not a scalar. The target forms a base at 0x801D0000 with
`lui` plus `addiu` and reads `2016(v0)`, which is the aggregate addressing
form; a scalar gives `lui %hi` plus `%lo(sym)(reg)` and no `addiu`. That one
instruction was the whole difference between distance 1 and distance 0.

The starchip step chain is four independent `if` statements over a
pre-initialised `step`, not an `if`/`else if` chain. The chain form makes the
compiler test before dividing and jump to the join; the sequential form lets it
divide, then test, then fall through, which is what the target does. Worth six
of the opcode distance and four instructions.

The `func_8002CCA8` result selects the 229 message when it is *non-zero*, and
the cost comparison is `cost < pool` selecting 228, with 227 in the `else`.
Both polarities were measured against both chain forms: they are not
independent, and the pair that wins was not the best of either axis alone.

The state-1 widget is read straight from the global for its two accesses
rather than through a local, which is worth six positions.

The starchip step divides by 10, 20, 30 and 40, not by 10, 100, 1000 and
10000, so the step grows with the magnitude of the count and the counter
drains in roughly constant time. The divisors are read off the magic
constants: an unsigned divide compiles to a multiply-high and a shift, and
0xCCCCCCCD shifted 3, 4 and 5 is division by 10, 20 and 40 while 0x88888889
shifted 4 is division by 30. The differing-position count cannot see this
error, because the whole block is displaced by one instruction and every
position in it differs either way; the register-blind instruction multiset
can, and it improves by sixteen.

```c
#include "../../src/types.h"

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[23];
    u8 f33;
} Widget;

typedef struct {
    u8 pad0[96];
    s16 f96;
    u8 pad98[10];
    u8 f108;
} Cursor;

typedef struct {
    u32 lo;
    u32 hi;
} Pair;

extern u16 D_8016D424;
extern u32 D_800EB12C;
extern u16 D_8016D49C;
extern Widget *D_8016D418;
extern s32 D_8016D428;
extern Cursor *D_8016D420;
extern u8 D_8016D410[];
extern u32 D_8016D438;
extern u32 D_801A8000[];
extern Pair D_801D5608;
extern u32 D_801D0000[];
extern volatile u16 D_8009B394;
extern volatile u16 D_8009B398;
extern volatile u16 D_8009B3A4;
extern u32 D_8009B0F4;
extern u32 D_8009B134;
extern u8 D_8009B269;
extern u8 D_8009B26C;
extern s8 D_8009B34D;

extern void func_80039794(void);
extern void SD_SEPlayFull(s32);
extern void Password_SetDigitCursorTarget(Cursor *);
extern void Password_RefreshDigitDisplay(void);
extern void func_8003FF34(void);
extern void Fade_WaitOut(void);
extern s32 Password_LookupCardID(void);
extern void func_80029164(s32, s32);
extern void func_8016A02C(s32);
extern s32 func_8002CCA8(s32);
extern void Password_CreateMessageBox(s32, s32);
extern void func_8002CCE4(s32);
extern void func_80021894(s32);
extern void Password_RefreshStarchipDisplay(void);

void func_8016A37C(void)
{
    Cursor *cursor;
    Widget *widget;
    s32 index;
    s32 digit;
    s32 state;
    u32 count;
    u32 step;
    u16 flags;
    u16 flags4;
    u16 card;
    s32 msg;

    func_80039794();
    if ((D_8016D420->f108 & 0x40) != 0) {
        return;
    }
    if ((D_800EB12C & 0x2008) != 0x2000) {
        return;
    }
    state = D_8016D424 & 0x1F;
    switch (state) {
    case 0:
        if ((D_8009B3A4 & 0xA000) != 0) {
            if ((D_8009B3A4 & 0x2000) != 0) {
                index = D_8016D428 + 1;
                D_8016D428 = index;
                if (index >= 8) {
                    D_8016D428 = 7;
                    return;
                }
            } else {
                index = D_8016D428 - 1;
                D_8016D428 = index;
                if (index < 0) {
                    D_8016D428 = 0;
                    return;
                }
            }
            SD_SEPlayFull(47);
            cursor = D_8016D420;
            Password_SetDigitCursorTarget(cursor);
            cursor->f96 = 8;
            cursor->f108 |= 0x40;
            return;
        }
        if ((D_8009B394 & 0x5000) != 0) {
            digit = D_8016D410[D_8016D428];
            if ((D_8009B394 & 0x1000) != 0) {
                digit = digit + 1;
                if (digit >= 10) {
                    digit = 0;
                }
            } else {
                digit = digit - 1;
                if (digit < 0) {
                    digit = 9;
                }
            }
            SD_SEPlayFull(7);
            D_8016D410[D_8016D428] = digit;
            Password_RefreshDigitDisplay();
            return;
        }
        if ((D_8009B398 & 0x20) != 0) {
            SD_SEPlayFull(8);
            func_8003FF34();
            Fade_WaitOut();
            D_8009B26C = D_8009B269;
            return;
        }
        if ((D_8009B398 & 0x40) != 0) {
            card = Password_LookupCardID();
            D_8016D49C = card;
            if (card == 0) {
                SD_SEPlayFull(9);
                return;
            }
            D_8016D424 = 1;
            SD_SEPlayFull(48);
        }
        return;
    case 1:
        flags = D_8016D424;
        if ((flags & 0x8000) == 0) {
            D_8016D424 = flags | 0x8000;
            func_80029164(0, D_8016D49C);
            return;
        }
        if ((flags & 0x4000) == 0) {
            if (((D_8009B0F4 & 0x2000030) | D_8009B134) != 0) {
                return;
            }
            D_8016D424 = flags | 0x4000;
            func_8016A02C(D_8016D49C);
            return;
        }
        D_8016D418->f33 = D_8016D418->f33 + 8;
        if (D_8016D418->f33 == 0) {
            D_8016D418->f8 &= 0xFFFB;
            SD_SEPlayFull(12);
            D_8016D424 = 2;
        } else {
            return;
        }
        /* fallthrough */
    case 2:
        flags = D_8016D424;
        if ((flags & 0x8000) == 0) {
            D_8016D424 = flags | 0x8000;
            D_801D5608.lo = D_801A8000[D_8016D49C * 2];
            D_801D5608.hi = D_8016D49C;
            if (func_8002CCA8(D_8016D49C + 1024) != 0) {
                Password_CreateMessageBox(229, 128);
                return;
            }
            if (D_801A8000[D_8016D49C * 2] < D_801D0000[504]) {
                msg = 228;
            } else {
                msg = 227;
            }
            Password_CreateMessageBox(msg, 0);
            D_8016D424 |= 0x4000;
            return;
        }
        if ((flags & 0x4000) != 0) {
            if (D_8009B34D == 0) {
                D_8016D424 = flags & 0xBFFF;
                func_8002CCE4(D_8016D49C + 1024);
                func_80021894(D_8016D49C);
                D_8016D424 = 3;
                return;
            }
        }
        D_8016D424 = 4;
        return;
    case 3:
        flags = D_8016D424;
        if ((flags & 0x8000) == 0) {
            D_8016D424 = flags | 0x8000;
            D_8016D438 = D_801A8000[D_8016D49C * 2];
        }
        count = D_8016D438;
        step = 1;
        if (count >= 10) { step = count / 10; }
        if (count >= 100) { step = count / 20; }
        if (count >= 1000) { step = count / 30; }
        if (count >= 10000) { step = count / 40; }
        if (step == 0) {
            step = 1;
        }
        count = count - step;
        D_8016D438 = count;
        D_801D0000[504] = D_801D0000[504] - step;
        if (count == 0) {
            D_8016D424 = 4;
            Password_RefreshStarchipDisplay();
        }
        return;
    case 4:
        flags4 = D_8016D424;
        if ((flags4 & 0x8000) == 0) {
            D_8016D424 = flags4 | 0x8000;
            D_8016D418->f8 |= 4;
        }
        widget = D_8016D418;
        widget->f33 = widget->f33 + 8;
        if ((s8)D_8016D418->f33 < 0) {
            Password_CreateMessageBox(226, 0);
            D_8016D424 = 0;
        }
        return;
    }
}
```
