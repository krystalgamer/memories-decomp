#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_slot_properties.h"
#include "../unmatched.h"

/* &D_800F2C40[0].field_DB0. The interior alias keeps the retail address
 * construction; MODEL_SLOT_SIZE preserves the stride between slots. */

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

ModelSlotS32Quad *func_8005949C(s32 index)
{
    return (ModelSlotS32Quad *)(
        (u8 *)&D_800F39F0 + index * MODEL_SLOT_SIZE
    );
}

void func_800594C0(s32 index, ModelSlotS32Quad *source)
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

u8 *func_80059520(s32 index)
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

void func_800595C8(s32 index, s32 x, s32 y, s32 z)
{
    ModelSlot *record = &D_800F2C40[index];

    x = x < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (x > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : x);
    record->field_DA0[0] = x;
    y = y < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (y > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : y);
    record->field_DA0[1] = y;
    z = z < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (z > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : z);
    record->field_DA0[2] = z;
    if (record->field_E11 != 4) {
        if (x == MODEL_FIXED_HALF && y == x && z == y)
            record->field_E11 = 0;
        else
            record->field_E11 = 3;
    }
}
