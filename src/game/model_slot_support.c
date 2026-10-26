#include "../types.h"
#include "model.h"
#include "model_copy_slot_position.h"
#include "model_init_light_triplet.h"
#include "model_slot_data.h"
#include "../game/model_draw_slot.h"
#include "screen_projection.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"
#include "model_slot_support.h"
#include "../unmatched.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model_update_coordinate_unit_matrix.h"
#include "../psyq/memory.h"
#include "model_slot_properties.h"

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

void Model_CopySlotPosition(s32 idx, u16 *out) {
    ModelSlot *rec = &gModel_aSlots[idx];
    out[0] = rec->cached_position[0];
    out[1] = rec->cached_position[1];
    out[2] = rec->cached_position[2];
    out[3] = rec->cached_position[3];
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

/* Rebuilds one coordinate unit's local matrix from its Euler angles, scales it
 * when the caller supplies a scale vector, and clears the stamp so libgs
 * recomputes the world matrix on the next pass. */
void Model_UpdateCoordinateUnitMatrix(GsCOORDUNIT *unit, VECTOR *scale)
{
    RotMatrixYXZ_gte(&unit->rot, &unit->matrix);
    if (scale != 0) ScaleMatrix(&unit->matrix, scale);
    unit->flg = 0;
}

void Model_SetSlotDrawMode(int index, int value)
{
    gModel_aSlots[index].draw_mode_request = value;
}

/* Builds `unit`'s local matrix from a camera-relative direction vector
   (derived from gModel_wViewYaw/gModel_wViewPitch) combined with the per-slot base
   angles on gModel_aSlots[arg0]'s own coordinate unit, then parents it. */
void Model_BuildCameraRelativeCoordinateUnit(
    s32 arg0,
    GsCOORDUNIT *parent,
    GsCOORDUNIT *unit
) {
    SVECTOR packed;
    SVECTOR raw;
    MATRIX sp20;
    ModelSlot *slot;

    memset(&raw, 0, sizeof(raw));
    raw.vy = (s16)(MODEL_ANGLE_FULL_TURN - gModel_wViewYaw);
    raw.vz = (s16)gModel_wViewPitch;

    slot = &gModel_aSlots[arg0];

    packed = raw;

    unit->flg = 0;
    unit->rot.vx =
        (s16)(MODEL_ANGLE_FULL_TURN - slot->root_coordinate->rot.vx);
    unit->rot.vy =
        (s16)(MODEL_ANGLE_FULL_TURN - slot->root_coordinate->rot.vy);
    unit->rot.vz =
        (s16)(MODEL_ANGLE_FULL_TURN - slot->root_coordinate->rot.vz);

    RotMatrixZXY(&unit->rot, &unit->matrix);
    RotMatrix_gte(&packed, &sp20);
    MulMatrix(&unit->matrix, &sp20);

    unit->matrix.t[2] = 0;
    unit->matrix.t[1] = 0;
    unit->matrix.t[0] = 0;
    unit->super = parent;
}

/* &gModel_aSlots[0].render_scale. The interior alias keeps the retail address
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
       &p->hmd_units[arg1 + 1], GCC folds the +8 into the load and drops
       the instruction retail spends on it. */
    e = (GsUNIT *)((s32)p->hmd_units + (arg1 + 1) * (s32)sizeof(GsUNIT));
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
        entry->render_scale = *source;
    } else {
        entry->render_scale.field_08 = MODEL_FIXED_ONE;
        entry->render_scale.field_04 = MODEL_FIXED_ONE;
        entry->render_scale.field_00 = MODEL_FIXED_ONE;
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
