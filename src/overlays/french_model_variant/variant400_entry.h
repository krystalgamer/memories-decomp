#ifndef MEMORIES_FRENCH_MODEL_VARIANT400_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT400_ENTRY_H
#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 unknown_00[4];
    u8 part, unknown_05[7];
    u32 start, field_10, end, field_18, field_1C;
} Variant400EntryConfig;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x2C8 - 0x240];
    VECTOR position;
    u8 unknown_2D8[0x370 - 0x2D8];
} Variant400EntryNode;

typedef struct {
    SVECTOR points[16];
    CVECTOR inner, outer;
    s32 field_88, field_8C, field_90;
    u8 unknown_94[4];
} Variant400EntrySheet;

typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0x334 - 0x2AC];
} Variant400EntryColumn;

typedef struct {
    Variant400EntryNode nodes[5];
    Variant400EntrySheet sheets[7];
    Variant400EntryColumn columns[2];
    u8 unknown_1BC0[0x1ED8 - 0x1BC0];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat_textured[2];
    u8 unknown_1FD0[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    u8 unknown_2018[0x10];
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, duration;
    u32 fade;
    Variant400EntryConfig *G32 config;
    u32 field_2060;
    GsCOORDUNIT *G32 part;
    s32 field_2068, field_206C, field_2070;
    s16 field_2074, field_2076;
    s32 field_2078, field_207C, field_2080, field_2084, field_2088, field_208C;
    s32 phase;
    u8 unknown_2094[8];
    s32 tint;
    s16 slot, command;
} Variant400EntryState;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} Variant400EntryProjection;

extern u8 gFrenchModelVariant400_D2748[];
void FrenchModelVariant400_DrawRibbons(u8 *context);
void FrenchModelVariant400_DrawSheets(u8 *context);
void FrenchModelVariant400_DrawStreamers(u8 *context);
s32 FrenchModelVariant400_HandleRequest(SVECTOR *point, s32 command);
#endif
