#ifndef MEMORIES_MODEL_VARIANT408_ENTRY_H
#define MEMORIES_MODEL_VARIANT408_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/gpu_packets.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"

typedef struct {
    SVECTOR points[2][4][6];
    CVECTOR color;
    VECTOR position;
    s32 phase;
    s32 repeats;
} ModelVariant408Ring;

typedef struct {
    SVECTOR points[9];
} ModelVariant408MeshRow;

typedef struct {
    ModelVariant408MeshRow rows[17];
    CVECTOR colors[9];
    VECTOR scale;
    s32 phases[17];
    s32 repeats[17];
} ModelVariant408Mesh;

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
    ModelVariantPackets packets;
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

extern GsIMAGE gSpanishModelVariant408_aImages[];
extern ModelVariant408EntryConfig gSpanishModelVariant408_aConfigs[];
void SpanishModelVariant408_DrawMesh(ModelVariant408EntryState *context);
void SpanishModelVariant408_DrawRingLines(ModelVariant408EntryState *context);

#endif
