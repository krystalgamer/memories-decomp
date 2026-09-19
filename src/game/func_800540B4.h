#ifndef MEMORIES_DECOMP_FUNC_800540B4_H
#define MEMORIES_DECOMP_FUNC_800540B4_H

#include "../types.h"

/* Per-frame draw pass for model slot `index` (0 and 1 are the two duel sides,
 * 2 the shared scene slot): lights, colour fade into the primitive templates,
 * unit sort, bounding volume, ground shadow and palette flash. It returns
 * early while the slot is unloaded or fully faded out.
 *
 * Six callers, all with this spelling: model_scene_setup.c,
 * model_scene_states.c, model_slot_support.c, func_800164FC.c,
 * func_80029934.c and the overworld overlay's set_location.c. */
void func_800540B4(s32 index);

#endif
