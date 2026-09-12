#ifndef MEMORIES_DECOMP_DUEL_MAGIC_EFFECT_DISPATCH_H
#define MEMORIES_DECOMP_DUEL_MAGIC_EFFECT_DISPATCH_H

#include "../types.h"

/* Card-effect activation, dispatch, and one handler the dispatcher can reach.
 *
 * func_80026B34 is the dispatcher: while DUEL_CARD_EFFECT_FLAG_ACTIVE is set in
 * D_8009B220 it indexes gDuelEffect_abGroupByEffectId by the current effect id,
 * picks the second of the group's two handlers when
 * DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER is set, and calls it -- then returns
 * D_8009B220, so a caller reads "still running" straight out of the flags word
 * the handler just updated. func_80024200 (src/candidates/func_80024200.c)
 * tests it exactly that way.
 *
 * func_80026BA4 maps the presented card ID to the effect ID the dispatcher
 * consumes, records the original card ID, and raises the active/handler flags.
 * DuelEffect_ApplyHarpiesFeatherDuster is its own group handler. */
void DuelEffect_ApplyHarpiesFeatherDuster(void);
s32 func_80026B34(void);
void func_80026BA4(s32 value, s32 flag);

#endif
