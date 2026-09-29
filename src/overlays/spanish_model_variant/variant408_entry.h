#ifndef MEMORIES_MODEL_VARIANT408_ENTRY_H
#define MEMORIES_MODEL_VARIANT408_ENTRY_H

#include "../../types.h"
#include "variant408_update.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../game/func_80058E1C.h"
#include "../../psyq/libhmd.h"

typedef struct {
    s32 projected;
    s32 interpolation;
    s32 flag;
    s32 target_projected;
} ModelVariant408Projection;

typedef struct {
    u8 unknown_00[4];
    u8 part;
    u8 unknown_05[7];
    u32 start;
    u32 full;
    u32 cycle_start;
    u8 unknown_18[12];
} ModelVariant408EntryConfig;

typedef struct {
    ModelVariant408Mesh meshes[1];
    ModelVariant408Ring rings[4];
    POLY_F4 flat;
    u8 unknown_C0C[4];
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 other;
    POLY_GT4 opaque;
    POLY_FT4 sprites[2];
    u8 unknown_D54[16];
    GsGLINE line;
    MATRIX matrix;
    SVECTOR target;
    VECTOR delta;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    s32 frame;
    s32 elapsed;
    s32 animation_frame;
    s32 step;
    ModelVariant408EntryConfig *config;
    u8 unknown_DE0[4];
    void *part;
    s32 extension;
    s32 angle;
    s32 repeat_limit;
    s16 field_DF4;
    s16 field_DF6;
    s32 field_DF8;
    s32 field_DFC;
    s32 state;
    s32 fade;
    u8 unknown_E08[8];
    s32 brightness;
    s16 slot;
    s16 command;
} ModelVariant408EntryState;

extern GsIMAGE D_8013C470[];
extern ModelVariant408EntryConfig D_8013C550[];
void func_8013B93C(u8 *context);
void func_8013C02C(u8 *context);

#endif
