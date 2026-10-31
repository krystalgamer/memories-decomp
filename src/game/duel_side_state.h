#ifndef MEMORIES_DECOMP_DUEL_SIDE_STATE_H
#define MEMORIES_DECOMP_DUEL_SIDE_STATE_H

#include "../types.h"
#include "duel_grid.h"
#include "card_constants.h"
#include "duel_card_effects.h"
#include "duel_effect_request.h"

/* The two 0x20-byte per-side duel records at gDuel_aSideStates, one per duellist.
 * gDuel_bActiveSide selects the side, and code that switches turns writes
 * gDuel_pActiveSideState = (u8 *)gDuel_aSideStates + gDuel_bActiveSide * 0x20, so gDuel_pActiveSideState always
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

/* The rank statistics stored at the front of each side record. The named
 * bytes are the counters consumed by Duel_CalcRankScore; face_down_plays
 * counts normal single-card face-down commitments, not fusion results.
 * Unresolved display rows retain their offsets. */
typedef struct {
    s8 result_adjustment;
    u8 turns_taken;
    u8 effective_attacks;
    u8 defensive_wins;
    u8 face_down_plays;
    u8 pure_magic_used;
    u8 traps_triggered;
    u8 field_07;
    u8 fusions_initiated;
    u8 equips_used;
    u8 field_0A;
    u8 field_0B;
    u8 field_0C;
} DuelRankStatistics;

typedef struct {
    DuelRankStatistics rank;
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
    /* Index of the next deck card to draw. The result screen labels this
     * statistic "cards used", but refill draws advance it before play. */
    s8 deck_draw_cursor;
    s8 swords_turns_remaining;
    /* duel_draw_resolution.c's own view of this record calls +0x1A
     * hand[HAND_SIZE] and Duel_HasAllExodiaPieces copies five entries out of
     * it, so the six bytes are five hand slots and one separate byte. */
    s8 hand[HAND_SIZE];
    /* Per-side card presentation mode copied from the two-player setup
     * option; positive and negative values select distinct card markers. */
    s8 card_view_mode;
} DuelSideState;

#define DUEL_SIDE_STATE_OFFSET(member) ((u32)&(((DuelSideState *)0)->member))

typedef char DuelSideState_size_must_be_0x20[
    sizeof(DuelSideState) == 0x20 ? 1 : -1
];
typedef char DuelSideState_hand_must_be_at_0x1A[
    DUEL_SIDE_STATE_OFFSET(hand) == 0x1A ? 1 : -1
];
typedef char DuelRankStatistics_size_must_be_0x0D[
    sizeof(DuelRankStatistics) == 0x0D ? 1 : -1
];
typedef char DuelSideState_deck_draw_cursor_must_be_at_0x18[
    DUEL_SIDE_STATE_OFFSET(deck_draw_cursor) == 0x18 ? 1 : -1
];
typedef char DuelSideState_swords_turns_remaining_must_be_at_0x19[
    DUEL_SIDE_STATE_OFFSET(swords_turns_remaining) == 0x19 ? 1 : -1
];
typedef char DuelSideState_card_view_mode_must_be_at_0x1F[
    DUEL_SIDE_STATE_OFFSET(card_view_mode) == 0x1F ? 1 : -1
];

/* The side selector: 0 or 1, and the index behind both cursors this header
 * and duel_grid.h describe. Thirty-nine private declarations before this. */
#ifdef D_8009B1D5_IS_AGGREGATE
extern u8 gDuel_bActiveSide[];
#elif defined(D_8009B1D5_IS_ABSOLUTE_SCALAR)
extern u8 gDuel_bActiveSide __attribute__((section(".data")));
#else
extern u8 gDuel_bActiveSide;
#endif

/* A volatile view of the same byte. Swords of Revealing Light and
 * Cursebreaker (duel_field_effects.c) reload it at every use, while the rest
 * of that unit reads the plain declaration above. */
#define DUEL_SIDE_SELECTOR_VOLATILE (*(volatile u8 *)&gDuel_bActiveSide)

