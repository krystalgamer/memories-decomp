#ifndef MEMORIES_DECOMP_DUEL_MAGIC_EFFECT_DISPATCH_H
#define MEMORIES_DECOMP_DUEL_MAGIC_EFFECT_DISPATCH_H

#include "../types.h"

/* Card-effect activation, dispatch, and one handler the dispatcher can reach.
 *
 * DuelEffect_UpdateCardEffect is the dispatcher: while
 * DUEL_CARD_EFFECT_FLAG_ACTIVE is set in gDuel_wCardEffectFlags, it indexes
 * gDuelEffect_abGroupByEffectId by the current effect id and picks the second
 * of the group's two handlers when
 * DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER is set, and calls it -- then returns
 * gDuel_wCardEffectFlags. A caller therefore reads "still running" directly
 * from the flags word the handler just updated. Candidate DuelScene_Update tests
 * it exactly that way.
 *
 * DuelEffect_StartCardEffect maps the presented card ID to the effect ID the
 * dispatcher consumes, records the original card ID, and raises the
 * active/handler flags. DuelEffect_ApplyHarpiesFeatherDuster is its own group
 * handler. */
void DuelEffect_ApplyHarpiesFeatherDuster(void);
s32 DuelEffect_UpdateCardEffect(void);
void DuelEffect_StartCardEffect(s32 value, s32 flag);

#endif
