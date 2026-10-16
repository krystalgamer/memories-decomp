#ifndef MEMORIES_DECOMP_DISPLAY_PROJECTION_H
#define MEMORIES_DECOMP_DISPLAY_PROJECTION_H

#include "display_object.h"
#include "duel_card.h"

void DisplayObject_ProjectGroundPosition(DisplayObject *object);
void DisplayObject_ProjectLinkedGroundPosition(DisplayObject *object);
void Duel_DrawProjectedFieldCardFrame(DuelCardRecord *card);

#endif
