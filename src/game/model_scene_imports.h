#ifndef MEMORIES_DECOMP_MODEL_SCENE_IMPORTS_H
#define MEMORIES_DECOMP_MODEL_SCENE_IMPORTS_H

#include "../types.h"

/* Measured resident caller views, not recovered overlay implementation types.
 * func_800507D0 calls these fixed linker symbols after loading SU.MRG sectors
 * [1223, 1239) through D_80010030: no arguments, one s32 zero, and no arguments
 * with an s32 result tested for zero, respectively. Keep the address names:
 * other packages reuse this overlay address range (including main_menu's
 * SU.MRG sectors [98, 114)), and their declarations are not interchangeable. */
void func_801807B0(void);
void func_80181C4C(s32 value);
s32 func_80180A24(void);

#endif
