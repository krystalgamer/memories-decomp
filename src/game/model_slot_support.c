#include "../types.h"

#include "model.h"
#include "model_slot_support.h"
#include "../unmatched.h"

void func_80059000(s32 index, s16 *output)
{
    register ModelSlot *entry = &D_800F2C40[index];
    register u8 *source = entry->field_CF8.field_00;
    s32 value;

    *(ModelBytes8 *)output = *(ModelBytes8 *)entry->field_DC8;
    value = source[7];
    if (value) {
        output[0] = value << 4;
    }
    value = source[8];
    if (value) {
        output[1] = value << 4;
    }
    value = source[9];
    if (value) {
        output[2] = value << 4;
    }
    output[3] = 0;
    if (output[0] > 0) {
        output[3] = output[0];
    }
    if (output[3] < output[1]) {
        output[3] = output[1];
    }
    if (output[3] < output[2]) {
        output[3] = output[2];
    }
}

void func_800590DC(s32 index)
{
    ModelSlot *entry = &D_800F2C40[index];
    u8 old = entry->field_E11;

    entry->field_E11 = 1;
    func_800540B4(index);
    entry->field_E11 = old;
}
