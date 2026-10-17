#ifndef MEMORIES_DECOMP_FUNC_800291E0_H
#define MEMORIES_DECOMP_FUNC_800291E0_H

#include "../types.h"

struct DisplayObject;

/* Builds the paired objects in one effect-resource slot and returns its
 * primary object, allocated second. Negative default-path coordinates are
 * derived from the packed card-stat word; special card types keep their
 * preset setup. Callers retain their explicit partial-view casts. */
struct DisplayObject *func_800291E0(s32 index, s32 x, s32 y);

/* Releases both display-object words of one record through DisplayObject_ReleaseIfPresent and
 * zeroes them. */
void DuelEffect_ReleaseResourceObjects(s32 index);

#endif
