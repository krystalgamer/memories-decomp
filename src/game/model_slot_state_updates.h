#ifndef MEMORIES_DECOMP_MODEL_SLOT_STATE_UPDATES_H
#define MEMORIES_DECOMP_MODEL_SLOT_STATE_UPDATES_H

#include "../types.h"

/* Three updates applied to one model slot, all selected by slot index rather
 * than by a slot pointer, so callers do not need the record's layout.
 *
 * Model_SetSlotPlaybackSpeed selects a bounded type and starts the positive state when the
 * slot is idle; Model_SetSlotPlaybackDirection turns a sign into the slot's velocity;
 * Model_SeekSlotAnimation sets the slot's frame and, when the animation
 * argument is nonzero, its current animation, seeking every part there. */
void Model_SetSlotPlaybackSpeed(s32 index, s32 type);
void Model_SetSlotPlaybackDirection(s32 index, s32 sign);
void Model_SeekSlotAnimation(s32 idx, s32 flag, s32 val);

#endif
