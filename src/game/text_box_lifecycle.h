#ifndef MEMORIES_DECOMP_TEXT_BOX_LIFECYCLE_H
#define MEMORIES_DECOMP_TEXT_BOX_LIFECYCLE_H

#include "../types.h"

struct DuelEffectChannel;

/* Record 3 of gText_aChannels (base + 3 * 0x64; Japanese uses base + 3 * 0x60),
 * the text box record the duel and main-menu cleanup paths destroy directly.
 * src/game/duel_scene_update.c casts it to a DuelEffectChannel to read the 0x2000
 * bit of its flags word before destroying it, and the main-menu trade screen
 * hands it straight to TextBox_Destroy; both had been declaring it privately
 * in these same words. Left unsized, which is what both declarers said. */
extern u8 gText_Channel3[];
/* The frozen main-menu trade overlay still names the same record by address. */
extern u8 D_800EB224[];

void TextBox_Destroy(struct DuelEffectChannel *record);
void TextBox_DestroyIndex(s32 index);
void *TextBox_Create(
    s32 index, s32 string_id, s32 x, s32 y, s32 width, s32 height
);
void *TextBox_CreateFlagged(
    s32 index, s32 string_id, s32 x, s32 y, s32 width, s32 height, s32 flags
);

#endif
