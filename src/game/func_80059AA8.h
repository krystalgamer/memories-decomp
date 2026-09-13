#ifndef MEMORIES_DECOMP_FUNC_80059AA8_H
#define MEMORIES_DECOMP_FUNC_80059AA8_H

#include "../types.h"

/* Reads model slot `index`'s field_E12 and returns it, replacing it first
 * when `value` is non-negative. model_scene_setup.c uses both halves: it
 * saves the old value with one call and restores it with another. */
s32 func_80059AA8(s32 index, s32 value);

#endif
