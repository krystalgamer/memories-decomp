#include "../types.h"
#include "sound.h"

s32 func_8004A8E4(s32 index)
{
    SDSecondaryState *state = D_8009B458;
    SDSecondaryObject *object = &state->objects[index];

    if (object->field_0003 != 0x63) {
        SDSecondaryRecord *record =
            (SDSecondaryRecord *)state + object->field_0003;

        if ((record->field_0006 & 0xF) != 0) {
            record->field_0006 = record->field_0006 - 1;
        }
    }
    return index;
}
