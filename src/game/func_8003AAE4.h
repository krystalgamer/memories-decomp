#ifndef MEMORIES_DECOMP_FUNC_8003AAE4_H
#define MEMORIES_DECOMP_FUNC_8003AAE4_H

#include "../types.h"
#include "menu_record.h"

/* D_80090F68 display effect step: the fade-out. It steps the record's 0x40
 * halfword as a 0..0x80 ramp by D_8009B0D8 * 8 and releases the grid[1] row
 * through func_80039F90 when it reaches the end.
 *
 * display_effect_step_table.c declared this `void func_8003AAE4(u8 *);` while
 * the definition had already become MenuRecord *. Nothing caught it: the
 * table only stores the address, so the wrong prototype type-checked and the
 * build matched. That is the drift this header exists to prevent. */
void func_8003AAE4(MenuRecord *record);

#endif
