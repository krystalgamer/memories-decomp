#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "model.h"

typedef struct {
    u8 bytes[MODEL_SLOT_SIZE];
} ModelSlotTransformEntry;

typedef ModelSlotS32Quad ModelSlotTransform;

extern void func_8008A4A0(u8 *, u8 *);
extern void func_80087970(u8 *, s32, s32 *);
extern ModelSlotTransformEntry D_800F39F0[];

s32 func_800593D0(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    u8 sp10[32];
    s32 sp30[2];
    u8 *p;
    u8 *e;
    u8 *q;
    u8 *base;

    p = (u8 *)&D_800F2C40[arg0];
    e = p + (arg1 + 1) * 8;
    q = *(u8 **)(e + 4);
    q = *(u8 **)(q + 4);
    base = *(u8 **)(q + 8);

    PushMatrix();
    func_8008A4A0(
        *(u8 **)(p + 0xD14) + arg1 * MODEL_SLOT_DATA_ENTRY_SIZE,
        sp10
    );
    GsSetLsMatrix((MATRIX *)sp10);
    func_80087970(base + arg2 * 8, arg3, sp30);
    PopMatrix();
    return sp30[0];
}

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
