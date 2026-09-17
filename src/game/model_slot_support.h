#ifndef MEMORIES_DECOMP_MODEL_SLOT_SUPPORT_H
#define MEMORIES_DECOMP_MODEL_SLOT_SUPPORT_H

#include "../types.h"

void func_80059000(s32 index, s16 *output);
void func_800590DC(s32 index);

/* Returns flat light `light` of model slot `index`, clamping `light` to 0 at
   three; callers write the light's vectors through the returned pointer. The
   definition in model_slot_support.c records why it forms the address from
   D_800F39B0 rather than through D_800F2C40[0].field_D70. */
unsigned char *Model_GetFlatLight(unsigned int index, unsigned int light);

#endif
