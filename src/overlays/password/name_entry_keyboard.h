#ifndef MEMORIES_DECOMP_NAME_ENTRY_KEYBOARD_H
#define MEMORIES_DECOMP_NAME_ENTRY_KEYBOARD_H

#include "../../types.h"
#include "../../game/duel_effect.h"

typedef void (*NameEntryGlyphUpdate)(u8 *sprite);

void NameEntry_BuildKeyboardTextBox(s32 textOffset);
void NameEntry_UpdateKeyboard(void);
/* The glyph nodes this screen walks are D_800EB288 entries: the record's
   0x24 pointer is where TextBox_GetGlyphAt starts, and it returns the entry
   whose x_0C/y_0E pair matches. */
DuelEffectEntry *TextBox_GetGlyphAt(s32 textBoxSlot, s32 x, s32 y);
void *NameEntry_SpawnGlyphSprite(s32 textBoxSlot, DuelEffectEntry *glyphNode);
void NameEntry_UpdateGlyphPulse(u8 *sprite);
void NameEntry_UpdateGlyphFragment(u8 *sprite);
void NameEntry_UpdateGlyphShatter(u8 *sprite);
void NameEntry_UpdateGlyphTransfer(u8 *sprite);

#endif
