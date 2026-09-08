#ifndef MEMORIES_DECOMP_DUEL_SIDE_STATE_H
#define MEMORIES_DECOMP_DUEL_SIDE_STATE_H

#include "../types.h"
#include "duel_grid.h"

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
    s16 field_12;
    union DuelSideLifePoints life_points;
    s16 max_life_points;
    /* Cards taken from the deck: the status readout draws
     * DECK_SIZE - field_18 as the remaining count. */
    s8 field_18;
    s8 field_19;
    s8 field_1A[6];
} DuelSideState;

typedef char DuelSideState_size_must_be_0x20[
    sizeof(DuelSideState) == 0x20 ? 1 : -1
];

extern DuelSideState D_800E9FF0[DUEL_SIDE_COUNT];

#endif
