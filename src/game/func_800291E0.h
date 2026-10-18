#ifndef MEMORIES_DECOMP_FUNC_800291E0_H
#define MEMORIES_DECOMP_FUNC_800291E0_H

#include "../types.h"

struct DisplayObject;

/* Builds the paired objects in one effect-resource slot and returns its
 * primary object, allocated second. Default-path attack/defense overrides
 * are narrowed to s16 before testing: a negative low halfword selects the
 * corresponding packed base stat. Special card types keep their preset
 * setup. Callers retain their explicit partial-view casts. */
struct DisplayObject *func_800291E0(s32 index, s32 attack, s32 defense);

/* Releases both display-object words of one record through DisplayObject_ReleaseIfPresent and
 * zeroes them. */
void DuelEffect_ReleaseResourceObjects(s32 index);

#endif