/* Destination slot and signed stat adjustment used by the card-move
 * presentation sequence (DuelScene_UpdateCardMove). The adjustment is the
 * pending equip bonus: DuelScene_UpdateCardPlacement adds 500 per equip
 * (a second 500 for card 0x291) and clears it for a fusion. Both are reached
 * through small data. */
extern u8 gDuel_bCardMoveSlot;
extern s16 gDuel_wPendingEquipBonus;

extern DuelSideState gDuel_aSideStates[DUEL_SIDE_COUNT];
/* Always &gDuel_aSideStates[gDuel_bActiveSide]: four translation units assign it exactly
 * that on a turn change. */
extern DuelSideState *G32 gDuel_pActiveSideState;

/* Per-side request-pool entries for Swords of Revealing Light. Apply and
 * replay reconstruction publish NONBLOCKING effect 0x15 requests; draw entry
 * writes the current side's removal command before clearing its pointer. */
extern DuelEffectRequest
    *G32 gDuel_apSwordsEffectObjects[DUEL_SIDE_COUNT];

/* The flag the field-action step raises for the battle step: set when Square
 * (PAD_BUTTON_SQUARE) is among the buttons that commit the field-action confirm,
 * cleared otherwise and by two battle-step paths, one of them a selected
 * attack trap (Duel_SelectAttackTrap). When it is set, the
 * battle step moves to sub-step 5, which fades out and enters the animated
 * battle main mode, instead of sub-step 7.
 * DuelScene_UpdateFieldActions stores 0 twice and 1 once
 * (src/game/duel_scene_field_actions.c), DuelScene_UpdateBattle stores 0 twice and
 * is the only reader, testing it against 0
 * (src/game/duel_scene_battle.c). That single load is lbu and the five
 * stores are sb and cannot say, so the byte is unsigned -- which settles a
 * disagreement the two units carried, one declaring it u8 and the other s8
 * while every use is a store of 0 or 1 or a test against 0, so neither
 * spelling showed in the object code. No other C source in this tree mentions
 * the symbol. Every access in both listings is gp-relative, so this is the
 * plain declaration. Initial value not read. */
extern u8 gDuel_bAnimatedBattleRequested;

/* The card id the last search or trap selection left behind.
 * Duel_SelectTrapByCardId stores 0 before its slot loop and its argument on a
 * hit, and its own comment says what a hit and a miss leave;
 * Duel_SelectAttackTrap stores `sel + DUEL_ATTACK_TRAP_FIRST_CARD_ID` in one
 * arm and `v` under its `hit:` label;
 * DuelEffect_ApplyLifePointRecovery and DuelEffect_ApplyDirectDamage test it
 * against 0. DuelScene_UpdateBattle (src/game/duel_scene_battle.c) stores 0
 * once and loads it eight times. Every retail load is lh
 * (DuelEffect_ApplyLifePointRecovery, DuelEffect_ApplyDirectDamage, and the
 * eight in DuelScene_UpdateBattle), so the halfword is signed; the stores
 * are sh and cannot
 * say. Every access in all five listings is gp-relative, so this is the
 * plain declaration for duel_card_effects.c, duel_battle_stats.c and
 * duel_select_trap_by_card_id.c alike, and the `u16` one of them used to write was the same
 * halfword under the other sign. Initial value not read. */
extern s16 gDuel_wSelectedTrapCardId;

/* Assigned in two units and read by no C statement: Duel_InitScene writes
 * `gDuel_pActiveFieldGrid = &gDuel_abFieldGridCardRecordIndices[gDuel_bActiveSide * 20];`
 * (src/game/duel_init_scene.c:119-120) and
 * DuelScene_UpdateTurnSwitch writes `gDuel_pActiveFieldGrid = gDuel_abFieldGridCardRecordIndices + gDuel_bActiveSide *
 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;` (duel_scene_battle.c); no other listing
 * mentions the symbol. u8 * because gDuel_abFieldGridCardRecordIndices is `extern u8 gDuel_abFieldGridCardRecordIndices[]`
 * under the arm both units take (duel_grid.h:63) and 20 is
 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT (duel_grid.h:9); DuelScene_UpdateTurnSwitch's source used to
 * write `void *` for the same address, and no prototype takes &gDuel_pActiveFieldGrid.
 * Both stores are gp-relative sw (Duel_InitScene.s:128, DuelScene_UpdateTurnSwitch.s:46),
 * so this is the plain declaration. Initial value not read. */
