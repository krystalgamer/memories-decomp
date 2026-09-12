#ifndef MEMORIES_DECOMP_TEXT_BOX_LIFECYCLE_H
#define MEMORIES_DECOMP_TEXT_BOX_LIFECYCLE_H

#include "../types.h"

/* The text box record the duel and main-menu cleanup paths destroy by address.
 * src/game/func_80024200.c casts it to a DuelEffectChannel to read the 0x2000
 * bit of its flags word before destroying it, and the main-menu trade screen
 * hands it straight to TextBox_Destroy; both had been declaring it privately
 * in these same words. Left unsized, which is what both declarers said. */
extern u8 D_800EB224[];

void TextBox_Destroy(void *record);
void *TextBox_Create(
    s32 index, s32 string_id, s32 x, s32 y, s32 width, s32 height
);
void *TextBox_CreateFlagged(
    s32 index, s32 string_id, s32 x, s32 y, s32 width, s32 height, s32 flags
);

#endif
