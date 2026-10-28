#ifndef MEMORIES_DECOMP_MODEL_VARIANT372_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT372_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libhmd.h"

typedef struct {
    char text[23];
} Model372Path23;

typedef struct {
    char text[25];
} Model372Path25;

typedef struct {
    CVECTOR color;
    s32 field_04;
    u8 parts[8];
    s32 field_10;
    s16 field_14, field_16;
    s32 paired;
    s32 single;
    s32 offset[3];
    u32 sheet_start;
    u32 ribbon_start;
    u32 field_34, field_38, field_3C, field_40;
} Model372Config;

typedef struct {
    SVECTOR a[17];
    s32 sa[17];
    s32 angle[17];
    SVECTOR b[17];
    s32 sb[17];
    s32 width[17];
    CVECTOR tint[17];
    CVECTOR color[17];
    VECTOR scale;
    s16 rotation_x, rotation_y;
    s32 rotation_z;
    VECTOR offset;
    s32 flag[17];
    s32 depth[17];
    s16 ox[17];
    s16 oy[17];
    s32 progress[17];
    s32 done;
} Model372Ribbon;

typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    CVECTOR outer;
    CVECTOR inner;
    s32 size;
    s32 field_8C;
    s32 field_90;
    s32 field_94;
    s32 shown;
} Model372Sheet;

typedef struct {
    Model372Ribbon ribbons[8];
    Model372Sheet sheets[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_GT4 extra;
    u8 unknown_29BC[0x18];
    POLY_FT4 flat[2];
    u8 unknown_2A24[0x10];
    MATRIX matrix;
    SVECTOR target;
    MATRIX transforms[8];
    VECTOR positions[8];
    VECTOR directions[8];
    VECTOR view;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame;
    u32 time;
    s32 last_frame;
    s32 step;
    s32 fade;
    Model372Config *G32 config;
    u8 unknown_2CA0[4];
    GsCOORDUNIT *G32 parts[8];
    SVECTOR spin;
    s32 angle_a;
    s32 angle_b;
    SVECTOR camera;
    s32 ambient;
    s32 phase;
    s16 slot;
    u16 command;
} Model372State;

extern Model372Path23 D_8013CE10;
extern Model372Path25 D_8013CE28;
extern GsIMAGE D_8013CE44[];
extern Model372Config D_8013CF40[];

void func_8013BE68(u8 *context);
void func_8013C828(u8 *context);
s32 func_8013B004(u8 *context, s32 command);

#endif
