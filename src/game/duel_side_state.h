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

/* Each side's pending effect object, one func_8002C604 return per side.
 * func_8001825C creates one for every side whose field_19 is set and
 * writes its +0x1A and +0x1C; func_80025F3C stores the object it also
 * hands to D_8009B17C into the other side's slot; func_8001898C writes +0x1A
 * and clears the current side's slot once its pending counter runs out.
 * u8 * is func_8002C604's return type, and duel_field_effect_steps.c, which
 * reads the object through its own Object view (field_1A at 0x1A, flags at
 * 0x1C, the same offsets the byte views write), casts at the store the way
 * it already does for D_8009B17C. Two pointers, eight bytes to D_8009B1F8;
 * retail reaches the array gp-relative in all three functions, so the sized
 * spelling stays small data everywhere. Initial value not read. */
extern u8 *D_8009B1F0[DUEL_SIDE_COUNT];

/* The card id the last search or trap selection left behind. func_80025028
 * stores 0 before its slot loop and its argument on a hit, and its own
 * comment says what a hit and a miss leave; func_8001F0D0 stores `sel +
 * DUEL_ATTACK_TRAP_FIRST_CARD_ID` in one arm and `v` under its `hit:` label;
 * func_800250C8 and func_8002525C test it against 0. func_8001F55C, still
 * assembly, stores 0 once and loads it eight times. Every retail load is lh
 * (func_800250C8.s:37, func_8002525C.s:24, and the eight in
 * func_8001F55C.s), so the halfword is signed; the stores are sh and cannot
 * say. Every access in all five listings is gp-relative, so this is the
 * plain declaration for duel_card_effects.c, duel_trap_resolution.c and
 * func_80025028.c alike, and the `u16` one of them used to write was the
 * same halfword under the other sign. Initial value not read. */
extern s16 D_8009B22A;

/* The halfword func_8002DC38 passes, as `(u8 *)&D_8009B230`, to
 * MainMenu_StartValueSetup's `toggle` parameter (entrypoints.h declares
 * `u8 *toggle`), beside the two halfwords below; func_800175A0, when both
 * D_8009B360 and gDuel_bOpponentID
 * are negative, copies `*(u8 *)&D_8009B230` into field_1F of both
 * D_800E9FF0 records; Main_Init stores 1 into it. Every retail access is a
 * byte or an address (lbu func_800175A0.s:65, sb func_80012B50.s:39, the
 * lui/addiu pair func_8002DC38.s:14-15), so the listings do not say how
 * wide the object is; two of the three units declare the u16 and reach the
 * byte through a `(u8 *)` cast, and overlays/main_menu/README.md describes the
 * option as the low byte of this wider view, so that is the declaration
 * kept here and main_init.c now writes the byte through the same cast.
 * Initial value not read.
 *
 * duel_state_init.c reaches it gp-relative and takes the plain halfword;
 * func_8002DC38.c and main_init.c reach it through %hi/%lo and define the
 * .data arm. */
#ifdef D_8009B230_IN_DATA
extern u16 D_8009B230 __attribute__((section(".data")));
#else
extern u16 D_8009B230;
#endif

/* The two halfwords func_8002DC38 stores DUEL_STARTING_LIFE_POINTS into
 * (D_8009B236 first, then D_8009B234) and passes to MainMenu_StartValueSetup
 * as `first` and `second`, both declared `u16 *` in entrypoints.h -- that
 * signature is
 * what fixes the type, and func_800175A0's lhu of each
 * (func_800175A0.s:11-12) agrees. func_800175A0 copies them into `sp[0]`
 * and `sp[1]` of its `u16 sp[DUEL_SIDE_COUNT]` when gDuel_bOpponentID is
 * negative. No other C
 * unit touches them. Initial value not read.
 *
 * duel_state_init.c reaches both gp-relative and takes the plain
 * declarations; func_8002DC38.c reaches both through %hi/%lo
 * (func_8002DC38.s:10-13 for the addresses, 19-22 for the stores) and
 * defines the two .data arms, one control each. */
#ifdef D_8009B234_IN_DATA
extern u16 D_8009B234 __attribute__((section(".data")));
#else
extern u16 D_8009B234;
#endif
#ifdef D_8009B236_IN_DATA
extern u16 D_8009B236 __attribute__((section(".data")));
#else
extern u16 D_8009B236;
#endif

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

