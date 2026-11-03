#ifndef MEMORIES_DECOMP_MODEL_SLOT_SUPPORT_H
#define MEMORIES_DECOMP_MODEL_SLOT_SUPPORT_H

#include "../types.h"

void Model_GetSlotDimensionsWithOverrides(s32 index, s16 *output);
void Model_DrawSlotUnitsOnly(s32 index);

/* Returns flat light `light` of model slot `index`, clamping `light` to 0 at
   three; callers write the light's vectors through the returned pointer. */
unsigned char *Model_GetFlatLight(unsigned int index, unsigned int light);

#endif
