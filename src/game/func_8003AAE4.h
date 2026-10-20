#ifndef MEMORIES_DECOMP_FUNC_8003AAE4_H
#define MEMORIES_DECOMP_FUNC_8003AAE4_H

#include "../types.h"
#include "menu_record.h"

/* Incoming effect step 7: initialize the blend row and advance a stored
 * halfword ramp from zero by D_8009B0D8 * 8. Completion keeps the primary
 * row, restores its existing row attributes, releases grid[1] and clears
 * the activity-inhibit bit.
 *
 * Before canonical names, the table privately declared address 0x8003AAE4
 * as `void func_8003AAE4(u8 *);` while the definition used MenuRecord *.
 * This owned header retains the definition's actual callback ABI. */
void DisplayEffect_FadeInRecord(MenuRecord *record);

#endif
