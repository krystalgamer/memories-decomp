#ifndef MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_STATE_H
#define MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_STATE_H

#include "../../types.h"

/* State shared by the name-entry screen's translation units.
 *
 * name_entry_frame.h and name_entry_keyboard.h already cover this screen's
 * drawing prefix and its keyboard entry points; neither is a home for the
 * screen's data, which is what six sources here were each re-declaring.
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
 * Four of the five neighbours this header used to list as unsettled are the
 * four above. They were never really in dispute: name_entry_setup.c is the
 * only source that spelled them differently, and it only ever writes them
 * (`= 0`, `= 244`, and one pointer assignment), so it never constrained the
 * type. D_8016D418's third spelling was name_entry_runtime.c's
 * `u16 *`, which it casts to s32 before doing any arithmetic, so that one
 * never used its pointee type either. A declarer that only stores, or only
 * takes an address, abstains rather than votes.
 *
 * D_8016D404, the fifth, is declared below the list, with the record
 * name_entry_runtime.c used to keep for it.
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

/* The record name_entry_runtime.c used to keep privately as SelectionFrame,
 * moved here with the description it carried: the selection frame the
 * keyboard moves, which NameEntry_Init positions and whose drawing callback
 * name_entry_frame.h describes; the fields extend that drawing prefix with
 * the ones the keyboard's tween needs, and +0x30/+0x32 and +0x3C agree with
 * NameEntrySelectionFrameView where the two views overlap.
 *
 * NameEntry_Init stores +0x30, +0x32, +0x3C, +0x3E and +0x4C through the u8
 * pointer func_800400AC returned (name_entry_setup.c:134, :136, :138-140,
 * :143) and then stores that pointer here with a cast (:144).
 * NameEntry_UpdateKeyboard reads and writes x, y, width, widthBonus and
 * timer, and writes stepX and stepY, through a copy of it
 * (name_entry_runtime.c:105, :107-108, :110, :113-114, :117-119, :173, :176,
 * :186-188).
 * NameEntry_SpawnGlyphSprite reads x and y (name_entry_glyph_effects.c:294).
 * Every offset the other two units reach is a member here or falls inside
 * pad_3E, so this record is the superset view: name_entry_setup.c keeps its
 * u8 pointer and casts at the store, and name_entry_glyph_effects.c's
 * private view (pad0[48]; s16 x; s16 y) is gone. */
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
