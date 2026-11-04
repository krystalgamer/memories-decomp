#ifndef MEMORIES_DECOMP_DUEL_EFFECT_MARK_OBJECT_IF_ACTIVE_H
#define MEMORIES_DECOMP_DUEL_EFFECT_MARK_OBJECT_IF_ACTIVE_H

#include "../types.h"
#include "menu_record.h"

struct DuelEffectChannel;

/* Sets DUEL_EFFECT_MARK_OBJECT_FLAG in `channel`'s flags_34 if any
 * gDisplayEffect_aMenuRecords record is still active -- it walks that table
 * backward and stops at the first non-negative effect_id marker -- and does
 * nothing when every marker is negative.
 *
 * The parameter is the text-box channel its callers have just made: every
 * caller passes the result of TextBox_Create or TextBox_CreateFlagged, and
 * Script_OpShowDialog ORs its own flag into the same flags_34 right after.
 * It used to be typed as a MenuRecord, whose 0x34 is the record's x
 * position, not a flag word. */
void DuelEffect_MarkObjectIfActive(struct DuelEffectChannel *channel);

#endif
