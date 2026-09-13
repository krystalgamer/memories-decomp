#ifndef MEMORIES_DECOMP_FUNC_800291E0_H
#define MEMORIES_DECOMP_FUNC_800291E0_H

#include "../types.h"

/* Builds the paired objects in one effect-resource slot and returns its
 * primary object, allocated second. Negative default-path coordinates are
 * derived from the packed card-stat word; special card types keep their
 * preset setup. The byte-pointer return preserves the existing caller views. */
u8 *func_800291E0(s32 index, s32 x, s32 y);

/* Releases both display-object words of one record through func_8004036C and
 * zeroes them. */
void func_80029528(s32 index);

#endif
