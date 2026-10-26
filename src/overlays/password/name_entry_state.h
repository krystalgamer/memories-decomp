#ifndef MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_STATE_H
#define MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_STATE_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "module_state.h"

/* State shared by the name-entry screen's lifecycle functions.
 *
 * name_entry_frame.h and name_entry_keyboard.h already cover this screen's
 * drawing prefix and its keyboard entry points; neither is a home for the
 * screen's data. The fourteen-function name-entry pipeline uses this header
 * as the single view of that state: eleven functions in
 * name_entry_runtime.c, two in name_entry_dialog.c, and
 * NameEntry_UpdateKeyboard in name_entry_runtime.c.
 *
 * What the uses show (the evidence-backed ones are named):
 *
 *   gNameEntry_bStateFlags  A flag byte. Tested with `& 0x10`, set with `| 4`, cleared
 *               on setup.
 *   gNameEntry_bKeyboardRow  The keyboard row, cleared on setup, stepped and wrapped
 *               through a signed compare, and used to index gNameEntry_aRowLinks.
 *               Every declarer spells it u8 and casts (s8) at each use.
 *   gNameEntry_bSavedRow  A saved copy of gNameEntry_bKeyboardRow, restored back into it.
 *   gNameEntry_bGlyphSequence  A counter handed to each spawned glyph as obj->f106 and
 *               then incremented.
 *   gNameEntry_wCursorTargetX  The cursor's target x, gNameEntry_bKeyboardColumn * 20 + 22.
 *   gNameEntry_wCursorTargetY  The cursor's target y, gNameEntry_bKeyboardRow * 18 + 24, from which the
 *               tween step (target - current) << 8 / 8 is built.
 *   gNameEntry_pCaret  The object the length adjuster works on.
 *
 *   gNameEntry_bKeyboardColumn  The keyboard column, stepped and wrapped at 15.
 *   gNameEntry_bCaretPos  The caret position: scaled by 2 to index the name buffer, by
 *               16 for the glyph x, and shifted left 4 for the glyph lookup.
 *   gNameEntry_wPendingDialogId  The pending dialog id, masked with 0xFFF at its one reader.
 *   gNameEntry_pNameBuffer  The name buffer, pointed at gSaveData_aPlayerNameSjis.
 *
 * Before the source coalesce, setup and runtime carried a few different
 * spellings. The setup path only stored zero, 244, or one pointer and never
 * constrained those types; the runtime's old `u16 *` view of gNameEntry_pNameBuffer cast
 * to s32 before arithmetic and did not use its pointee type. The merged source
 * therefore keeps the evidence-backed declarations in module_state.h.
 */
/* Four more of the same family, each declared privately until now.
 *
 *   gNameEntry_bGlyphVramSlot  Cleared to 0 by the name-entry reset in
 *               name_entry_runtime.c; the Japanese NameEntry_SpawnGlyphSprite
 *               uses it to pick one of sixteen gNameEntry_aGlyphVramRects and
 *               then advances it modulo 16.
 *   D_8016D41C  Cleared to 0 by the dialog teardown in
 *               name_entry_dialog.c. Nothing else touches it.
 *   gNameEntry_bCharacterSet  The keyboard character set: the reset sets 2
 *               (0 in the Japanese build), and the Japanese mode key cycles it
 *               through 0, 1 and 2, rebuilding the keyboard each time.
 *   gNameEntry_wSelectionFrameFlags  Bit 0x4000 marks the selection-frame
 *               tween in progress: NameEntry_UpdateKeyboard raises it when it
 *               starts a move, steps the frame while it is set and clears it
 *               with `&= 0xBFFF` when the eight-step tween ends.
 *
 * The first three were ALSO declared in the func_8016913C candidate, which
 * never named them anywhere else -- dead declarations rather than a second
 * opinion, and were deleted along with that unit's equally unused
 * `extern DuelEffectChannel D_800EB1C0;`. */
extern DuelEffectChannel D_800EB1C0;

/* The selection frame the keyboard moves. NameEntry_Init positions it and
 * installs the drawing callback described by name_entry_frame.h; the fields
 * combine the drawing fields with the ones the keyboard tween needs.
 * NameEntrySelectionFrameView is an alias of this same central contract.
 *
 * NameEntry_Init stores +0x30, +0x32, +0x3C, +0x3E and +0x4C through the u8
 * pointer DisplayObject_AcquireSlot returned, then stores that pointer here with a cast.
 * NameEntry_UpdateKeyboard reads and writes x, y, width, widthBonus and timer
 * and writes stepX and stepY. NameEntry_SpawnGlyphSprite reads x and y.
 * The drawing callback additionally constrains priority and height.
 * ygo_types.h defines the combined SelectionFrame; the observed prefix is
 * not a claim about the complete display-object allocation. */
#endif
