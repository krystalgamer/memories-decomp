#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_SET_OBJECT_ROW_BLEND_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_SET_OBJECT_ROW_BLEND_H

#include "../types.h"

/* Exactly three nullable slots, descending. Replaces blend bits, narrows
 * depth to s8 and adjusts CLUT y; zero retains GsALON, restores neutral RGB
 * and decrements CLUT y rather than selecting an opaque/idempotent mode. */
void DisplayEffect_SetObjectRowBlend(u8 **objects, u32 attribute, s32 value);

#endif
