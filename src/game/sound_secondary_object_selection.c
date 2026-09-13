#include "../types.h"
#include "sound.h"
#include "sound_secondary_object_selection.h"

s32 func_8004A8E4(s32 index, s32 value)
{
    SDSecondaryState *state = D_8009B458;
    SDSecondaryObject *object = &state->objects[index];

    if (object->channel_index != SD_SECONDARY_RECORD_NONE) {
        SDSecondaryRecord *record =
            (SDSecondaryRecord *)state + object->channel_index;

        if ((record->field_0006 & 0xF) != 0) {
            record->field_0006 = record->field_0006 - 1;
        }
    }
    return index;
}

