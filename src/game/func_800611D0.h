#ifndef MEMORIES_DECOMP_FUNC_800611D0_H
#define MEMORIES_DECOMP_FUNC_800611D0_H

#include "../types.h"

/* Opens the tall text box in channel 3 (string 0x20) with a highlight
 * object. Before that it sets colour slots 0 to 2 in gText_abColorSlots to 4
 * and then slot `offset` to 0, so one of the three draws differently. */
void func_800611D0(int offset);

#endif
