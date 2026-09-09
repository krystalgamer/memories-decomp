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

/* The signed byte just below gDuel_bOpponentID. func_80024DC8 stores its
 * first argument here and its second into gDuel_bOpponentID;
 * Text_StartCampaignDuel resets it to -1 before it sets
 * gDuel_bOpponentID; duel_state_init.c and func_80019CC8 test it
 * negative, the latter together with D_8009B1D5 == 0 and a non-negative
 * gDuel_bOpponentID. Every retail access is lb or sb and none is
 * gp-relative, so the -G0 units (func_80024DC8.c, text_start_campaign_duel.c)
 * take the plain scalar and the -G8 units (duel_state_init.c,
 * func_80019CC8.c) take _IN_DATA, out of small data at the compiler with
 * the byte's true width; the two _IN_DATA arms are each justified by a
 * control build recorded in the PR that added this block. The `s8 [9]`
 * one of them used to declare reached the same form and in doing so
 * spanned 0x8009B360..0x8009B368, eight named addresses; as
 * duel_terrain_boost.h says of the `[8]` on gDuel_bTerrain, such a size
 * is a threshold, not a length.
 *
 * Retail also reaches this address indexed by the side selector in
 * functions not yet in C (lui/addiu, addu with D_8009B1D5, lb -- e.g.
 * func_8001B170.s:19-23); what that says about the object's extent is
 * not established. This declaration claims one byte because that is all
 * the four C units touch. */
#ifdef D_8009B360_IN_DATA
extern s8 D_8009B360 __attribute__((section(".data")));
#else
extern s8 D_8009B360;
#endif

/* Which side won, and therefore which of the two records above the result
 * code applies to. duel_draw_resolution.c sets it as `D_8009B1D5 ^ 1`, the
 * side that is not the one the selector documented above points at, and both
 * it and duel_rewards.c immediately use it to index D_800E9FF0. That is what
 * puts it in this header rather than beside the screen that displays the
 * outcome: it is a side selector, and it is read as one. */
extern u8 gDuel_bWinnerSide;

/* The halfword the duel hands to SD_BGMPlay: func_8001825C and func_80018608
 * read it for that call, func_80024DC8 stores 0x7270, func_80030F40 stores
 * 0x71D0 and Text_StartCampaignDuel stores func_80036D3C's result. lhu/sh
 * everywhere, two bytes wide (gFreeDuel_bTargetColumn is at 0x8009B36C).
 * Retail reaches it through %hi/%lo at all five sites and never through $gp,
 * so duel_phase_entry.c, func_8001825C.c and frontend_scene_states.c define
 * the .data arm below; func_80024DC8.c and text_start_campaign_duel.c
 * compile with nothing in small data and take the plain arm. */
#ifdef D_8009B36A_IN_DATA
extern u16 D_8009B36A __attribute__((section(".data")));
#else
extern u16 D_8009B36A;
#endif

#endif
