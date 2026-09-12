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
 * card ID is in DuelEffect_ApplyRitual, still assembly. The terrain candidate
 * reads its low byte explicitly, which is retail's lbu. */
extern s16 gDuel_sCardEffectIndex;
extern s16 gDuel_wEffectCardID;

/* Two halfwords; the field sweeps in duel_card_effects.c and
 * DuelEffect_ApplyCursebreaker all reach [1], which retail addresses
 * as %gp_rel(D_8009B20C + 0x2): 23 sites, 7 lh, 5 lhu and 11 sh, three of
 * them in DuelEffect_ApplyRitual, still assembly. In the four matched TUs the lhu
 * is where the source writes *(u16 *)&D_8009B20C[1] for the +1 or -1.
 * DuelEffect_ApplyStatPenalty runs it down from 0x10; the other three reset it
 * to 0 or
 * -1, advance it and index D_800907D8 by it. [0] is written and read only
 * by func_8001BD88 and func_8001D670, still assembly, 30 sites at +0;
 * whether it belongs to the same object is not established. Four bytes,
 * the extent up to D_8009B210, stays gp-relative at every threshold in
 * play; with [4] shared instead, an assembler -G4
 * (DuelEffect_ApplyRaigeki's until
 * #3859 found that threshold inert) takes the eight bytes out of small data
 * and the tree does not link. */
extern s16 D_8009B20C[2];

/* The halfword immediately past that pair, at D_8009B20C + 4 -- the extent
 * the note above measures against. It stays a separate declaration for the
 * reason recorded there: spelling the pair as [4] to cover these four bytes
 * takes eight bytes out of small data under an assembler -G4, which
 * DuelEffect_ApplyRaigeki was built at until #3859, and the tree stopped
 * linking, so
 * this address got its own scalar.
 *
 * It is the trap presentation's mode: func_8001F364 in duel_trap_resolution.c
 * sequences on `D_8009B210 & 0xF` through four modes and stores 1, 2 and 3
 * itself, and duel_card_effects.c clears it to 0 at the two points the effect
 * finishes. Four further writers are still assembly, all `sh $zero`.
 *
 * Unsigned: every read in the image is lhu, and all nine accesses are
 * gp-relative, so no unit needs a .data arm. */
extern u16 D_8009B210;

s32 DuelEffect_MarkInitialized(void);
void DuelEffect_ClearCardEffect(void);

#endif
