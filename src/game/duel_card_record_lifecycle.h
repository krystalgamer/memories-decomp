#ifndef MEMORIES_DECOMP_DUEL_CARD_RECORD_LIFECYCLE_H
#define MEMORIES_DECOMP_DUEL_CARD_RECORD_LIFECYCLE_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "duel_card.h"
#include "duel_card_display_state.h"

/* Two GPU transfer rectangles per live card slot, including hand cards. */
extern RECT gDuel_aCardUploadRects[];

/* Releases the record's display object and clears DUEL_CARD_FLAG_OCCUPIED,
   leaving the rest of the flags alone. */
void DuelCard_DeactivateRecord(DuelCardRecord *object);

/* DuelCard_DeactivateRecord followed by clearing the whole flags halfword. */
void DuelCard_RemoveFromField(DuelCardRecord *object);

/* Builds the world-space field object; the position words become X/Z and
   world Y starts at -24. The caller publishes the record's object pointer. */
DuelCardDisplayObject *Duel_CreateFieldCardObject(s32 card_id, s32 x, s32 y);
/* Initializes the record from an encoded deck entry and attaches its field
   object. Callers handle retained flags/stat modifiers and old-object release. */
void Duel_SetupFieldCardFromDeck(s32 slot, s32 deck_index);

#endif
