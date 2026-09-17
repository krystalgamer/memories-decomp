#ifndef MEMORIES_DECOMP_DUEL_CARD_RECORD_LIFECYCLE_H
#define MEMORIES_DECOMP_DUEL_CARD_RECORD_LIFECYCLE_H

#include "../types.h"
#include "duel_card.h"
#include "duel_card_display_state.h"

/* Releases the record's display object and clears DUEL_CARD_FLAG_OCCUPIED,
   leaving the rest of the flags alone. */
void DuelCard_DeactivateRecord(DuelCardRecord *object);

/* DuelCard_DeactivateRecord followed by clearing the whole flags halfword. */
void DuelCard_RemoveFromField(DuelCardRecord *object);

DuelCardDisplayObject *func_80024C1C(s32 card_id, s32 x, s32 y);
void func_80024D34(s32 slot, s32 card_id);

#endif
