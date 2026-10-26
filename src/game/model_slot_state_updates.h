#ifndef MEMORIES_DECOMP_MODEL_SLOT_STATE_UPDATES_H
#define MEMORIES_DECOMP_MODEL_SLOT_STATE_UPDATES_H

#include "../types.h"

/* Three updates applied to one model slot, all selected by slot index rather
 * than by a slot pointer, so callers do not need the record's layout.
 *
 * Model_SetSlotPlaybackSpeed stores the speed (values outside 4..32 become 8)
 * and, when the slot is already playing forward (state 0x3E), reapplies
 * forward playback so the parts pick up the new speed; it does not start an
 * idle or held slot. Model_SetSlotPlaybackDirection turns a sign into the
 * slot's playback state and part speed;
 * Model_SeekSlotAnimation sets the slot's frame and, when the animation
 * argument is nonzero, its current animation, seeking every part there. */
void Model_SetSlotPlaybackSpeed(s32 slot_index, s32 speed);
void Model_SetSlotPlaybackDirection(s32 slot_index, s32 sign);
void Model_SeekSlotAnimation(s32 slot_index, s32 animation_row, s32 frame_position);

#endif