extern u8 *G32 gDuel_pActiveFieldGrid;

/* The halfword Main_RunTwoPlayerDuelSetup passes, as `(u8 *)&gDuel_wTwoPlayerCardViewMode`, to
 * MainMenu_StartValueSetup's `toggle` parameter (value_setup.h declares
 * `u8 *toggle`), beside the two halfwords below; Duel_InitSideStates, when both
 * D_8009B360 and gDuel_bOpponentID
 * are negative, copies `*(u8 *)&gDuel_wTwoPlayerCardViewMode` into card_view_mode of both
 * gDuel_aSideStates records; Main_Init stores 1 into it. Every retail access is a
 * byte or an address (lbu Duel_InitSideStates.s:65, sb func_80012B50.s:39, the
 * lui/addiu pair at 0x8002DC60), so the listings do not say how
 * wide the object is; two of the three units declare the u16 and reach the
 * byte through a `(u8 *)` cast, and overlays/main_menu/README.md describes the
 * option as the low byte of this wider view, so that is the declaration
 * kept here and Main_Init (src/game/main_init.c) now writes the
 * byte through the same cast.
 * Initial value not read.
 *
 * duel_state_init.c reaches it gp-relative and takes the plain halfword;
 * main_mode_runners.c and src/game/main_init.c reach it through
 * %hi/%lo and define the .data arm. */
#ifdef D_8009B230_IN_DATA
extern u16 gDuel_wTwoPlayerCardViewMode __attribute__((section(".data")));
#else
extern u16 gDuel_wTwoPlayerCardViewMode;
#endif

/* The two halfwords Main_RunTwoPlayerDuelSetup stores DUEL_STARTING_LIFE_POINTS into
 * (gDuel_wTwoPlayerSide1LifePoints first, then gDuel_wTwoPlayerSide0LifePoints) and passes to MainMenu_StartValueSetup
 * as `first` and `second`, both declared `u16 *` in value_setup.h -- that
 * signature is
 * what fixes the type, and Duel_InitSideStates's lhu of each
 * (Duel_InitSideStates.s:11-12) agrees. Duel_InitSideStates copies them into `sp[0]`
 * and `sp[1]` of its `u16 sp[DUEL_SIDE_COUNT]` when gDuel_bOpponentID is
 * negative. No other C
 * unit touches them. Initial value not read.
 *
 * duel_state_init.c reaches both gp-relative and takes the plain
 * declarations; main_mode_runners.c reaches both through %hi/%lo
 * defines the two .data arms, one control each. */
#ifdef D_8009B234_IN_DATA
extern u16 gDuel_wTwoPlayerSide0LifePoints __attribute__((section(".data")));
#else
extern u16 gDuel_wTwoPlayerSide0LifePoints;
#endif
#ifdef D_8009B236_IN_DATA
extern u16 gDuel_wTwoPlayerSide1LifePoints __attribute__((section(".data")));
#else
extern u16 gDuel_wTwoPlayerSide1LifePoints;
#endif

