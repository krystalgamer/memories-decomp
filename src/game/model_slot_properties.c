#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"

typedef struct {
    u8 bytes[MODEL_SLOT_SIZE];
} ModelSlotTransformEntry;

typedef ModelSlotS32Quad ModelSlotTransform;

extern ModelSlotTransformEntry D_800F39F0[];

/* RotTrans types the last three: the vertex run reached through the slot's
   part chain is SVECTOR, `out` is the VECTOR it projects into, and the local
   scratch is the flag pair. GsGetLwUnit and GsSetLsMatrix likewise agree that
   the 32-byte local is a MATRIX. No caller survives in C, so nothing outside
   this file had to change. */
s32 func_800593D0(s32 arg0, s32 arg1, s32 arg2, VECTOR *out)
{
    MATRIX sp10;
    long sp30[2];
    u8 *p;
    u8 *e;
    u8 *q;
    SVECTOR *base;

    p = (u8 *)&D_800F2C40[arg0];
    e = p + (arg1 + 1) * 8;
    q = *(u8 **)(e + 4);
    q = *(u8 **)(q + 4);
    base = *(SVECTOR **)(q + 8);

    PushMatrix();
    GsGetLwUnit(
        (GsCOORDUNIT *)(*(u8 **)(p + 0xD14) + arg1 * MODEL_SLOT_DATA_ENTRY_SIZE),
        &sp10
    );
    GsSetLsMatrix(&sp10);
    RotTrans(&base[arg2], out, sp30);
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
