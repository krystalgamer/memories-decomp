#include "../types.h"
#include "model_slot_part_animation.h"
#include "model.h"
#include "model_slot_state_updates.h"
#include "model_slot_updates.h"

void Model_SetSlotPlaybackSpeed(s32 slot_index, s32 speed)
{
    ModelSlot *slot = &gModel_aSlots[slot_index];

    if ((u32)(speed - 4) >= 29) {
        speed = 8;
    }
    slot->playback_speed = speed;
    if (slot->playback_state == 0x3E) {
        Model_SetSlotPlaybackDirection(slot_index, 1);
    }
}

void Model_SetSlotPlaybackDirection(s32 slot_index, s32 sign)
{
    ModelSlot *slot;
    s32 doubled_copy;
    s32 state;
    s32 velocity;
    s32 doubled;

    {
        ModelSlot *base = gModel_aSlots;

        slot = base + slot_index;
    }
    if (sign > 0)
        goto state_62;
    if (sign >= 0)
        goto state_35;
    slot->playback_state = 60;
    goto update;
state_35:
    slot->playback_state = 35;
    goto update;
state_62:
    slot->playback_state = 62;
update:
    {
        ModelSlot *current = &gModel_aSlots[slot_index];

        doubled = current->playback_speed * 2;
        state = current->playback_state;
    }
    doubled_copy = doubled;
    velocity = doubled;
    if (state == 60)
        goto negate;
    if (state < 61) {
        velocity = 0;
        if (state == 35)
            goto apply;
        return;
    } else {
        velocity = doubled_copy;
        if (state == 62)
            goto apply;
        return;
    }
negate:
    velocity = -velocity;
apply:
    Model_SetSlotPartSpeeds(slot_index, velocity);
}

void Model_SeekSlotAnimation(s32 slot_index, s32 animation_row, s32 frame_position)
{
    s32 part_index = 0;
    ModelSlot *slot = &gModel_aSlots[slot_index];
    s32 count;
    s32 scaled_position;

    count = slot->animation_sequence_count;
    slot->animation_position = frame_position << 4;
    scaled_position = slot->animation_position;

    if (count != 0) {
        do {
            s32 selected_row = slot->current_animation_row;

            if (animation_row != 0) {
                ModelSlotPart *part = slot->animation_sequences[part_index];
                selected_row = animation_row;
                part->sid = animation_row;
            }

            Model_SeekSlotPartAnimation(slot, part_index, selected_row, scaled_position);
            count = slot->animation_sequence_count;
            part_index++;
        } while (part_index < count);
    }

    if (animation_row != 0) {
        slot->current_animation_row = animation_row;
    }
    slot->cached_dimensions[3] = 0;
}
