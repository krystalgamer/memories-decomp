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

s32 DuelEffect_MarkInitialized(void);
void func_80024E4C(void);

#endif
