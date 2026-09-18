#ifndef MEMORIES_DECOMP_FUNC_8001D240_H
#define MEMORIES_DECOMP_FUNC_8001D240_H

#include "../types.h"
#include "display_object.h"

/* Rotates a displayed duel card between attack and defense orientation while
   keeping its DuelCardRecord defense-position flag synchronized. */
void DuelCard_UpdateDefenseRotation(DisplayObject *object);

/* Compatibility symbol used by the tracked field-action candidate. */
void func_8001D240(DisplayObject *object);

#endif
