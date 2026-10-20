#ifndef MEMORIES_DECOMP_DISPLAY_PARENT_LINKS_H
#define MEMORIES_DECOMP_DISPLAY_PARENT_LINKS_H

#include "../types.h"
#include "display_object.h"
#include "duel_selection_layout.h"

void DuelSelection_LinkDisplayObject(
    DuelSelectionRecord *parent,
    volatile DisplayObject *object
);
void DuelSelection_LinkDisplayObjects(DuelSelectionRecord *parent, s32 clear);

#endif
