#ifndef MEMORIES_DECOMP_TEXT_BOX_LIFECYCLE_H
#define MEMORIES_DECOMP_TEXT_BOX_LIFECYCLE_H

#include "../types.h"

void TextBox_Destroy(void *record);
void *TextBox_Create(
    s32 index, s32 string_id, s32 x, s32 y, s32 width, s32 height
);
void *TextBox_CreateFlagged(
    s32 index, s32 string_id, s32 x, s32 y, s32 width, s32 height, s32 flags
);

#endif
