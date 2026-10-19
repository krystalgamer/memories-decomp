#ifndef MEMORIES_DECOMP_DUEL_FIELD_EFFECTS_H
#define MEMORIES_DECOMP_DUEL_FIELD_EFFECTS_H

#include "../types.h"
#include "duel_card.h"

/* Field-wide stat-restoration step driven once per 16 frames. It sweeps the
 * third grid row, staggering a type-8 effect object per slot and clearing a
 * negative stat modifier where it finds one. */
void DuelEffect_ApplyCursebreaker(void);

void DuelEffect_StartRitual(void);
void DuelEffect_ApplyRitual(void);

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

/* Returns the first unoccupied slot at or after `start` on the active side,
 * or -1 when the five-slot row is full. */
s32 Duel_FindFreeFieldSlot(s32 start);

/* Collecting occupied field records into a caller-supplied array.
 *
 * Both collectors terminate the array with a null, so the caller's array must
 * hold one more entry than the row it scans. They return how many were
 * written, which is the same count the null terminator implies.
 *
 * The element type is DuelCardRecord * because that is what both collectors
 * walk and store: each writes `*out++ = r` from a DuelCardRecord * cursor
 * over gDuel_aCardRecords. This used to be u8 **, on the grounds that a byte pointer
 * was what they stored and that callers holding DuelCardRecord * should
 * convert at the call site. Neither half held up: the cast was a consequence
 * of the parameter type rather than a fact about the data, and the callers
 * did the opposite -- ai_turn_action.c passes DuelCardRecord *list0[6]
 * uncast, which warned, while casting a stale local type for another call in
 * the same file. */
s32 Duel_CollectFieldCardsBelowType(DuelCardRecord **out, s32 arg1, s32 arg2);
s32 Duel_CollectFieldCardsByType(DuelCardRecord **out, s32 arg1, s32 arg2);

/* Returns 0 when it has written an equip pairing into gAi_Selection, 1 when
 * there is nothing to pair. */
s32 Duel_SelectEquipPair(void);

/* The same convention, for a trap rather than an equip: 0 once it has written
 * a play into gAi_Selection, 1 when it found no trap on the field or no free
 * slot. ai_turn_action.c calls the two in adjacent lines behind the same
 * `(rand() & 1) == 0 && ... == 0` guard, so they are alternatives in one
 * chain rather than unrelated helpers. */
s32 Duel_SelectTrapPlay(void);

#endif
