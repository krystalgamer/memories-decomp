#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_CALLBACKS_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_CALLBACKS_H

#include "../types.h"
#include "menu_record.h"

/* The two D_80090F68 display effect steps this unit defines.
 *
 * func_8003AD6C is the builder: it fills all four of the record's grid rows
 * by handing func_8003A1EC the groups at +0, +0xC, +0x18 and +0x24, which is
 * the shape func_8003A1EC.h documents and menu_record.h names grid[4][3].
 *
 * func_8003B054 drives the objects those rows hold.
 *
 * display_effect_step_table.c declared func_8003AD6C as `u8 *` while the
 * definition had already become MenuRecord *; the table only stores the
 * address, so nothing caught the disagreement. func_8003B054's own parameter
 * is still u8 * because that is what its definition says. */
void func_8003AD6C(MenuRecord *record);
void func_8003B054(u8 *record);

#endif
