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

/* Screen entry and exit. NameEntry_Init builds the keyboard, the name field
   and the caret; NameEntry_PollCompletion runs one frame and returns non-zero
   once the name has been accepted; NameEntry_BuildStarterDeck fills the
   player's deck once it has. */
void NameEntry_Init(void);
s32 NameEntry_PollCompletion(void);
void NameEntry_BuildStarterDeck(void);

/* One frame of the name field: reads the pad, moves the caret, and commits or
   cancels. */
void NameEntry_UpdateDialog(void);

/* Moves the name's length by `delta` and re-aims the caret, returning the new
   length. */
s32 NameEntry_AdjustLength(s32 delta, s32 arg);

/* The caret's slide callback, installed into a sprite's +0x24 slot by
   NameEntry_AdjustLength. */
void NameEntry_UpdateCaretTween(u8 *object);

#endif
