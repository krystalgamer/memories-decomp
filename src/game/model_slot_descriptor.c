#include "../types.h"

#include "model.h"

void *func_80059520(s32 index)
{
    ModelSlot *entry = &D_800F2C40[index];
    s32 remainder = entry->field_DC0[7] % 6;
    u8 *descriptor = entry->field_DC0;

    if (remainder != 0 && descriptor[3] == 0) {
        descriptor += 4;
    }
    return descriptor;
}

void func_80059590(
    s32 index,
    s32 fourth,
    s32 first,
    s32 second,
    s32 third)
{
    ModelSlot *entry = &D_800F2C40[index];

    entry->field_DC0[3] = fourth;
    entry->field_DC0[0] = first;
    entry->field_DC0[1] = second;
    entry->field_DC0[2] = third;
}