/* The outro's own copy of the winning side, and the only one of these that
 * carries a "not set" state. func_800179F4 clears it to -1 when the duel
 * starts, func_80020F4C writes the winner alongside D_8009B362 when the
 * result sequence begins, and duel_scene_update.c reads it as an override:
 * it takes D_8009B1D5 and replaces it with this value only when the test
 * `D_8009B238 >= 0` passes. That signed test is why it is s8 and not u8 --
 * the sentinel is the whole point of the field. */
extern s8 D_8009B238;

/* The duel's outcome as a plain 0/1 byte. func_80020F4C sets it in the
 * same statement group as D_8009B238: 0 when gDuel_bWinnerSide is 0, 1
 * otherwise. Main_RunDuel indexes its two campaign continuations with it
 * (`table[D_8009B362 * 2]`), and the free_duel overlay's FreeDuel_Init
 * steps from the wins halfword to the losses halfword of the duelist
 * record when it is 1. Every retail access is a byte: sb through $at in
 * func_80020F4C and lui/lbu in Main_RunDuel, both in units that reach
 * other symbols through $gp, so func_80020F4C.c and
 * main_run_duel_and_library.c define the .data arm below; free_duel/init.c
 * (-G0) takes the plain byte. The `u8 [9]` main_run_duel_and_library.c
 * used to declare reached the same form; as the note on D_8009B360 says,
 * such a size is a threshold, not a length. */
#ifdef D_8009B362_IN_DATA
extern u8 D_8009B362 __attribute__((section(".data")));
#else
extern u8 D_8009B362;
#endif

/* A byte Main_RunDuel reads into a local after File_WaitForTransfers and,
 * past its nop barrier, copies into D_8009B26C. Four functions store it:
 * func_80030F40 stores 0 (then calls func_80024DC8), func_8002DC38 stores
 * 8 when MainMenu_UpdateValueSetup returned 1, Text_StartCampaignDuel
 * stores 2 at the end of its setup, and the free_duel overlay's
 * FreeDuel_UpdateScreen stores 6 after its func_80024DC8 call. Every
 * retail access is a byte (sb, and Main_RunDuel's lbu); no C unit defines
 * it. Initial value not read.
 *
 * Retail reaches it through %hi/%lo at every site and never through $gp,
 * so frontend_scene_states.c, func_8002DC38.c and
 * main_run_duel_and_library.c -- units that reach other symbols through
 * $gp -- define the .data arm below; text_start_campaign_duel.c and the
 * overlay's screen_runtime.c compile with nothing in small data and take
 * the plain byte. The `u8 [9]` and `u8 []` two of them used to declare
 * were accessed only at [0]; as the notes on D_8009B360 and D_8009B362
 * say, such a size is a threshold, not a length. */
#ifdef D_8009B368_IN_DATA
extern u8 D_8009B368 __attribute__((section(".data")));
#else
extern u8 D_8009B368;
#endif

/* A byte the duel setup clears and func_800179F4 tests against 1.
 * func_80024DC8 stores 0 beside gDuel_bOpponentID, D_8009B370, D_8009B372
 * and gDuel_bTerrain; Text_StartCampaignDuel stores 0 after its own copy
 * of that setup; func_80018FEC (a tracked candidate) stores 1;
 * func_8001F55C (still assembly) stores it too. func_800179F4 skips two
 * blocks when it is 1, and Main_RunDuel clears D_8009B26E only when it is
 * 0 and gDuel_bOpponentID is not negative. Every retail access is sb or
 * lbu and every declarer said u8. Initial value not read.
 *
 * Retail reaches it through %hi/%lo at every site and never through $gp.
 * func_800179F4.c and main_run_duel_and_library.c reach other symbols
 * through $gp, so they define the .data arm below; func_80024DC8.c and
 * text_start_campaign_duel.c compile with nothing in small data and take
 * the plain byte. src/candidates/func_80018FEC.c keeps its own .data
 * declaration: it does not include this header and its object is
 * fingerprinted in candidates.json. */
#ifdef D_8009B369_IN_DATA
extern u8 D_8009B369 __attribute__((section(".data")));
#else
extern u8 D_8009B369;
#endif

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

/* Points D_8009B1C8 and D_8009B22C at the side D_8009B1D5 selects, then
 * clears the rest of the per-duel state -- roughly thirty scalars, several of
 * them written more than once.
 *
 * Declared here because its first two statements are the assignment this
 * header already describes: the note on D_8009B1C8 says four translation
 * units assign it exactly `&D_800E9FF0[D_8009B1D5]` on a turn change, and
 * this is one of them. func_800179F4.c is the only caller and had the only
 * declaration. */
void func_800175A0(void);

#endif
