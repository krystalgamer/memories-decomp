## `func_8004CB0C` at 0x8004CB0C

`gcc_2_8_1_g0`, 394 instructions against a target of 394, **opcode multiset
distance 0**, 14 of 394 words differing. The candidate has the exact control-flow
and instruction shape, with no hard register assignments. Every remaining word
is a register-allocation difference.

Channel initialiser for the model/secondary-driver state rooted at
`D_800F2C40`. The selected `0xE20`-byte slot is reset, channels zero and one
receive their initial signed pan values, and the input command lists are scanned
into the slot's leading `{record, command}` pairs. Event tags 0 and 1 build and
finish stream records, tags 2 and 3 dispatch their specialised handlers, and
other tags install `GsU_00000000`. The tail then links each command to an
`0x50`-byte coordinate record, selects the first free record, and retries until
that record is not already owned by a live command.

## Why this candidate is closer

The user-supplied source established the complete shape: all 394 opcodes and the
relocation set match. Two source-lifetime changes improve its 29 differing words
to 14 without pinning a register:

- The fallback handler address is computed once in a function-scope `handler`
  local. GCC rematerialises it in the default arm, producing retail's
  `$v0`/`$t1` load and store sequence and improving the final add allocation.
- The retry scan has its own `search_slot` cursor. Shortening the primary
  `slot` live range makes GCC naturally assign that cursor to `$s4` and the
  scratch-pad pointer to `$s3`, matching every earlier use of both registers.

The raw byte offsets remain intentional. The same slot is viewed as an array of
8-byte command pairs at the front and as a state block in its tail, while the
record run reached through `+0xD14` has an independently proven `0x50` stride.
Giving those partial views a speculative whole-record type does not improve the
remaining allocation and would overstate what is known.

## Remaining 14 words

- Four words are the two `D_800E9D98` / `D_800E9D9C` loads. Retail uses `$v0`
  as a temporary address base before loading `$fp`; this candidate expands the
  symbolic load through `$fp` itself. Original ASPSX 2.81 was tested and emits
  the same expansion as MASPSX for the candidate assembly, so this is **not a
  MASPSX bug**. The source must make GCC emit a distinct address temporary.
- Eight words are confined to the retry scan. Its separate cursor and the
  limit/sentinel values use `$a1`/`$a2`/`$a3`, while retail reuses `$s4` and
  shifts the other two values down one argument register.
- Two words are the final reloads: the right stack values reach `$t0` and `$t1`
  in the opposite order. The resulting addition and store are exact.

The older pin-assisted experiment reached 390 of 394 words, but it forced the
pair cursor and handler-store temporaries into named registers. It remains
useful comparison evidence under `tmp/decompile-4cb0c-20260908/`; this tracked
candidate deliberately keeps the clearer unpinned source so the unresolved
work stays focused on ownership, lifetime, and expression shape.

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
extern void GsU_00000000(void);

extern void GsMapUnit(u8 *);
extern void func_8004D58C(s32, u8 *);
extern s32 GsScanUnit(s32 *, Event *, void *, void *);
extern Rec *GsMapCoordUnit(u8 *, s32 *);
extern s32 func_8004D134(s32, Event *, void *, s32 *, s32 *);
extern void func_8006086C(Event *);
extern void func_80060AEC(Event *);
extern void func_80060220(s32, Event *, void *);
extern void func_8005C6A0(Event *, u8 *);
extern s32 func_8005A3D0(u8 *, Rec *);

void func_8004CB0C(s32 index, u8 *arg1, s32 arg2, s32 arg3)
{
    u8 *base;
    u8 *slot;
    u8 *search_slot;
    u8 *cursor;
    void *table;
    Event ev;
    s32 acc;
    s32 count;
    s32 i;
    u32 tag;
    s32 off;
    s32 loaded_limit;
    s32 limit;
    s32 sentinel;
    s32 next;
    s32 *cmd;
    Rec *rec;
    Rec *scan;
    Rec *cur;
    Rec *q;
    s32 handler;

    handler = (s32)GsU_00000000;
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
    GsMapUnit(arg1);
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
                GsScanUnit(cmd, 0, 0, 0);
            evloop:
                if (GsScanUnit(0, &ev, table, (void *)0x1F800000) == 0) {
                    goto evdone;
                }
                {
                    void *scratch;

                    tag = (u32)ev.word >> 24;
                    scratch = (void *)0x1F800000;
                    if (ev.word == 0) {
                        goto evloop;
                    }
                    {
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
                        *(Rec **)(base + 0xD14) = GsMapCoordUnit(arg1, ev.ptr);
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
                    {
                        *ev.ptr = handler;
                        break;
                    }
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
    scan = *(Rec **)(base + 0xD14);
    if (scan != 0) {
        i = 0;
        while (scan->field_4C != 0) {
            scan++;
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
            search_slot = base;
            loaded_limit = base[0xE1A];
            i = 0;
            if (loaded_limit != 0) {
                sentinel = -1;
                limit = loaded_limit;
                do {
                    if (((Pair *)search_slot)->rec != 0 && ((Pair *)search_slot)->cmd != 0) {
                        if (*((Pair *)search_slot)->cmd != sentinel ||
                            *(((Pair *)search_slot)->cmd + 2) != 0) {
                            if (((Pair *)search_slot)->rec->field_4C == (s32)cur) {
                                break;
                            }
                        }
                    }
                    i++;
                    search_slot += 8;
                } while (i < limit);
                if (i < base[0xE1A]) {
                    break;
                }
            }
            next = func_8005A3D0(base, cur);
            if (!(next < base[0xE17])) {
                break;
            }
            cur = *(Rec **)(base + 0xD14) + next;
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
