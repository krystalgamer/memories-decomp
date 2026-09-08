#ifndef MEMORIES_DECOMP_NAME_ENTRY_KEYBOARD_H
#define MEMORIES_DECOMP_NAME_ENTRY_KEYBOARD_H

#include "../../types.h"

/* Known prefix only; text-glyph nodes have a 0x1C stride. */
typedef struct {
    u16 code;
    u8 pad02[0xA];
    s16 x;
    s16 y;
} NameEntryGlyphNodeView;

typedef void (*NameEntryGlyphUpdate)(u8 *sprite);

void NameEntry_UpdateKeyboard(void);
void *NameEntry_SpawnGlyphSprite(s32 textBoxSlot, NameEntryGlyphNodeView *glyphNode);
void NameEntry_UpdateGlyphPulse(u8 *sprite);
void NameEntry_UpdateGlyphFragment(u8 *sprite);
void NameEntry_UpdateGlyphShatter(u8 *sprite);
void NameEntry_UpdateGlyphTransfer(u8 *sprite);

#endif
