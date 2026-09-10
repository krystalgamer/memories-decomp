#ifndef MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_STATE_H
#define MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_STATE_H

#include "../../types.h"

/* State shared by the name-entry screen's lifecycle functions.
 *
 * name_entry_frame.h and name_entry_keyboard.h already cover this screen's
 * drawing prefix and its keyboard entry points; neither is a home for the
 * screen's data. The fourteen-function name_entry_runtime.c pipeline uses
 * this header as the single view of that state.
 *
 * What the uses show, without renaming anything:
 *
 *   D_8016D400  A flag byte. Tested with `& 0x10`, set with `| 4`, cleared
 *               on setup.
 *   D_8016D402  The keyboard row, cleared on setup, stepped and wrapped
 *               through a signed compare, and used to index D_8016ABC0.
 *               Every declarer spells it u8 and casts (s8) at each use.
 *   D_8016D426  A saved copy of D_8016D402, restored back into it.
 *   D_8016D408  A counter handed to each spawned glyph as obj->f106 and
 *               then incremented.
 *   D_8016D434  The cursor's target x, D_8016D401 * 20 + 22.
 *   D_8016D436  The cursor's target y, D_8016D402 * 18 + 24, from which the
 *               tween step (target - current) << 8 / 8 is built.
 *   D_8016D43C  The object the length adjuster works on.
 *
 *   D_8016D401  The keyboard column, stepped and wrapped at 15.
 *   D_8016D42C  The caret position: scaled by 2 to index the name buffer, by
 *               16 for the glyph x, and shifted left 4 for the glyph lookup.
 *   D_8016D4D2  The pending dialog id, masked with 0xFFF at its one reader.
 *   D_8016D418  The name buffer, pointed at gSaveData_aPlayerNameSjis.
 *
 * Before the source coalesce, setup and runtime carried a few different
 * spellings. The setup path only stored zero, 244, or one pointer and never
 * constrained those types; the runtime's old `u16 *` view of D_8016D418 cast
 * to s32 before arithmetic and did not use its pointee type. The merged source
 * therefore keeps the evidence-backed declarations below.
 */
extern u8 D_8016D400;
extern u8 D_8016D402;
extern u8 D_8016D408;
extern u8 D_8016D426;
extern s16 D_8016D434;
extern s16 D_8016D436;
extern u8 *D_8016D43C;
extern s8 D_8016D401;
extern s8 D_8016D42C;
extern u16 D_8016D4D2;
extern u8 *D_8016D418;

/* The selection frame the keyboard moves. NameEntry_Init positions it and
 * installs the drawing callback described by name_entry_frame.h; the fields
 * extend that drawing prefix with the ones the keyboard tween needs, and
 * +0x30/+0x32 and +0x3C agree with NameEntrySelectionFrameView where the two
 * views overlap.
 *
 * NameEntry_Init stores +0x30, +0x32, +0x3C, +0x3E and +0x4C through the u8
 * pointer func_800400AC returned, then stores that pointer here with a cast.
 * NameEntry_UpdateKeyboard reads and writes x, y, width, widthBonus and timer
 * and writes stepX and stepY. NameEntry_SpawnGlyphSprite reads x and y.
 * Every offset those functions reach is a member here or falls inside
 * pad_3E, so this remains the superset view. */
typedef struct {
    u8 pad_00[0x30];
    s16 x;           /* 0x30 */
    s16 y;           /* 0x32 */
    u8 pad_34[0x2];
    s16 stepX;       /* 0x36, signed 8.8 per update */
    s16 stepY;       /* 0x38 */
    u8 pad_3A[0x2];
    u16 width;       /* 0x3C */
    u8 pad_3E[0x20];
    u8 widthBonus;   /* 0x5E, 20 for the wide finish control, else 0 */
    u8 pad_5F;
    s16 timer;       /* 0x60, eight updates of the move tween */
} SelectionFrame;

extern SelectionFrame *D_8016D404;

#endif