/* The signed byte just below gDuel_bOpponentID. Main_StartDuel stores its
 * first argument here and its second into gDuel_bOpponentID;
 * Text_StartCampaignDuel resets it to -1 before it sets
 * gDuel_bOpponentID; duel_state_init.c and Duel_MarkCreatedCardKnown test it
 * negative, the latter together with gDuel_bActiveSide == 0 and a non-negative
 * gDuel_bOpponentID. Every retail access is lb or sb and none is
 * gp-relative, so the -G0 units (main_start_duel.c,
 * text_start_campaign_duel.c) take the plain scalar and the -G8 units
 * (duel_state_init.c, duel_mark_created_card_known.c) take _IN_DATA, out of small data at
 * the compiler with the byte's true width; the two _IN_DATA arms are each
 * justified by a control build recorded in the PR that added this block. The
 * `s8 [9]` one of them used to declare reached the same form and in doing so
 * spanned 0x8009B360..0x8009B368, eight named addresses; as
 * duel_terrain_boost.h says of the `[8]` on gDuel_bTerrain, such a size
 * is a threshold, not a length.
 *
 * DuelScene_UpdateCardMove indexes the two adjacent identities by the 0/1 side
 * selector: D_8009B360 and gDuel_bOpponentID at the following byte.
 * Its array view describes those two bytes without changing the scalar
 * view used by the existing setup code. */
#ifdef D_8009B360_AS_SIDE_ARRAY
extern s8 D_8009B360[DUEL_SIDE_COUNT] __attribute__((section(".data")));
#elif defined(D_8009B360_IN_DATA)
extern s8 D_8009B360 __attribute__((section(".data")));
#else
extern s8 D_8009B360;
#endif

/* Which side won, and therefore which of the two records above the result
 * code applies to. duel_draw_resolution.c sets it as `gDuel_bActiveSide ^ 1`, the
 * side that is not the one the selector documented above points at, and both
 * it and duel_result_runtime.c immediately use it to index gDuel_aSideStates. That is what
 * puts it in this header rather than beside the screen that displays the
 * outcome: it is a side selector, and it is read as one. */
extern u8 gDuel_bWinnerSide;

/* The outro's own copy of the winning side, and the only one of these that
 * carries a "not set" state. Duel_InitScene clears it to -1 when the duel
 * starts, DuelScene_UpdateResultOutro writes the winner alongside gDuel_bPlayerLost when the
 * result sequence begins, and duel_scene_update.c reads it as an override:
 * it takes gDuel_bActiveSide and replaces it with this value only when the test
 * `gDuel_bOutroWinnerSide >= 0` passes. That signed test is why it is s8 and not u8 --
 * the sentinel is the whole point of the field. */
extern s8 gDuel_bOutroWinnerSide;

/* The duel's outcome as a plain 0/1 byte. DuelScene_UpdateResultOutro sets it in the
 * same statement group as gDuel_bOutroWinnerSide: 0 when gDuel_bWinnerSide is 0, 1
 * otherwise. Main_RunDuel indexes its two campaign continuations with it
 * (`table[gDuel_bPlayerLost * 2]`), and the free_duel overlay's FreeDuel_Init
 * steps from the wins halfword to the losses halfword of the duelist
 * record when it is 1. Every retail access is a byte: sb through $at in
 * DuelScene_UpdateResultOutro and lui/lbu in Main_RunDuel, both in units that reach
 * other symbols through $gp, so duel_result_runtime.c and
 * src/game/main_run_duel_and_library.c define the .data arm below;
 * free_duel/screen_runtime.c (-G0) takes the plain byte. The `u8 [9]`
 * main_run_duel_and_library.c used to declare reached the same form; as the
 * note on D_8009B360 says, such a size is a threshold, not a length. */
#ifdef D_8009B362_IN_DATA
extern u8 gDuel_bPlayerLost __attribute__((section(".data")));
#else
extern u8 gDuel_bPlayerLost;
#endif

/* A byte Main_RunDuel reads into a local after File_WaitForTransfers and,
 * past its nop barrier, copies into D_8009B26C. Four functions store it:
 * DebugMenu_EnterDuel stores 0 (then calls Main_StartDuel),
 * Main_RunTwoPlayerDuelSetup stores
 * 8 when MainMenu_UpdateValueSetup returned 1, Text_StartCampaignDuel
 * stores 2 at the end of its setup, and the free_duel overlay's
 * FreeDuel_UpdateScreen stores 6 after its Main_StartDuel call. Every
 * retail access is a byte (sb, and Main_RunDuel's lbu); no C unit defines
 * it. Initial value not read.
 *
 * Retail reaches it through %hi/%lo at every site and never through $gp,
 * so frontend_scene_states.c, main_mode_runners.c and
 * src/game/main_run_duel_and_library.c -- units that reach other symbols through
 * $gp -- define the .data arm below; text_start_campaign_duel.c and the
 * overlay's screen_runtime.c take the plain byte. The `u8 [9]` and `u8 []`
 * two of them used to declare
 * were accessed only at [0]; as the notes on D_8009B360 and gDuel_bPlayerLost
 * say, such a size is a threshold, not a length. */
