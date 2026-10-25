#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_slot_properties.h"
#include "../unmatched.h"

/* &gModel_aSlots[0].field_DB0. The interior alias keeps the retail address
 * construction; MODEL_SLOT_SIZE preserves the stride between slots. */

/* RotTrans types the last three: the vertex run reached through the slot's
   part chain is SVECTOR, `out` is the VECTOR it transforms into, and the local
   scratch is the flag pair. GsGetLwUnit and GsSetLsMatrix likewise agree that
   the 32-byte local is a MATRIX. */
s32 Model_TransformPartVertex(s32 arg0, s32 arg1, s32 arg2, VECTOR *out)
{
    MATRIX sp10;
    PSXLONG sp30[2];
    ModelSlot *p;
    GsUNIT *e;
    u32 *q;
    SVECTOR *base;

    p = &gModel_aSlots[arg0];
    /* The part's GsUNIT, reached as an integer sum: indexed as
       &p->field_000[arg1 + 1], GCC folds the +8 into the load and drops
       the instruction retail spends on it. */
    e = (GsUNIT *)((s32)p->field_000 + (arg1 + 1) * (s32)sizeof(GsUNIT));
    q = (u32 *)e->primtop[1];
    base = (SVECTOR *)q[2];

    PushMatrix();
    GsGetLwUnit(
        (GsCOORDUNIT *)p->entries + arg1,
        &sp10
    );
    GsSetLsMatrix(&sp10);
    RotTrans(&base[arg2], out, sp30);
    PopMatrix();
    return sp30[0];
}

ModelSlotS32Quad *Model_GetSlotScale(s32 index)
{
    return (ModelSlotS32Quad *)(
        (u8 *)&gModel_Slot0Scale + index * MODEL_SLOT_SIZE
    );
}

void Model_SetSlotScale(s32 index, ModelSlotS32Quad *source)
{
    ModelSlot *entry = &gModel_aSlots[index];

    if (source != 0) {
        entry->field_DB0 = *source;
    } else {
        entry->field_DB0.field_08 = MODEL_FIXED_ONE;
        entry->field_DB0.field_04 = MODEL_FIXED_ONE;
        entry->field_DB0.field_00 = MODEL_FIXED_ONE;
    }
}

u8 *Model_GetSlotTintDescriptor(s32 index)
{
    ModelSlot *entry = &gModel_aSlots[index];
    s32 remainder = entry->tint[7] % 6;
    u8 *descriptor = entry->tint;

    if (remainder != 0 && descriptor[3] == 0) {
        descriptor += 4;
    }
    return descriptor;
}

void Model_SetSlotTintTarget(
    s32 slot,
    s32 mode,
    s32 target0,
    s32 target1,
    s32 target2)
{
    ModelSlot *entry = &gModel_aSlots[slot];

    entry->tint[3] = mode;
    entry->tint[0] = target0;
    entry->tint[1] = target1;
    entry->tint[2] = target2;
}

void Model_SetSlotAmbient(s32 index, s32 x, s32 y, s32 z)
{
    ModelSlot *record = &gModel_aSlots[index];

    x = x < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (x > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : x);
    record->ambient_rgb_scale[0] = x;
    y = y < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (y > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : y);
    record->ambient_rgb_scale[1] = y;
    z = z < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (z > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : z);
    record->ambient_rgb_scale[2] = z;
    if (record->draw_mode_request != 4) {
        if (x == MODEL_FIXED_HALF && y == x && z == y)
            record->draw_mode_request = 0;
        else
            record->draw_mode_request = 3;
    }
}
