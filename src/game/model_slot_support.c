#include "../types.h"
#include "model.h"
#include "model_copy_slot_u16_values.h"
#include "model_init_light_triplet.h"
#include "model_slot_data.h"
#include "../game/model_draw_slot.h"
#include "screen_projection.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"
#include "model_slot_support.h"
#include "../unmatched.h"

void *Model_GetSlotDataEntry(s32 index, s32 slot)
{
    ModelSlot *entry = &gModel_aSlots[index];

    if (slot > entry->entry_count) {
        slot = entry->root_coordinate_index;
    }
    return entry->entries + slot * MODEL_SLOT_DATA_ENTRY_SIZE;
}

u32 Model_GetCurrentDataEntry(s32 index)
{
    ModelSlot *entry = &gModel_aSlots[index];

    return (u32)entry->entries +
           entry->root_coordinate_index * MODEL_SLOT_DATA_ENTRY_SIZE;
}

void Model_CopySlotU16Values(s32 idx, u16 *out) {
    ModelSlot *rec = &gModel_aSlots[idx];
    out[0] = rec->field_DD0[0];
    out[1] = rec->field_DD0[1];
    out[2] = rec->field_DD0[2];
    out[3] = rec->field_DD0[3];
}

void Model_GetSlotDimensionsWithOverrides(s32 index, s16 *output)
{
    register ModelSlot *entry = &gModel_aSlots[index];
    register u8 *source = entry->field_CF8.prefix.bytes.field_00;
    s32 value;

    *(ModelBytes8 *)output = *(ModelBytes8 *)entry->cached_dimensions;
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

void Model_DrawSlotUnitsOnly(s32 index)
{
    ModelSlot *entry = &gModel_aSlots[index];
    u8 old = entry->draw_mode_request;

    entry->draw_mode_request = 1;
    Model_DrawSlot(index);
    entry->draw_mode_request = old;
}

/* Three directional-light records consumed by GsSetFlatLight, not positions. */
void Model_InitLightTriplet(s32 index)
{
    ModelSlot *slot = &gModel_aSlots[index];

    slot->flat_lights[0].direction_x = 3000;
    slot->flat_lights[0].direction_y = 5000;
    slot->flat_lights[0].direction_z = 2000;
    slot->flat_lights[0].rgb[0] = MODEL_LIGHT_BASE_INTENSITY;
    slot->flat_lights[0].rgb[1] = MODEL_LIGHT_BASE_INTENSITY;
    slot->flat_lights[0].rgb[2] = MODEL_LIGHT_BASE_INTENSITY;

    slot->flat_lights[1].direction_x = -3000;
    slot->flat_lights[1].direction_y = 5000;
    slot->flat_lights[1].direction_z = -2000;
    slot->flat_lights[1].rgb[0] = MODEL_LIGHT_BASE_INTENSITY;
    slot->flat_lights[1].rgb[1] = MODEL_LIGHT_BASE_INTENSITY;
    slot->flat_lights[1].rgb[2] = MODEL_LIGHT_BASE_INTENSITY;

    slot->flat_lights[2].direction_x = 0;
    slot->flat_lights[2].direction_y = -5000;
    slot->flat_lights[2].direction_z = 0;
    slot->flat_lights[2].rgb[0] = MODEL_LIGHT_DIM_INTENSITY;
    slot->flat_lights[2].rgb[1] = MODEL_LIGHT_DIM_INTENSITY;
    slot->flat_lights[2].rgb[2] = MODEL_LIGHT_DIM_INTENSITY;
}

/* Returns flat light `light` of model slot `index`: Model_DrawSlot and
   Model_DrawBackground hand the three flat_lights records to
   GsSetFlatLight(0..2). This is gModel_aSlots[0].flat_lights, but spelling the
   base that way materializes the 0xD70 offset and makes this function one
   instruction longer. */
unsigned char *Model_GetFlatLight(unsigned int index, unsigned int light)
{
    register u8 *entry;

    if (light >= 3) {
        light = 0;
    }
    entry = (u8 *)gModel_aSlot0FlatLights + index * MODEL_SLOT_SIZE;

    return entry + (u32)&((ModelSlotLightEntry *)0)[light];
}

GsRVIEW2 *Model_GetCameraViewBuffer(void)
{
    return &gModel_RView;
}

ModelViewMetrics *Model_GetViewMetricsBuffer(void)
{
    return (ModelViewMetrics *)&gModel_wViewDistance;
}

GsCOORDUNIT *Model_GetCameraCoordinateUnit(void)
{
    return &gModel_CameraCoordinateUnit;
}

void *Model_GetLightSourceMatrix(void)
{
    return &gGraphics_LightSourceMatrix;
}
