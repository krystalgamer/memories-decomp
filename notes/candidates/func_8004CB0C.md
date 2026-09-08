## `func_8004CB0C` at 0x8004CB0C

`gcc_2_8_1_g0`, 392 instructions against a target of 394, **opcode multiset
distance 2**, 93 of 394 words differing. The instruction sequence is exact
through instruction 150, the frame and every saved-register role match retail,
and what is left is two loop-invariant constants that GCC hoists one loop
further out than retail does.

Channel initialiser for the secondary sound driver. `D_800F2C40` is an array of
`0xE20`-byte channel records (the same `Channel` array `func_8004D75C.c`
already declares) and the argument selects one. The function clears the whole
tail of the record, seeds the pan/volume pair from `D_8001001C`..`D_80010028`
for channels 0 and 1, then walks the command-list table the caller passed in:
for each of up to `0x3C` entries it opens the list with `func_80089F80`, pumps
events out of it, and dispatches on the top byte of each event word --
0 and 1 go to `func_8004D134` plus `func_8006086C`/`func_80060AEC`, 2 to
`func_80060220`, 3 to `func_8005C6A0`, anything else installs `func_80089E20`
as the handler. Events whose tag is in `{0, 1, 5, 6}` and which carry bit
`0x800000` first allocate a voice block through `func_8008A420`. It then links
each entry to its voice record, finds the first free record, and retries
`func_8005A3D0` until it lands on a record no live entry is using.

**The TU idiom is raw `u8 *` with byte offsets**, matching `sd_sequence_tracks.c`
and the rest of that family; only the two places where the arithmetic forces it
use typed pointers. Those are the `0x50`-byte voice records (indexed as
`base[0xD14] + i`, and one exact pointer difference divided by 80) and the
8-byte `{record, command}` pairs the channel record starts with.

## Levers, in the order they paid

- *Three `switch` statements, not `if`/`else` chains.* The channel-index test
  and the event-tag dispatch both compile to compare chains either way, but the
  branch **polarity** differs: a chain gives `bne`/`bnez` falling through to the
  case body, a `switch` gives `beq`/`beqz` jumping to it. Worth 12 positions
  across the two. GCC picks a balanced order for the tag switch -- 1, 0, 2, 3 --
  which is why the emitted order looks wrong for a switch and right for a chain.
- *Write `default:` first.* With `default` last, all three arms of the
  channel-index switch emit their own store and their own jump. With `default`
  written **before** `case 0`, GCC cross-jumps the two case arms onto one shared
  store, which is retail's layout: two `sh` and one `j`, not three and two.
  This was the single largest step, distance 5 to 1.
- *A scratch-pad pointer local is what makes GCC spill the parameters.* Retail
  keeps `0x1F800000` in `$s3` and reloads both incoming arguments from their
  home slots at `0x54`/`0x58(sp)`. Passing the constant inline at each of the
  three call sites instead leaves GCC enough registers to keep the arguments
  live, and the build loses five `lw` and gains four `addu`. The local has to
  exist, and it has to be **declared inside the event-loop body** after the
  event-word test -- at function scope it is hoisted out and the whole loop
  shifts by one instruction.
- *Transcribe the event loop with explicit `goto`s.* Written as
  `while (pump(...)) { if (word == 0) continue; ... }` GCC emits its own
  `a0 = 0` for the `continue` edge; written with a label before the argument
  setup and `goto`s for both the continue and the break edges, the `continue`
  path lands on the existing setup, which is retail. That is the last
  instruction of the count, distance 1 to 0. `goto` is already the idiom in
  this family -- `SD_ProcessSequenceTracks` in `sd_sequence_tracks.c` is written
  the same way.
- *Do not reuse one variable for a value passed to a call and a value that lives
  across it.* The record pointer stored to `+0xD18` and passed to
  `func_8005A3D0` is a short-lived temporary in retail: it is computed straight
  into `$a1` and stored from there. Sharing the variable with the long-lived
  cursor puts it in `$s0` and costs a `move a1, s0`. Splitting them was worth 33
  words.
- *The range test must be written as nested `if`s, not `&&`.*
  `tag < 2 || (tag < 7 && tag >= 5)` is folded by GCC into the range check
  `(tag - 5) < 2` and emits an `addiu`; retail keeps three separate `sltiu`.
  Every spelling of the compound condition folds -- `tag >= 5`, `!(tag < 5)`,
  `tag > 4`, and a signed cast all give the same output. Only writing the two
  bounds as separate nested `if`s with `goto`s out of them keeps them apart.
- *`u32` for the tag*, or the three bound tests come out `slti` instead of
  `sltiu`. *`cursor += 4` before the clamp*, not after, or the increment cannot
  reach the branch delay slot ahead of it.

## What is left

Two instructions, and they are the same fact twice: **retail materialises a
loop-invariant constant inside a loop where GCC hoists it out.**

- `lui $s3, 0x1F800000` sits in the delay slot of the event loop's break test
  and is re-executed every iteration. This build fills that slot with `nop` and
  materialises the constant one slot later, in the delay slot of the following
  branch. Six placements for the declaration and assignment were tried,
  including function scope, both loop bodies, and a block wrapping the break
  test; the tight in-body declaration is the best of them.
