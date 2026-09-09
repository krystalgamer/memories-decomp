#ifndef MEMORIES_DECOMP_DUEL_FIELD_EQUIP_SEARCH_H
#define MEMORIES_DECOMP_DUEL_FIELD_EQUIP_SEARCH_H

#include "../types.h"
#include "duel_card.h"

/* Collecting occupied field records into a caller-supplied array.
 *
 * Both collectors terminate the array with a null, so the caller's array must
 * hold one more entry than the row it scans. They return how many were
 * written, which is the same count the null terminator implies.
 *
 * The element type is DuelCardRecord * because that is what both collectors
 * walk and store: each writes `*out++ = r` from a DuelCardRecord * cursor
 * over D_801A7AD8. This used to be u8 **, on the grounds that a byte pointer
 * was what they stored and that callers holding DuelCardRecord * should
 * convert at the call site. Neither half held up: the cast was a consequence
 * of the parameter type rather than a fact about the data, and the callers
 * did the opposite -- ai_turn_action.c passes DuelCardRecord *list0[6]
 * uncast, which warned, while casting a stale local type for another call in
 * the same file. */
s32 Duel_CollectFieldCardsBelowType(DuelCardRecord **out, s32 arg1, s32 arg2);
s32 Duel_CollectFieldCardsByType(DuelCardRecord **out, s32 arg1, s32 arg2);

/* Returns 0 when it has written an equip pairing into D_800EAE88, 1 when
 * there is nothing to pair. */
s32 func_80026DC8(void);

#endif
