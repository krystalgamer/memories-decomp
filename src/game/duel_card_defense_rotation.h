#ifndef MEMORIES_DECOMP_DUEL_CARD_DEFENSE_ROTATION_H
#define MEMORIES_DECOMP_DUEL_CARD_DEFENSE_ROTATION_H

#include "../types.h"
#include "display_object.h"

/* Rotates a displayed duel card between attack and defense orientation while
   keeping its DuelCardRecord defense-position flag synchronized. */
void DuelCard_UpdateDefenseRotation(DisplayObject *object);

#endif
