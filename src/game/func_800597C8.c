#include "../types.h"
#include "func_8004DC38.h"
#include "model.h"
#include "model_slot_state_updates.h"
#include "model_slot_updates.h"

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
