#ifndef MEMORIES_DECOMP_MODEL_GEOMETRY_TABLES_H
#define MEMORIES_DECOMP_MODEL_GEOMETRY_TABLES_H

#include "../types.h"

/* Shared model-effect presets. The ring renderer's twelve-byte RingSettings
 * view covers D_80091604's six initialized halfwords; the other table
 * consumers remain assembly. Keep their existing element/addressing views. */
extern u32 D_800915E8[];
extern u16 D_80091604[];
extern u16 D_80091610[];
extern u32 D_800916D4[];

#endif
