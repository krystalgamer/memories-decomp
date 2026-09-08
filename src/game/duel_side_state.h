#ifndef MEMORIES_DECOMP_DUEL_SIDE_STATE_H
#define MEMORIES_DECOMP_DUEL_SIDE_STATE_H

#include "../types.h"
#include "duel_grid.h"
#include "card_constants.h"

/* The two 0x20-byte per-side duel records at D_800E9FF0, one per duellist.
 * D_8009B1D5 selects the side, and code that switches turns writes
 * D_8009B1C8 = (u8 *)D_800E9FF0 + D_8009B1D5 * 0x20, so D_8009B1C8 always
 * points at one of these two records.
 */

/* Offset 0x14 is read as unsigned by the life-point code, which clamps it at
 * zero and against the maximum, and as signed by the rank scorer, which
 * copies it into a signed slot. Both loads exist in the retail image, so the
 * field is offered under both spellings rather than forcing a cast at one of
 * them.
 */
union DuelSideLifePoints {
    u16 unsigned_value;
    s16 signed_value;
};

typedef struct {
    s8 field_00;
    u8 field_01;
    u8 field_02;
    u8 field_03;
    u8 field_04;
    u8 field_05;
    u8 field_06;
    u8 field_07;
    u8 field_08;
    u8 field_09;
    u8 field_0A;
    u8 field_0B;
    u8 field_0C;
    u8 field_0D;
    s16 field_0E;
    s16 field_10;
    /* The life-point value actually drawn. Duel_UpdateLifePointDisplay steps
     * it towards life_points a little each frame, and
     * Duel_DrawLifePointsAndDeckCounts draws this field, not life_points, as
     * four digits. */
    s16 displayed_life_points;
    union DuelSideLifePoints life_points;
    s16 max_life_points;
    /* Cards taken from the deck: the status readout draws
     * DECK_SIZE - field_18 as the remaining count. */
    s8 field_18;
    s8 field_19;
    /* duel_draw_resolution.c's own view of this record calls +0x1A
     * hand[HAND_SIZE] and Duel_HasAllExodiaPieces copies five entries out of
     * it, so the six bytes are five hand slots and one separate byte. */
    s8 hand[HAND_SIZE];
    s8 field_1F;
} DuelSideState;

#define DUEL_SIDE_STATE_OFFSET(member) ((u32)&(((DuelSideState *)0)->member))

typedef char DuelSideState_size_must_be_0x20[
    sizeof(DuelSideState) == 0x20 ? 1 : -1
];
typedef char DuelSideState_hand_must_be_at_0x1A[
    DUEL_SIDE_STATE_OFFSET(hand) == 0x1A ? 1 : -1
];
typedef char DuelSideState_field_1F_must_be_at_0x1F[
    DUEL_SIDE_STATE_OFFSET(field_1F) == 0x1F ? 1 : -1
];

/* The side selector: 0 or 1, and the index behind both cursors this header
 * and duel_grid.h describe. Thirty-nine private declarations before this. */
#ifdef D_8009B1D5_IS_AGGREGATE
extern u8 D_8009B1D5[];
#elif defined(D_8009B1D5_IS_VOLATILE)
extern volatile u8 D_8009B1D5;
#else
extern u8 D_8009B1D5;
#endif

extern DuelSideState D_800E9FF0[DUEL_SIDE_COUNT];
/* Always &D_800E9FF0[D_8009B1D5]: four translation units assign it exactly
 * that on a turn change. */
extern DuelSideState *D_8009B1C8;

#endif
