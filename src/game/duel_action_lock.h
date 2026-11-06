#ifndef YUGIOH_GAME_DUEL_ACTION_LOCK_H
#define YUGIOH_GAME_DUEL_ACTION_LOCK_H

#include "../types.h"

/* The card-effect flag word. duel_action_lock.c owns the initialization bit:
 * DuelEffect_MarkInitialized sets bit 0x80 the first time it is asked and
 * DuelEffect_ClearCardEffect clears the whole word.
 * DuelEffect_StartCardEffect starts an effect by writing
 * DUEL_CARD_EFFECT_FLAG_ACTIVE, or ACTIVE | SECOND_HANDLER
 * (card_constants.h). Effect handlers set and test the bits below 0x80 as
 * they step. Retail reaches it gp-relative at every site, 23 lhu and 34 sh,
 * so u16 is the width and sign the accesses give it. */
extern u16 gDuel_wCardEffectFlags;

/* The two values DuelEffect_StartCardEffect stores before the flag word.
 * gDuel_wEffectCardID is its first parameter, stored unmodified once it falls
 * in one of the two DUEL_EFFECT_*_CARD_ID blocks or equals
 * DUEL_DARK_MAGIC_RITUAL_CARD_ID. The handlers compare it against
 * DUEL_DRAGON_CAPTURE_JAR_CARD_ID and DUEL_SPELLBINDING_CIRCLE_CARD_ID,
 * subtract DUEL_DIRECT_DAMAGE_FIRST_CARD_ID from it, and pass it to
 * Duel_CheckRitual as ritualId. gDuel_sCardEffectIndex is the normalized
 * table index consumed by gDuelEffect_abGroupByEffectId.
 *
 * Retail reads gDuel_wEffectCardID with lh at seven sites and
 * gDuel_sCardEffectIndex with lh at one, all gp-relative; the one lhu of the
 * card ID is in the matching DuelEffect_ApplyRitual source.
 * DuelEffect_ApplyTerrain reads its low byte explicitly, which is retail's
 * lbu. */
extern s16 gDuel_sCardEffectIndex;
extern s16 gDuel_wEffectCardID;

/* Two adjacent halfwords in the North American and Japanese small data,
 * separate variables in PAL (whose action substate sits elsewhere).
 *
 * gDuel_wActionSubstate is the packed action substate of
 * DuelScene_UpdateHandActions and DuelScene_UpdateFieldActions.
 * gDuel_sEffectWorkCounter is the field-sweep and countdown counter: retail
 * reaches it gp-relative at 23 sites (7 lh, 5 lhu, 11 sh, three in
 * DuelEffect_ApplyRitual); the lhu is where the source writes
 * `*(u16 *)&gDuel_sEffectWorkCounter` for the +1 or -1. DuelEffect_ApplyStatPenalty
 * runs it down from 0x10; the other three reset it to 0 or -1, advance it and
 * index gDuel_abFieldGridCardRecordIndices by it. Both were once one
 * `s16 [2]` array plus linker aliases; the scalars build identically. */
extern u16 gDuel_wActionSubstate;
extern s16 gDuel_sEffectWorkCounter;

/* The halfword immediately past the counter.
 *
 * It is the trap presentation's mode: Duel_UpdateTrapPresentation in
 * duel_battle_stats.c sequences on `gDuel_wTrapPresentationMode & 0xF` through four modes
 * and stores 1, 2 and 3 itself, and duel_card_effects.c clears it to 0 at the
 * two points the effect finishes. Four further writers are still assembly,
 * all `sh $zero`. */
extern u16 gDuel_wTrapPresentationMode;

s32 DuelEffect_MarkInitialized(void);
void DuelEffect_ClearCardEffect(void);

#endif
