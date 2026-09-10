#ifndef MEMORIES_DECOMP_OPTIONS_H
#define MEMORIES_DECOMP_OPTIONS_H

#include "../types.h"
#include "display_object.h"

/* The options screen's shared state.
 *
 * options_update_layout.h next to this one is the per-TU header for
 * Options_UpdateLayout, in the shape #2495 asks for. This one is the family's
 * state, which all three sources were declaring by hand.
 *
 *   gOptions_bSelection   The highlighted row, passed to Options_UpdateLayout
 *                         and tested against 0 for the first entry.
 *   gOptions_bState       The screen's small state machine, switched on as
 *                         `gOptions_bState & 0xF`.
 *   gOptions_bOutputType  The audio output setting, only ever 0 or 1.
 *
 * options_update.c writes `gOptions_bState = *(u8 *)&gOptions_bSelection + 1`,
 * reading the signed byte through an unsigned lvalue. That cast is at the use,
 * not in the declaration, so the divergent load stays exactly where it was and
 * gOptions_bSelection keeps the s8 spelling all three sources gave it.
 */
extern s8 gOptions_bSelection;
extern u8 gOptions_bState;
extern s8 gOptions_bOutputType;

/* NOT HERE, ON PURPOSE
 *
 * gSD_bOutputType is shared by both sources but is declared in sound.h,
 * after the driver words it follows in memory, because it is not an
 * options symbol: the sound driver owns it, SaveData_BuildPayload
 * writes it into the save, and sound_frontend.c reaches it as a plain
 * scalar from small data, so sound.h carries the guarded arm those two
 * addressing groups need. An earlier version of this note said the
 * s8 [16] and u8 [9] spellings could not be reconciled, and that was
 * wrong: both bounds were the same lever for absolute addressing and
 * neither asserted a size; main_services.h says so beside D_8009B0A3.
 *
 * The two option display objects are no longer among the exceptions either:
 * see below.
 */

/* The two display objects the options screen keeps. options_init.c creates
 * them and stores them here; options_update_layout.c reads them back.
 *
 * options_init.c used to spell these `struct Obj *`, against a struct of its
 * own. It never dereferenced either global -- it only assigned them -- so
 * that spelling was an abstention rather than a competing claim, and the
 * local struct existed only to give the function's register-allocated local
 * a `f8` field at 0x08. The canonical DisplayObject names the same halfword
 * `flags` at the same offset, so the local type is gone and both globals are
 * DisplayObject * here. */
extern DisplayObject *D_8009B380;
extern DisplayObject *D_8009B388;

/* Builds the options screen and seeds the state above: it sets
 * gOptions_bState to 1, copies gSD_bOutputType into gOptions_bOutputType
 * (clamping a negative to 0), clears gOptions_bSelection, and stores the two
 * display objects into D_8009B388 and D_8009B380. It is declared here rather
 * than in a header of its own because this is the header that already owns
 * every global it writes.
 *
 * main_run_frontend_menus.c is the only caller and had the only declaration. */
void Options_Init(void);
s32 Options_Update(void);

#endif
