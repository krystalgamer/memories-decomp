#ifndef MEMORIES_DECOMP_FUNC_80039AD4_H
#define MEMORIES_DECOMP_FUNC_80039AD4_H

#include "../types.h"

struct DuelEffectChannel;

/* Releases a duel-effect object's occupancy slot: clears
 * D_800EAF08[object->field_10] -- the byte the object's own +0x10 index
 * owns -- zeroes its field_11, and raises the D_8009B330 rebuild flag so the next pass
 * recompacts. The object is not freed here; only its claim on the occupancy
 * table is. Two fade callbacks, func_80039BE0 and func_80039C94, end by
 * calling it. */
void DisplayObjectFade_ReleaseChannel(struct DuelEffectChannel *object);

#endif
