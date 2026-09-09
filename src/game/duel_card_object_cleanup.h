#ifndef MEMORIES_DECOMP_DUEL_CARD_OBJECT_CLEANUP_H
#define MEMORIES_DECOMP_DUEL_CARD_OBJECT_CLEANUP_H

#include "duel_card.h"

/* Releases the record's display object and clears DUEL_CARD_FLAG_OCCUPIED,
   leaving the rest of the flags alone. */
void func_80024914(DuelCardRecord *object);

/* func_80024914 followed by clearing the whole flags halfword. */
void func_80024954(DuelCardRecord *object);

#endif
