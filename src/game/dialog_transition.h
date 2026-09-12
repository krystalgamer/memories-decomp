#ifndef MEMORIES_DECOMP_DIALOG_TRANSITION_H
#define MEMORIES_DECOMP_DIALOG_TRANSITION_H

#include "../types.h"
#include "menu_record.h"

/* Three D_80090F68 display effect steps that run the dialog's open and close
 * transitions. All three latch their first frame through
 * DUEL_EFFECT_STATE_FLAG_INITIALIZED in D_8009B3C1 -- the same byte, so only
 * one of them can be mid-transition -- and all three keep the object they build
 * in the menu record they are handed.
 *
 * func_8003D518 and func_8003D74C build the frame object at screen centre and
 * grow it; func_8003D614 is the close, shrinking the object's +0x60 by 0x40 a
 * frame and releasing it at zero. */
void func_8003D518(MenuRecord *state);
void func_8003D614(MenuRecord *state);
void func_8003D74C(MenuRecord *state);

#endif
