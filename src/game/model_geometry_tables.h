#ifndef MEMORIES_DECOMP_MODEL_GEOMETRY_TABLES_H
#define MEMORIES_DECOMP_MODEL_GEOMETRY_TABLES_H

#include "../types.h"

/* Four tables read only by func_8006AF74 and func_8006CD78, both still
   assembly. Declared here so that when either is decompiled it takes the
   declaration from one place rather than writing its own. */
extern u32 D_800915E8[];
extern u16 D_80091604[];
extern u16 D_80091610[];
extern u32 D_800916D4[];

#endif
