#include "../types.h"
#include "sound.h"
#include "sound_secondary_object_selection.h"

s32 func_8004A854(s32 value)
{
    s32 result = -1;
    u8 *state = (u8 *)D_8009B458;
    s32 best = 0xFFFF;
    s32 i = 0;

    if (*(short *)(state + 0x510) > i) {
        s32 offset;

        value = (u8)value;
        offset = 0;
        do {
            u8 *entry = state + offset;
            u16 candidate = *(u16 *)(entry + 0x19E);

            if ((u16)best >= candidate && entry[0x183] == value &&
                entry[0x18D] != 0) {
                best = candidate;
                result = i;
            }
            offset += SD_SECONDARY_OBJECT_SIZE;
            state = (u8 *)D_8009B458;
            i++;
        } while (i < *(short *)(state + 0x510));
    }
    best = result;
    return best;
}

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

