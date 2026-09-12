#include "../types.h"

#include "model.h"
#include "model_copy_slot_u16_values.h"
#include "model_init_light_triplet.h"
#include "model_slot_data.h"
#include "model_slot_support.h"
#include "../unmatched.h"

void *func_80058F20(s32 index, s32 slot)
{
    ModelSlot *entry = &D_800F2C40[index];

    if (slot > entry->entry_count) {
        slot = entry->field_E18;
    }
    return entry->entries + slot * MODEL_SLOT_DATA_ENTRY_SIZE;
}

u32 func_80058F74(s32 index)
{
    ModelSlot *entry = &D_800F2C40[index];

    return (u32)entry->entries +
           entry->field_E18 * MODEL_SLOT_DATA_ENTRY_SIZE;
}

void Model_CopySlotU16Values(s32 idx, u16 *out) {
    ModelSlot *rec = &D_800F2C40[idx];
    out[0] = rec->field_DD0[0];
    out[1] = rec->field_DD0[1];
    out[2] = rec->field_DD0[2];
    out[3] = rec->field_DD0[3];
}

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

// Per-index (stride 0xE20) table; this function seeds 3 records of
// {s32,s32,s32,u8,u8,u8} at +0xD70/+0xD80/+0xD90 -- looks like X/Y/Z offsets
// plus an intensity-byte triple for a 3-point light/particle rig.
void Model_InitLightTriplet(s32 index)
{
    ModelSlot *slot = &D_800F2C40[index];

    slot->field_D70[0].field_00 = 3000;
    slot->field_D70[0].field_04 = 5000;
    slot->field_D70[0].field_08 = 2000;
    slot->field_D70[0].field_0C[0] = MODEL_LIGHT_BASE_INTENSITY;
    slot->field_D70[0].field_0C[1] = MODEL_LIGHT_BASE_INTENSITY;
    slot->field_D70[0].field_0C[2] = MODEL_LIGHT_BASE_INTENSITY;

    slot->field_D70[1].field_00 = -3000;
    slot->field_D70[1].field_04 = 5000;
    slot->field_D70[1].field_08 = -2000;
    slot->field_D70[1].field_0C[0] = MODEL_LIGHT_BASE_INTENSITY;
    slot->field_D70[1].field_0C[1] = MODEL_LIGHT_BASE_INTENSITY;
    slot->field_D70[1].field_0C[2] = MODEL_LIGHT_BASE_INTENSITY;

    slot->field_D70[2].field_00 = 0;
    slot->field_D70[2].field_04 = -5000;
    slot->field_D70[2].field_08 = 0;
    slot->field_D70[2].field_0C[0] = MODEL_LIGHT_DIM_INTENSITY;
    slot->field_D70[2].field_0C[1] = MODEL_LIGHT_DIM_INTENSITY;
    slot->field_D70[2].field_0C[2] = MODEL_LIGHT_DIM_INTENSITY;
}

/* This is D_800F2C40[0].field_D70, but spelling the base that way materializes
   the 0xD70 offset and makes this function one instruction longer. */
unsigned char *func_800591C0(unsigned int index, unsigned int slot)
{
    register u8 *entry;

    if (slot >= 3) {
        slot = 0;
    }
    entry = (u8 *)D_800F39B0 + index * MODEL_SLOT_SIZE;

    return entry + slot * sizeof(ModelSlotLightEntry);
}
