#ifndef MEMORIES_DECOMP_MAIN_MENU_VALUE_SETUP_H
#define MEMORIES_DECOMP_MAIN_MENU_VALUE_SETUP_H

#include "../../types.h"

void MainMenu_DrawValueSetup(void);
s32 MainMenu_CountDecimalDigits(s32 value);

/* Value-setup screen state, shared by the four sources that drive it. Every
 * declarer already spelled both of these the same way.
 *
 *   D_801845BC  At least three bytes. [0] is the live value, [1] its start,
 *               and [2] the toggle result the screen computes.
 *   D_801845D8  Points at the toggle byte the caller handed in; the finish
 *               step writes through it.
 *
 * Worth knowing: D_801845BC[2] and the byte finish_value_setup.c declares
 * separately as D_801845BE are the same storage, since D_801845BC is at
 * 0x801845BC. One source writes it through the array and the other reads it
 * by its own name. That overlap is left exactly as it is here; naming it is
 * the kind of thing #2602 exists to sort out, and folding it into a
 * declaration move would be a change of meaning rather than of placement.
 *
 *   D_801845C0  The two tweened value pairs: [0]/[1] is the first value and
 *               its target, [6]/[7] the second. u16 elements, which is
 *               measured rather than preferred -- see below.
 *
 * NOT HERE, ON PURPOSE
 *
 * Three more symbols are shared by these same sources and are declared
 * inconsistently, so they stay local until the disagreement is settled:
 *
 *   D_801845B0  u8 *[], void *[] and ValueWidgetView *[]
 *   D_801845A0  void * and u8 *
 *   D_801845A4  void * and u8 *
 */
extern u8 D_801845BC[];
extern u8 *D_801845D8;
extern u16 D_801845C0[];

#endif
