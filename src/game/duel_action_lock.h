#ifndef YUGIOH_GAME_DUEL_ACTION_LOCK_H
#define YUGIOH_GAME_DUEL_ACTION_LOCK_H

#include "../types.h"

/* The card-effect flag word. duel_action_lock.c owns the lock half of it:
 * DuelEffect_MarkInitialized sets bit 0x80 the first time it is asked and
 * func_80024E4C clears the whole word. func_80026BA4 starts an effect by
 * writing DUEL_CARD_EFFECT_FLAG_ACTIVE, or ACTIVE | SECOND_HANDLER
 * (card_constants.h), and the effect handlers in duel_card_effects.c and
 * its neighbours set and test the bits below 0x80 as they step. Retail
 * reaches it gp-relative at every site, 23 lhu and 34 sh, so u16 is the
 * width and sign the accesses give it. */
extern u16 D_8009B220;

/* The two values func_80026BA4 stores just before that flag word. D_8009B1D2
 * is its first parameter, stored unmodified once it falls in one of the two
 * DUEL_EFFECT_*_CARD_ID blocks or equals DUEL_DARK_MAGIC_RITUAL_CARD_ID; the
 * handlers compare it against
 * DUEL_DRAGON_CAPTURE_JAR_CARD_ID and DUEL_SPELLBINDING_CIRCLE_CARD_ID,
 * subtract DUEL_DIRECT_DAMAGE_FIRST_CARD_ID from it, and func_8002622C
 * passes it to Duel_CheckRitual as ritualId. D_8009B1A8 is the index
 * func_80026BA4 computes from that value, which duel_magic_effect_dispatch.c
 * uses as indices[D_8009B1A8] into gDuelEffect_abGroupByEffectId. Retail
 * reads D_8009B1D2 with lh at seven sites and D_8009B1A8 with lh at one,
 * all gp-relative; the one lhu of D_8009B1D2 is in func_800262D4, still
 * assembly. The u16 spellings were only in the writer, where a store shows
 * no sign. func_80024E58.c reads the low byte of D_8009B1D2 through an
 * explicit (u8 *) cast, which is retail's lbu and stays there. */
extern s16 D_8009B1A8;
extern s16 D_8009B1D2;

s32 DuelEffect_MarkInitialized(void);
void func_80024E4C(void);

#endif
