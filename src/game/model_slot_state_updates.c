#include "../types.h"
#include "func_8004DC38.h"
#include "model.h"
#include "model_slot_state_updates.h"
#include "model_slot_updates.h"

void Model_SetSlotPlaybackSpeed(s32 index, s32 type)
{
    ModelSlot *slot = &gModel_aSlots[index];

    if ((u32)(type - 4) >= 29) {
        type = 8;
    }
    slot->field_E0D = type;
    if (slot->field_E16 == 0x3E) {
        Model_SetSlotPlaybackDirection(index, 1);
    }
}

void Model_SetSlotPlaybackDirection(s32 index, s32 sign)
{
    ModelSlot *slot;
    s32 doubled_copy;
    s32 state;
    s32 velocity;
    s32 doubled;

    {
        ModelSlot *base = gModel_aSlots;

        slot = base + index;
    }
    if (sign > 0)
        goto state_62;
    if (sign >= 0)
        goto state_35;
    slot->field_E16 = 60;
    goto update;
state_35:
    slot->field_E16 = 35;
    goto update;
state_62:
    slot->field_E16 = 62;
update:
    {
        ModelSlot *current = &gModel_aSlots[index];

        doubled = current->field_E0D * 2;
        state = current->field_E16;
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
    Model_SetSlotPartSpeeds(index, velocity);
}

void Model_SeekSlotAnimation(s32 idx, s32 flag, s32 val)
{
    s32 i = 0;
    ModelSlot *rec = &gModel_aSlots[idx];
    s32 count;
    s32 arg3;

    count = rec->animation_sequence_count;
    rec->animation_position = val << 4;
    arg3 = rec->animation_position;

    if (count != 0) {
        do {
            s32 arg2 = rec->field_BF5;

            if (flag != 0) {
                ModelSlotPart *part = rec->animation_sequences[i];
                arg2 = flag;
                part->sid = flag;
            }

            Model_SeekSlotPartAnimation(rec, i, arg2, arg3);
            count = rec->animation_sequence_count;
            i++;
        } while (i < count);
    }

    if (flag != 0) {
        rec->field_BF5 = flag;
    }
    rec->field_DC8[3] = 0;
}
