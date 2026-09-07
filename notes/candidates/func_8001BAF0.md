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

### From 61 to 15

Every one of the 61 was a register name at the right position, the *allocation*
class, so register constraints reach them. The steps, each measured on the one
before it:

- *Pin the two long-lived table pointers, one side of each swap.* The
  `gDuel_aActiveCards` base belongs in `$t0` and the `D_800EAE88` walk pointer
  in `$s6`; pinning both takes 61 to **50**. Pinning the other side of either
  swap instead is worse, which is the same one-sided rule that carried
  `func_80057AF4`.
- *Pin the deck record pointer to `$16`*: 50 to **42**.
- *Name the second `gDuel_aActiveCards` read.* The inner walk reads the global
  again, and retail keeps that second materialisation in its own register
  `$t1`. A block-scoped `register u8 *act asm("$9") = gDuel_aActiveCards;` with
  `card = act + v * 12;` gives 42 to **32**.
- *Pin the hand slot value to `$5`*: 32 to **20**.
- *Compute the offset before the base, not after.* The `act` pin left five
  positions where the candidate materialised `$t1` **before** the `v * 12`
  chain and retail does it after. Naming the offset in an enclosing scope, so
  the multiply is a complete statement before the pinned declaration is
  reached, reorders them: 20 to **15**. This is the placement half of the
  standing cost of pinning a destination - the pinned value is materialised as
  early as its declaration allows, so the declaration has to be late.

### What is left

Fifteen positions in four groups.

*Six are `addu` operand order*, `addu s0,s8,s0` where retail has
`addu s0,s0,s8` and four more of the same shape. **Writing the addition the
other way round in C does not move them**: flipping `deck + sidx * 6` to
`sidx * 6 + deck`, and the three other pointer additions likewise, one at a
time and all together, is neutral in every case, because GCC canonicalises the
operands of `plus` before the register allocator ever sees them. Naming the
scaled offsets in their own statement is also neutral here, unlike the `act`
case where naming did reorder the materialisation.

*Three are the `D_800EAE88` limit*: retail materialises the base into `$t2` and
computes `$v0 = $t2 + 5`, the candidate accumulates both into `$v0`. Pinning
`end` to `$10` costs 22, naming the limit in its own variable is neutral, and a
block-scoped pinned copy costs 22.

*Two are the `D_800907CC` table pointer*, `$a2` for retail's `$a0`; pinning
`tbl` to `$4` costs one.

*Four are the two swapped slot bytes*, `$a0` for retail's `$a1`.

### From 61 to 2

Every one of the 61 was a register name at the right position, the *allocation*
class. Each step measured on the one before it:

- *Pin the two long-lived table pointers, one side of each swap*: the
  `gDuel_aActiveCards` base to `$t0`, the `D_800EAE88` walk pointer to `$s6`.
  61 to **50**.
- *Pin the deck record pointer to `$16`*: 50 to **42**.
- *Name the second `gDuel_aActiveCards` read.* The inner walk reads the global
  again and retail keeps that materialisation in its own `$t1`; a block-scoped
  `register u8 *act asm("$9")` gives 42 to **32**.
- *Pin the hand slot value to `$5`*: 32 to **20**.
- *Compute the offset before the base.* The `act` pin had put the `$t1`
  materialisation five positions too early. Naming `v * 12` in an enclosing
  scope, so the multiply is a finished statement before the pinned declaration
  is reached, reorders them: 20 to **15**. This is the placement half of the
  cost of pinning a destination - the pinned value is materialised as early as
  its declaration allows.
- *Pin the swapped record byte to `$5`*: 15 to **12**.
- *Cast the pointer to `s32` before adding, and write the offset first*: 12 to
  **10**, then to **7** applying the same to three more sites.
- *Hoist the end-of-hand limit out of the loop*: 7 to **4**.
- *Write the table subscript with the index first and the base last*: 4 to
  **3**, and naming the `D_8009B1D5 * 5` product in its own statement, **2**.

### The operand-order lever, and the earlier wrong conclusion

An earlier pass here recorded that "flipping the addition the other way round in
C does not move `addu` operand order, because GCC canonicalises `plus`". **That
was measured but wrong in its generality.** Flipping `deck + sidx * 6` to
`sidx * 6 + deck` is indeed neutral - both are *pointer* arithmetic and GCC
canonicalises the `plus`. Casting the pointer to `s32` first, so that the
addition is integer arithmetic, and then writing the offset on the left,
`(DeckCardRecord *)(sidx * 6 + (s32)deck)`, **does** flip the emitted operand
order. Six positions here came back that way.

The lesson is narrower than the one first recorded: **canonicalisation applies
to pointer addition, and casting to an integer type escapes it.** Applied one
site at a time, each of four sites is worth one or two positions, and the
direction is per-site - one of them wants the base first and the others want it
last, so they cannot be changed as a group.

### What is left

Two positions, one store. `sel[v - 0xB] = -1;` computes its address into `$a1`,
reusing the pinned hand value which is dead there, where retail uses `$v0`.
Measured and rejected: five spellings of the store, including an integer-cast
address, a named index, a named base pointer and a pinned address temporary; a
`continue`-shaped loop; a pointer walk instead of a subscript (+128); and
splitting the hand value into a separate variable for this loop (+8). All six
pins are load-bearing, costing 24, 9, 18, 14, 4 and 34 positions when removed
one at a time.

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
    register u8 *hand asm("$22");
    u8 *p;
    u8 *end;
    s8 *q;
    u8 *deck;
    register u8 *base asm("$8");
    u8 *tbl;
    u8 *recs;
    register DeckCardRecord *rec asm("$16");
    DeckCardRecord *other;
    Slot *slot;
    Spawned *spawned;
    u8 *card;
    s32 i;
    s32 j;
    register s32 v asm("$5");
    s32 id;
    s32 sidx;
    register s32 a asm("$5");
    s32 b;

    for (i = 0; i < HAND_SIZE; i++) {
        p = D_8009B1C8;
        sel[i] = *(u8 *)((0x1A + i) + (s32)p);
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
    end = D_800EAE88;
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
            {
                s32 o;

                o = v * 12;
                {
                    register u8 *act asm("$9") = gDuel_aActiveCards;

                    card = act + o;
                }
            }
            rec = (DeckCardRecord *)(sidx * 6 + (s32)deck);
            other = (DeckCardRecord *)(card[0xB] * 6 + (s32)deck);
            a = *(s8 *)&rec->slot;
            b = other->slot;
            rec->slot = b;
            other->slot = a;
            tmp = *rec;
            *rec = *other;
            *other = tmp;
            {
                s32 k;

                k = D_8009B1D5 * 5;
                id = *(u8 *)((j + k) + (s32)tbl);
            }
            spawned = slot->base;
            Duel_SetupCardRecord(id, *(s8 *)&rec->slot);
            slot->base = func_80018004((u8 *)(id * DUEL_CARD_RECORD_SIZE + (s32)recs), spawned->x, spawned->y);
            func_8004036C(spawned);
            *(u8 *)((s32)D_8009B1C8 + (0x1A + j)) = rec->slot;
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
    if ((s32)hand < (s32)(end + HAND_SIZE)) {
        goto next;
    }
}
```
