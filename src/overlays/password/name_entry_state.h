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
 * NOT HERE, ON PURPOSE
 *
 * Five neighbours in the same address block are declared inconsistently and
 * are left local until the disagreement is resolved from evidence rather
 * than by picking one:
 *
 *   D_8016D401  s8 and u8        D_8016D42C  s8 and u8
 *   D_8016D404  u8 *, Fixed *, W *
 *   D_8016D418  u8 * and u16 *   D_8016D4D2  u16 and s16
 *
 * A shared wrong declaration spreads further than a local one, so these wait.
 */
extern u8 D_8016D400;
extern u8 D_8016D402;
extern u8 D_8016D408;
extern u8 D_8016D426;
extern s16 D_8016D434;
extern s16 D_8016D436;
extern u8 *D_8016D43C;

#endif
