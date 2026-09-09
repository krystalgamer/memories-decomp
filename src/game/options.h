#ifndef MEMORIES_DECOMP_OPTIONS_H
#define MEMORIES_DECOMP_OPTIONS_H

#include "../types.h"

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
 * Three more symbols are shared by these sources and their declarers disagree:
 *
 *   gSD_bOutputType  s8 [16] in options_init.c, u8 [9] in options_update.c.
 *                    Both the element sign and the bound differ, and a small
 *                    array's size decides whether -G8 puts it in small data,
 *                    so this one cannot be settled by preferring a spelling.
 *   D_8009B380       DisplayObject * in options_update_layout.c, struct Obj *
 *   D_8009B388       in options_init.c, where struct Obj is local to that file.
 *
 * The two pointers are the same shape of problem the password digit cursor
 * had: a canonical type against a local one. That is #2501 work and wants a
 * measurement, not a preference.
 */

#endif
