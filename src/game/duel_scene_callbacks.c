#include "../types.h"
#include "duel_scene_callbacks.h"
#include "duel_scene_state.h"
#include "../unmatched.h"
#include "debug_effect_screen.h"
#include "duel_draw_resolution.h"
#include "duel_phase_entry.h"
#include "duel_scene_resume.h"
#include "duel_scene_card_use.h"
#include "duel_scene_turn_switch.h"
#include "duel_result_outro.h"

/* Initialized data at 0x80090998: the duel scene's phase callback table.
 *
 * func_80024200 (src/candidates/func_80024200.c) materializes this address
 * itself and
 * calls `gDuel_apfnSceneStateHandler[gDuel_wSceneStateFlags &
 * DUEL_SCENE_PHASE_MASK]()`, so the table is reached only through the
 * low four bits of that state word.
 *
 * It is written here rather than resolved out of the blob at 0x800908A0
 * because every entry is a function this tree already names. Matching
 * callbacks use their subsystem headers. The unmatched ones are
 * declared in unmatched.h rather than here: this file only takes their
 * addresses, but that is the same thing the entries already in that header
 * do, so a local declaration would just be a second place for them to be
 * spelled.
 *
 * The table is fifteen entries and the mask permits sixteen. Index 15 would
 * read the first word of duel_terrain_boost, which begins immediately after
 * it. That is recorded rather than corrected: nothing here establishes that
 * the game ever produces index 15, and widening the array would change the
 * bytes. */

void (*gDuel_apfnSceneStateHandler[])(void) = {
    DuelScene_UpdateEffectPreview,
    DuelScene_UpdateStartup,
    DuelScene_UpdateDrawPhase,
    DuelScene_UpdateDrawResolution,
    DuelScene_UpdateHandActions,
    DuelScene_UpdateFieldActions,
    DuelScene_UpdateCardUse,
    DuelScene_UpdateCardPlacement,
    func_8001B170,
    DuelScene_UpdateBattle,
    DuelScene_UpdateTurnSwitch,
    DuelScene_UpdateResume,
    DuelScene_UpdateResultOutro,
    DuelScene_UpdateResultRewards,
    DuelScene_UpdateExodiaResult,
};
