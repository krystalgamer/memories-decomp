#include "../types.h"

#include "model.h"

typedef struct {
    u8 bytes[0xE20];
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
        entry->field_DB0.field_08 = 0x1000;
        entry->field_DB0.field_04 = 0x1000;
        entry->field_DB0.field_00 = 0x1000;
    }
}
