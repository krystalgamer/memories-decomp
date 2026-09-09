#include "../types.h"
#include "model.h"
#include "model_init_light_triplet.h"

//@ target 49934 8C
//@ sym D_800F2C40=0x800F2C40
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
