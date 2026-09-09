#ifndef MEMORIES_DECOMP_DUEL_FIELD_EQUIP_SEARCH_H
#define MEMORIES_DECOMP_DUEL_FIELD_EQUIP_SEARCH_H

#include "../types.h"

/* Collecting occupied field records into a caller-supplied array.
 *
 * Both collectors write byte pointers and terminate the array with a null,
 * so the caller's array must hold one more entry than the row it scans.
 * They return how many were written, which is the same count the null
 * terminator implies.
 *
 * The element type is u8 * because that is what the collectors store: each
 * record is cast on the way in. Callers that keep their arrays as
 * DuelCardRecord * convert at the call site, which is where the conversion
 * belongs. */
s32 Duel_CollectFieldCardsBelowType(u8 **out, s32 arg1, s32 arg2);
s32 Duel_CollectFieldCardsByType(u8 **out, s32 arg1, s32 arg2);

/* Returns 0 when it has written an equip pairing into D_800EAE88, 1 when
 * there is nothing to pair. */
s32 func_80026DC8(void);

#endif
