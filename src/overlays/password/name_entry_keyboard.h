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
/* The sprite created by NameEntry_SpawnGlyphSprite and stepped by the glyph
 * callbacks. +0x44/+0x46 are scale for the pulse callback and destination XY
 * for the tween callbacks; the installed +0x24 callback selects the meaning. */
typedef struct {
    u8 pad_00[0x4];
    u32 flags;                    /* 0x04 */
    u8 pad_08[0x3C];
    s16 scale_x;                  /* 0x44, target x during a tween */
    s16 scale_y;                  /* 0x46, target y during a tween */
    u32 field_48;                 /* 0x48 */
    DuelEffectEntry *sourceGlyph; /* 0x4C */
    u8 pad_50[0xA];
    s16 savedSourceX;             /* 0x5A */
    u8 pad_5C[0x4];
    s16 frame;                    /* 0x60, also the tween timer */
    u8 pad_62[0x5];
    u8 textBoxSlot;               /* 0x67 */
    u8 pad_68[0x2];
    u8 sequence;                  /* 0x6A */
} GlyphSprite;

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

/* The dialog's own sliding panel object, reached through the text box at
 * field_2C or by tag from func_80042B40. A different object from the
 * selection frame name_entry_state.h declares, and the fields are spelled
 * by hex offset here so that the two views cannot be read as if they shared
 * a numbering: this unit's +0x30 is the old Caret's `f48`. */
typedef struct {
    u8 pad_00[0x8];
    u16 flags;       /* 0x08 */
    u8 pad_0A[0x26];
    s16 x;           /* 0x30 */
    s16 y;           /* 0x32 */
    u8 pad_34[0x2C];
    s16 slide;       /* 0x60, signed distance still to travel */
    u8 pad_62[0xA];
    u8 status;       /* 0x6C */
} DialogCaret;

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
