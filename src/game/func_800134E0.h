#ifndef MEMORIES_DECOMP_FUNC_800134E0_H
#define MEMORIES_DECOMP_FUNC_800134E0_H

#include "../types.h"
#include "view_state.h"

/* Places a view's viewpoint at an offset from its own reference point and
 * installs it.
 *
 * The three arguments are added to the reference point at 0x1C, 0x20 and
 * 0x24, the sum becomes the viewpoint at 0x10, 0x14 and 0x18, and the whole
 * thing is handed to GsSetRefView2.
 *
 * The record from 0x10 to 0x2F is a Psy-Q GsRVIEW2. That is not inferred
 * from the offsets lining up: GsSetRefView2 takes a GsRVIEW2 * and this
 * function passes it the address 0x10 bytes into the object. Both callers
 * pass &D_800F2848, and view_state.h now spells that range as the GsRVIEW2
 * it is, so the function takes ViewState and reaches the view by name. */
void func_800134E0(ViewState *object, s32 x, s32 y, s32 z);

#endif
