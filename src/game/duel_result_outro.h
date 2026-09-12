#ifndef MEMORIES_DECOMP_DUEL_RESULT_OUTRO_H
#define MEMORIES_DECOMP_DUEL_RESULT_OUTRO_H

#include "../types.h"
#include "duel_card_display_state.h"
#include "display_object.h"

void func_80020EE8(DuelCardDisplayObject *object);

#define DUEL_RESULT_SPRITE_COUNT 7

/* One row entry of the duel-result sprite tables D_80090928 (a real opponent)
   and D_80090960 (none). Each table is indexed by the winning side and then
   by sprite, DUEL_RESULT_SPRITE_COUNT of them, and func_80020F4C spends the
   whole entry in one call: `x` and `y` are the spawn position it hands
   func_800428A8, `kind` is that call's part selector and doubles as the
   "this slot is used" test, and `tag` goes to the object's field_48. */
typedef struct {
    u8 x;
    u8 y;
    u8 kind;
    u8 tag;
} DuelResultSpriteSpec;

/* One slot of the table func_80020F4C keeps in the gDuel_awRitualData scratch
   while the outro runs: the object it spawned for that sprite, so step 3 can
   retarget every one of them. Only the pointer is reached; the rest of the
   0xC-byte stride is what the scratch already holds. */
typedef struct {
    DisplayObject *object;
    u8 pad_04[0xC - 4];
} DuelResultSpriteSlot;

typedef char DuelResultSpriteSpec_size_must_be_4[
    sizeof(DuelResultSpriteSpec) == 4 ? 1 : -1
];
typedef char DuelResultSpriteSlot_size_must_be_0xC[
    sizeof(DuelResultSpriteSlot) == 0xC ? 1 : -1
];

/* gDuel_apfnSceneStateHandler duel phase entry: the result outro. It spawns one object per row
 * of the winning side's sprite table -- D_80090928 with a real opponent,
 * D_80090960 without -- keeps them in the gDuel_awRitualData scratch as
 * DuelResultSpriteSlot entries, and retargets all of them on its third step. */
void func_80020F4C(void);

#endif
