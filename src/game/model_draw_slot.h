#ifndef MEMORIES_DECOMP_MODEL_DRAW_SLOT_H
#define MEMORIES_DECOMP_MODEL_DRAW_SLOT_H

#include "../types.h"

/* Per-frame draw pass for model slot `index` (0 and 1 are the two duel sides,
 * 2 the shared scene slot): lights, colour fade into the primitive templates,
 * unit sort, bounding volume, ground shadow and palette flash. It returns
 * early while the slot is unloaded or fully faded out.
 *
 * Six callers, all with this spelling: model_scene_controllers.c,
 * model_scene_states.c, model_slot_support.c, duel_draw_field_cards.c,
 * library_draw_wireframe_globe.c and the overworld overlay's set_location.c. */
void Model_DrawSlot(s32 index);

#endif
