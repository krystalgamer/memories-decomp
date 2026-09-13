#ifndef MEMORIES_DECOMP_DUEL_EFFECT_MARK_OBJECT_IF_ACTIVE_H
#define MEMORIES_DECOMP_DUEL_EFFECT_MARK_OBJECT_IF_ACTIVE_H

#include "../types.h"
#include "menu_record.h"

/* Sets bit 0x2 in `record`'s field_34 if any D_800EB010 record is still active
 * -- it walks that table backward and stops at the first non-negative field_30
 * marker -- and does nothing when every marker is negative.
 *
 * The parameter is a MenuRecord because that is what the definition takes, and
 * the record it is handed is another element of the same table rather than a
 * separate type: Script_OpShowMenu passes a D_800EB010 element and writes the same
 * field_34 halfword itself. Its two callers spelled it `void *`; they pass a
 * text-box channel over the same memory, and cast. */
void DuelEffect_MarkObjectIfActive(MenuRecord *record);

#endif
