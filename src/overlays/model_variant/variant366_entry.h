#ifndef MEMORIES_MODEL_VARIANT366_ENTRY_H
#define MEMORIES_MODEL_VARIANT366_ENTRY_H
#include "../../types.h"
#include "model_variant.h"
#include "../../game/camera_view.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"

/* One 0x18-byte command record of the table at D_8013E558. */
typedef struct {
    u8 unknown_00[4];
    u8 parts[3], unknown_07[5];
    u16 count, unknown_0E;
    u8 unknown_10[4];
    u32 end;
} Variant366Config;

/* The strip drawn by func_8013BCC0. */
typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color;
    VECTOR scale;
    s16 field_5C, field_5E;
    s32 field_60;
    u8 unknown_64[8];
} Variant366Strip;

/* The four-quadrant quad group drawn by func_8013C918. */
typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    CVECTOR outer, inner;
} Variant366Quads;

/* The three 17-point ribbon cones drawn by func_8013CEC8. */
typedef struct {
    SVECTOR inner[17], middle[17], outer[17];
    s32 progress, field_19C;
} Variant366Ribbon;

/* The four eight-point rings. */
typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR inner_color, outer_color;
    s32 size, state;
} Variant366Ring;

/* The sixteen rays drawn by func_8013D908. */
typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2], outer[2];
    u8 unknown_60[0x1C];
} Variant366Ray;

typedef struct {
    Variant366Strip strips[1];
    Variant366Quads quads[1];
    u8 unknown_F4[0x360];
    Variant366Ribbon ribbons[3];
    Variant366Ring rings[4];
    Variant366Ray rays[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_F4 mask;
    POLY_GT4 textured[2], plain, paged;
    POLY_FT4 flat[2];
    u8 unknown_14AC[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant366Config *G32 config;
    u8 unknown_153C[4];
    GsCOORDUNIT *G32 parts[3];
    s32 index;
    s32 field_1550, field_1554;
    s32 field_1558[3];
    s16 field_1564, field_1566, field_1568, field_156A, field_156C, field_156E;
    s16 field_1570, field_1572, field_1574, unknown_1576;
    s16 field_1578, field_157A;
    s32 field_157C, phase, field_1584;
    u8 unknown_1588[8];
    s32 tint;
    s16 slot, command;
} Variant366State;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} Variant366Projection;

extern GsIMAGE D_8013E45C[];
extern Variant366Config D_8013E558[];

s32 func_8013B004(u8 *context, s32 command);
void func_8013C204(u8 *context);
void func_8013C918(u8 *context);
void func_8013CEC8(u8 *context);
void func_8013D908(u8 *context);
#endif
