#ifndef MEMORIES_DECOMP_DUEL_SELECTION_UPDATE_LINKED_OBJECT_H
#define MEMORIES_DECOMP_DUEL_SELECTION_UPDATE_LINKED_OBJECT_H

#include "display_object.h"
#include "duel_selection_layout.h"

typedef struct {
    DisplayObject *parent;
    u8 pad_04[DUEL_SELECTION_RECORD_SIZE - 4];
} DuelSelectionDisplayRecord;

typedef char DuelSelectionDisplayRecord_size_must_match_record_stride[
    sizeof(DuelSelectionDisplayRecord) == DUEL_SELECTION_RECORD_SIZE ? 1 : -1
];

void DuelSelection_UpdateLinkedObject(DisplayObject *object);

#endif
