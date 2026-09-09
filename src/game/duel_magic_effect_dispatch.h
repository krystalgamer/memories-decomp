#ifndef MEMORIES_DECOMP_DUEL_MAGIC_EFFECT_DISPATCH_H
#define MEMORIES_DECOMP_DUEL_MAGIC_EFFECT_DISPATCH_H

#include "../types.h"

/* The card-effect dispatcher and one of the handlers it can reach.
 *
 * func_80026B34 is the dispatcher: while DUEL_CARD_EFFECT_FLAG_ACTIVE is set in
 * D_8009B220 it indexes gDuelEffect_abGroupByEffectId by the current effect id,
 * picks the second of the group's two handlers when
 * DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER is set, and calls it -- then returns
 * D_8009B220, so a caller reads "still running" straight out of the flags word
 * the handler just updated. duel_scene_update.c tests it exactly that way.
 *
 * func_80026A3C is its own group handler, one of the table entries. */
void func_80026A3C(void);
s32 func_80026B34(void);

#endif
