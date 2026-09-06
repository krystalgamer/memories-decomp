#include "../types.h"

#include "model.h"

typedef struct {
    u8 bytes[MODEL_SLOT_SIZE];
} ModelSlotTransformEntry;

typedef ModelSlotS32Quad ModelSlotTransform;

extern ModelSlotTransformEntry D_800F39F0[];

ModelSlotTransformEntry *func_8005949C(int index)
{
    return &D_800F39F0[index];
}

void func_800594C0(int index, ModelSlotTransform *source)
{
    ModelSlot *entry = &D_800F2C40[index];

    if (source != 0) {
        entry->field_DB0 = *source;
    } else {
        entry->field_DB0.field_08 = MODEL_FIXED_ONE;
        entry->field_DB0.field_04 = MODEL_FIXED_ONE;
        entry->field_DB0.field_00 = MODEL_FIXED_ONE;
    }
}

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
