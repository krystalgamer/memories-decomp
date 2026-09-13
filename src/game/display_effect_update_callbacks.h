#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_CALLBACKS_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_CALLBACKS_H

#include "../types.h"
#include "menu_record.h"

/* The final two D_80090F68 display effect steps in this unit.
 *
 * func_8003AD6C is the builder: it fills all four of the record's grid rows
 * by handing func_8003A1EC the groups at +0, +0xC, +0x18 and +0x24, which is
 * the shape func_8003A1EC.h documents and menu_record.h names grid[4][3].
 *
 * func_8003B054 drives the objects those rows hold.
 *
 * Both callbacks use the MenuRecord element type stored by the step table. */
void func_8003AD6C(MenuRecord *record);
void func_8003B054(MenuRecord *record);

#endif
