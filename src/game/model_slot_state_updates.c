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
    s32 saved_double_speed;
    s32 playback_state;
    s32 part_speed;
    s32 double_speed;

    {
        ModelSlot *slot_base = gModel_aSlots;

        slot = slot_base + slot_index;
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
        ModelSlot *speed_slot = &gModel_aSlots[slot_index];

        double_speed = speed_slot->playback_speed * 2;
        playback_state = speed_slot->playback_state;
    }
    saved_double_speed = double_speed;
    part_speed = double_speed;
    if (playback_state == 60)
        goto negate;
    if (playback_state < 61) {
        part_speed = 0;
        if (playback_state == 35)
            goto apply;
        return;
    } else {
        part_speed = saved_double_speed;
        if (playback_state == 62)
            goto apply;
        return;
    }
negate:
    part_speed = -part_speed;
apply:
    Model_SetSlotPartSpeeds(slot_index, part_speed);
}

void Model_SeekSlotAnimation(s32 slot_index, s32 animation_row, s32 frame_position)
{
    s32 part_index = 0;
    ModelSlot *slot = &gModel_aSlots[slot_index];
    s32 part_count;
    s32 scaled_position;

    part_count = slot->animation_sequence_count;
    slot->animation_position = frame_position << 4;
    scaled_position = slot->animation_position;

    if (part_count != 0) {
        do {
            s32 selected_row = slot->current_animation_row;

            if (animation_row != 0) {
                ModelSlotPart *part_sequence = slot->animation_sequences[part_index];
                selected_row = animation_row;
                part_sequence->sid = animation_row;
            }

            Model_SeekSlotPartAnimation(slot, part_index, selected_row, scaled_position);
            part_count = slot->animation_sequence_count;
            part_index++;
        } while (part_index < part_count);
    }

    if (animation_row != 0) {
        slot->current_animation_row = animation_row;
    }
    slot->cached_dimensions[3] = 0;
}