- `addiu $a2, $zero, -0x1` is materialised **inside** the retry loop, in a
  caller-saved register, because the scan loop it feeds contains no calls while
  the retry loop around it does. This build hoists the same constant out of the
  retry loop into a callee-saved register instead.

Both are loop-invariant-motion depth decisions rather than anything the source
spells, and no source shape tried moved either.

**One register pin is required and it is verified.** `slot` is pinned to `$s4`.
Without it the build is 106 words away instead of 93, because `slot` and the
scratch-pad pointer swap between `$s3` and `$s4`. Every one of the fourteen
instructions touching `$s4` in the build was checked against the disassembly
and all of them are the one variable, plus the prologue save and epilogue
restore -- no unrelated value shares the register. Pinning the scratch-pad
pointer to `$s3` instead scores identically; pinning both is no better; pinning
neither is worse. The pin was added only after the source shapes above were
exhausted.

Nothing here needs new linker aliases. `D_800F2C40` is already in
`c_symbols.ld`; `D_800E9D9C` and `D_8001001C`..`D_80010028` resolve from the
split symbol table.

```c
#include "../types.h"

typedef struct {
    u8 pad_00[0x4C];
    s32 field_4C;
} Rec;

typedef struct {
    Rec *rec;
    s32 *cmd;
} Pair;

typedef struct {
    u8 pad_0000[0xE20];
} Channel;

typedef struct {
    s32 word;
    s32 *ptr;
} Event;

extern Channel D_800F2C40[];
extern void *D_800E9D98;
extern void *D_800E9D9C;
extern s32 D_8001001C;
extern s32 D_80010020;
extern s32 D_80010024;
extern s32 D_80010028;
extern void func_80089E20(void);

extern void func_8008A280(u8 *);
extern void func_8004D58C(s32, u8 *);
extern s32 func_80089F80(s32 *, Event *, void *, void *);
extern Rec *func_8008A420(u8 *, s32 *);
extern s32 func_8004D134(s32, Event *, void *, s32 *, s32 *);
extern void func_8006086C(Event *);
extern void func_80060AEC(Event *);
extern void func_80060220(s32, Event *, void *);
extern void func_8005C6A0(Event *, u8 *);
extern s32 func_8005A3D0(u8 *, Rec *);

void func_8004CB0C(s32 index, u8 *arg1, s32 arg2, s32 arg3)
{
    u8 *base;
    register u8 *slot __asm__("$20");
    u8 *cursor;
    void *table;
    Event ev;
    s32 acc;
    s32 count;
    s32 i;
    u32 tag;
    s32 off;
    s32 n;
    s32 *cmd;
    Rec *rec;
    Rec *cur;
    Rec *q;

    base = (u8 *)&D_800F2C40[index];
    slot = base;
    cursor = arg1;
    if (index < 2) {
        table = D_800E9D98;
    } else {
        table = D_800E9D9C;
    }
    count = 0;
    acc = 0;
    base[0xE13] = 0;
    base[0xE12] = 0;
    if (arg3 >= 0) {
        base[0xE11] = arg3 & 0x7F;
        if (arg3 & 0x80) {
            base[0xDC7] = 5;
        } else {
            base[0xDC7] = 0;
        }
    }
    base[0xDC6] = 0;
    base[0xDC5] = 0;
    base[0xDC4] = 0;
    *(s16 *)(base + 0xDC8) = 0;
    *(s16 *)(base + 0xDCA) = 0;
    *(s16 *)(base + 0xDCC) = 0;
    *(s16 *)(base + 0xDCE) = 0;
    *(s16 *)(base + 0xDD0) = 0;
    *(s16 *)(base + 0xDD6) = -0x15E;
    *(s16 *)(base + 0xDD2) = -0x15E;
    switch (index) {
    default:
        *(s16 *)(base + 0xDD4) = 0;
        break;
    case 0:
        *(s16 *)(base + 0xDD4) = 0x1C2;
        break;
    case 1:
        *(s16 *)(base + 0xDD4) = -0x1C2;
        break;
    }
    base[0xE0E] = 6;
    base[0xE0F] = 0;
    base[0xE10] = 0;
    base[0xE17] = 0;
    base[0xE18] = 0;
    *(s32 *)(base + 0xD18) = 0;
    *(s32 *)(base + 0xD14) = 0;
    *(s32 *)(base + 0xD1C) = 0;
    base[0xE19] = 0;
    base[0xE1B] = 0;
    base[0xE1C] = 0;
    *(s32 *)(base + 0xDE8) = 0;
    *(s32 *)(base + 0xDEC) = 0;
    if (index < 2) {
        if (index != 0) {
            *(s32 *)(base + 0xDE8) = D_80010020;
        } else {
            *(s32 *)(base + 0xDE8) = D_8001001C;
        }
        if (index != 0) {
            *(s32 *)(base + 0xDEC) = D_80010028;
        } else {
            *(s32 *)(base + 0xDEC) = D_80010024;
        }
    }
    *(s32 *)(base + 0xDF0) = 0;
    *(s16 *)(base + 0xE00) = 0;
    *(s16 *)(base + 0xE02) = 0;
    *(s16 *)(base + 0xE04) = 0;
    *(s32 *)(base + 0xDF4) = 0;
    base[0xE15] = 0;
    base[0xE1E] = 0;
    base[0xE1F] = 0;
    if (arg1 == 0) {
        base[0xE1D] = 0;
        base[0xE14] = 0xFF;
        return;
    }
    func_8008A280(arg1);
    cursor += 0xC;
    func_8004D58C(index, arg1);
    base[0xE1A] = *cursor;
    cursor += 4;
    if ((u8)base[0xE1A] >= 0x3D) {
        base[0xE1A] = 0x3C;
    }
    if (base[0xE1A] != 0) {
        i = 0;
        do {
            *(s32 *)slot = 0;
            cmd = *(s32 **)cursor;
            cursor += 4;
            *(s32 **)(slot + 4) = cmd;
            if (cmd != 0) {
                func_80089F80(cmd, 0, 0, 0);
            evloop:
                if (func_80089F80(0, &ev, table, (void *)0x1F800000) == 0) {
                    goto evdone;
                }
                {
                    if (ev.word == 0) {
                        goto evloop;
                    }
                    {
                    void *scratch = (void *)0x1F800000;
                    tag = (u32)ev.word >> 24;
                    if (tag < 2) {
                        goto masktest;
                    }
                    if (tag < 7) {
                        if (tag < 5) {
                            goto dispatch;
                        }
                    } else {
                        goto dispatch;
                    }
                masktest:
                    if (ev.word & 0x800000) {
                        *(Rec **)(base + 0xD14) = func_8008A420(arg1, ev.ptr);
                        ev.word &= 0xFF7FFFFF;
                    }
                dispatch:
                    switch (tag) {
                    case 0:
                        *(u16 *)(base + 0xE04) += func_8004D134(index, &ev, scratch, &acc, &count);
                        func_8006086C(&ev);
                        break;
                    case 1:
                        *(u16 *)(base + 0xE04) += func_8004D134(index, &ev, scratch, 0, &count);
                        func_80060AEC(&ev);
                        break;
                    case 2:
                        func_80060220(index, &ev, scratch);
                        break;
                    case 3:
                        func_8005C6A0(&ev, base);
                        break;
                    default:
                        *ev.ptr = (s32)func_80089E20;
                        break;
                    }
                    }
                    goto evloop;
                }
            evdone: ;
            }
            i++;
            slot += 8;
        } while (i < base[0xE1A]);
    }
    slot = base + 8;
    i = 1;
    *(s16 *)(base + 0xE00) = count;
    *(s16 *)(base + 0xE02) = acc + i;
    if (i < base[0xE1A] - 1) {
        off = 0;
        do {
            base[0xE17]++;
            if (*(s32 *)(slot + 4) != 0) {
                *(s32 *)slot = *(s32 *)(base + 0xD14) + off;
            }
            off += 0x50;
            i++;
            slot += 8;
        } while (i < base[0xE1A] - 1);
    }
    rec = *(Rec **)(base + 0xD14);
    if (rec != 0) {
        i = 0;
        while (rec->field_4C != 0) {
            rec++;
            i++;
        }
        base[0xE18] = i;
        q = *(Rec **)(base + 0xD14) + i;
        *(Rec **)(base + 0xD18) = q;
        base[0xE19] = func_8005A3D0(base, q);
        if (!(base[0xE19] < base[0xE17])) {
            base[0xE19] = base[0xE18];
        }
        cur = *(Rec **)(base + 0xD14) + base[0xE19];
        *(Rec **)(base + 0xD1C) = cur;
        for (;;) {
            slot = base;
            n = base[0xE1A];
            if (n != 0) {
                i = 0;
                do {
                    if (((Pair *)slot)->rec != 0 && ((Pair *)slot)->cmd != 0) {
                        if (*((Pair *)slot)->cmd != -1 ||
                            *(((Pair *)slot)->cmd + 2) != 0) {
                            if (((Pair *)slot)->rec->field_4C == (s32)cur) {
                                break;
                            }
                        }
                    }
                    i++;
                    slot += 8;
                } while (i < n);
                if (i < base[0xE1A]) {
                    break;
                }
            }
            n = func_8005A3D0(base, cur);
            if (!(n < base[0xE17])) {
                break;
            }
            cur = *(Rec **)(base + 0xD14) + n;
        }
        rec = (Rec *)cur->field_4C;
        if (rec != *(Rec **)(base + 0xD18)) {
            *(Rec **)(base + 0xD1C) = rec;
            if (rec != 0) {
                base[0xE19] = rec - *(Rec **)(base + 0xD14);
            }
        }
    }
    if (*(s32 *)(base + 0xDE0) == 0) {
        *(s32 *)(base + 0xDE0) = arg2 + (s32)arg1;
    }
}
```