#ifdef D_8009B368_IN_DATA
extern u8 D_8009B368 __attribute__((section(".data")));
#else
extern u8 D_8009B368;
#endif

/* A byte the duel setup clears and Duel_InitScene tests against 1.
 * Main_StartDuel stores 0 beside gDuel_bOpponentID, D_8009B370, D_8009B372
 * and gDuel_bTerrain; Text_StartCampaignDuel stores 0 after its own copy
 * of that setup; matching DuelScene_UpdateExodiaResult stores 1;
 * DuelScene_UpdateBattle stores it too. Duel_InitScene skips two
 * blocks when it is 1, and Main_RunDuel clears gMain_bModeRunnerStep only when it is
 * 0 and gDuel_bOpponentID is not negative. Every retail access is sb or
 * lbu and every declarer said u8. Initial value not read.
 *
 * Retail reaches it through %hi/%lo at every site and never through $gp.
 * src/game/duel_init_scene.c and src/game/main_run_duel_and_library.c reach
 * other symbols through $gp, so they define the .data arm below;
 * main_start_duel.c and src/game/text_start_campaign_duel.c compile with nothing in
 * small data and take the plain byte. src/game/duel_draw_resolution.c also
 * defines the .data arm. It used to carry a private copy of this exact
 * declaration and not include this header at all; it now includes it and
 * selects the same arm, with its candidate object fingerprint unchanged by
 * the move. Which of the two reasons above puts it on this arm is not
 * re-derived here -- the spelling it already had is what is preserved. */
#ifdef D_8009B369_IN_DATA
extern u8 D_8009B369 __attribute__((section(".data")));
#else
extern u8 D_8009B369;
#endif

/* The halfword the duel hands to SD_BGMPlay: DuelScene_UpdateResume and DuelScene_UpdateStartup
 * read it for that call, Main_StartDuel stores 0x7270, DebugMenu_EnterDuel stores
 * 0x71D0 and Text_StartCampaignDuel stores TextStream_ReadU16LE's result. lhu/sh
 * everywhere, two bytes wide (gFreeDuel_bTargetColumn is at 0x8009B36C).
 * Retail reaches it through %hi/%lo at all five sites and never through $gp,
 * so duel_phase_entry.c and frontend_scene_states.c define
 * the .data arm below; main_start_duel.c and src/game/text_start_campaign_duel.c
 * compile with nothing in small data and take the plain arm. */
#ifdef GDUEL_WBGMID_IN_DATA
extern u16 gDuel_wBgmId __attribute__((section(".data")));
#else
extern u16 gDuel_wBgmId;
#endif

/* Clears the per-duel state -- roughly thirty scalars, several of them
 * written more than once -- and then, as its last statement, assigns
 * `gDuel_pActiveSideState = &gDuel_aSideStates[gDuel_bActiveSide];` (duel_state_init.c:62;
 * Duel_InitSideStates.s:85). It does not touch gDuel_pActiveFieldGrid: that store is in its
 * only caller, src/game/duel_init_scene.c:119, after the call at :95.
 *
 * Declared here because that last statement is the assignment this header
 * already describes: the note on gDuel_pActiveSideState says four translation units
 * assign it exactly `&gDuel_aSideStates[gDuel_bActiveSide]` on a turn change, and this is
 * one of them. Duel_InitScene (src/game/duel_init_scene.c) is the only
 * caller, and its old duel_init_scene.c had the only declaration. */
void Duel_InitSideStates(void);

#endif
