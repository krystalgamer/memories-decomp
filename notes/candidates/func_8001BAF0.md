## `func_8001BAF0` at 0x8001BAF0

`gcc_2_8_1_g8_split`, 150 of 150 instructions, opcode distance 0, 61 differing
positions, all of them register names: the hand pointer and the selection
slot pointer are `$s5`/`$s6` the other way round, the deck record pointer and
the card id `$s0`/`$s1` the other way round, the table pointer `$a2` for
`$a0`, and the temporaries that follow from those.

The hand reorder after a card selection, next to the matched
`func_8001B938`. It copies the five hand slot indices from the record at
`D_8009B1C8` (+0x1A) into a local array, clears every entry that the
selection bytes at `D_800EAE88` name as a hand slot (values 0xB to 0xF),
then walks the selection bytes until a zero: for each field card (0x10 and
above) it takes the first still-set hand slot, swaps that slot's byte 2 and
then the whole 6-byte record with the record of the card's slot in the
6-byte table 0x31E0 below `gDuel_aActiveCards`, looks the new card id up in
`D_800907CC` for the side in `D_8009B1D5`, sets its record up, replaces the
hand slot's display object through `func_80018004` on the 0x1C-byte record
at `D_801A7AD8`, releases the old one, writes the slot byte back into the
record, marks the selection byte with the slot number and clears the local
entry.

Six canonical attempts at 480 to 540 bytes against 600. This is the first
exact multiset.

**What the shape needed.**

- *The 6-byte record swap is a struct assignment through a stack
  temporary* (a struct with an `s16` member, so it copies as `lwl`/`lwr`
  plus `lh`), with the slot byte swapped separately first, as retail does.
- *The deck table is addressed relative to `gDuel_aActiveCards`,* through a
  base local (`base = gDuel_aActiveCards; deck = base - 0x31E0`); written
  as one expression the offset folds into the relocation.
- *Both walks are goto loops.* Real loops hoist the card address
  (`gDuel_aActiveCards + v * 12`) and the bound out of the body; retail
  computes both inside every iteration, and enters the outer walk without
  a test.
- *The bound is `(s32)hand < (s32)(end + 5)` with `end` a local read inside
  the loop,* giving retail's separate `addiu $v0, $t2, 5` and signed `slt`.
- *The record bytes are read raw:* `lbu` for the other record's slot and the
  final store, `lb` for the `Duel_SetupCardRecord` argument.
- *`func_80018004` takes the record pointer plus x and y, and
  `Duel_SetupCardRecord` the id plus the record's slot byte* (retail sets
  `$a1` from the record right before the call).

**Where the residual is.** With goto loops every reference weighs 1 and
the hand pointer (six references) outranks the selection slot pointer
(three), so global allocation hands them `$s5`/`$s6` the wrong way round.
Making the inner walk a real loop applies the loop-depth weighting and
gives retail's pair, but then loop.c hoists the card address out of the
body, which retail does not do; the natural nested-`for` spelling with
every address computed in the body hoists more still. The `$s0`/`$s1` and
`$a0`/`$a2` pairs move with the same choice. Crossed: declaration orders,
the address computations inside and outside the inner loop, the card
address inline and through a local, and outer-goto with inner-real.

```c
#include "../types.h"
#include "card_constants.h"
#include "duel_card_layout.h"

typedef struct {
    u8 f0;
    u8 f1;
    u8 slot;
    u8 f3;
    s16 f4;
} DeckCardRecord;

typedef struct {
    u8 p0[0x30];
    s16 x;
    s16 y;
} Spawned;

typedef struct {
    Spawned *base;
    Spawned *child;
    u8 p8[4];
} Slot;

extern u8 *D_8009B1C8;
extern u8 D_8009B1D5;
extern u8 D_800EAE88[];
extern u8 D_800907CC[];
extern u8 D_801A7AD8[];
extern Slot D_800EA030[HAND_SIZE];
extern u8 gDuel_aActiveCards[];

extern void Duel_SetupCardRecord(s32, s32);
extern Spawned *func_80018004(u8 *, s32, s32);
extern void func_8004036C(Spawned *);

void func_8001BAF0(void)
{
    s8 sel[HAND_SIZE];
    DeckCardRecord tmp;
    u8 *hand;
    u8 *p;
    u8 *end;
    s8 *q;
    u8 *deck;
    u8 *base;
    u8 *tbl;
    u8 *recs;
    DeckCardRecord *rec;
    DeckCardRecord *other;
    Slot *slot;
    Spawned *spawned;
    u8 *card;
    s32 i;
    s32 j;
    s32 v;
    s32 id;
    s32 sidx;
    s32 a;
    s32 b;

    for (i = 0; i < HAND_SIZE; i++) {
        p = D_8009B1C8;
        sel[i] = p[0x1A + i];
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = D_800EAE88[i];
        if (v == 0) {
            break;
        }
        if (v < 0x10) {
            sel[v - 0xB] = -1;
        }
    }
    base = gDuel_aActiveCards;
    deck = base - 0x31E0;
    hand = D_800EAE88;
next:
    v = *hand;
    if (v == 0) {
        return;
    }
    if (v >= 0x10) {
        j = 0;
        tbl = D_800907CC;
        recs = D_801A7AD8;
        slot = D_800EA030;
    inner:
        q = &sel[j];
        sidx = *q;
        if (sidx >= 0) {
            card = gDuel_aActiveCards + v * 12;
            rec = (DeckCardRecord *)(deck + sidx * 6);
            other = (DeckCardRecord *)(deck + card[0xB] * 6);
            a = *(s8 *)&rec->slot;
            b = other->slot;
            rec->slot = b;
            other->slot = a;
            tmp = *rec;
            *rec = *other;
            *other = tmp;
            id = tbl[j + D_8009B1D5 * 5];
            spawned = slot->base;
            Duel_SetupCardRecord(id, *(s8 *)&rec->slot);
            slot->base = func_80018004(recs + id * DUEL_CARD_RECORD_SIZE, spawned->x, spawned->y);
            func_8004036C(spawned);
            D_8009B1C8[0x1A + j] = rec->slot;
            *hand = j + 0xB;
            *q = -1;
        } else {
            j++;
            slot++;
            if (j < HAND_SIZE) {
                goto inner;
            }
        }
    }
    hand++;
    end = D_800EAE88;
    if ((s32)hand < (s32)(end + HAND_SIZE)) {
        goto next;
    }
}
```
