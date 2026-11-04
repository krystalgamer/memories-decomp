#ifndef MEMORIES_DECOMP_MODEL_SLOT_UPDATES_H
#define MEMORIES_DECOMP_MODEL_SLOT_UPDATES_H

#include "../types.h"
#include "model.h"

/* The two per-slot update steps model_slot_control.c and
 * Model_SetSlotPlaybackDirection (src/game/model_slot_state_updates.c) drive. Model_SetSlotPartSpeeds sets
 * the playback speed of every part's GsSEQ in one slot; Model_SetSlotRootTransform takes
 * the slot record itself rather than its index, which is why the two do not
 * share a parameter shape. */
void Model_SetSlotPartSpeeds(s32 index, s32 speed);
void Model_SetSlotRootTransform(ModelSlot *record, s32 translation_x, s32 translation_y, s32 translation_z, s32 yaw_request);

#endif
