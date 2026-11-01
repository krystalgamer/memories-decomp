#ifndef MEMORIES_DECOMP_MODEL_VARIANT386_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT386_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_slot_properties.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 unknown_00[8];
    s32 parts[5];
    s32 extra;
    u8 unknown_20[8];
    s32 count;
    s32 field_2C;
    u32 sheet_start;
    u32 shrink_start;
    u32 shrink_end;
} Model386Config;

typedef struct {
    SVECTOR pos[7];
    PSXLONG sxy[7];
    s32 angle[7];
    SVECTOR edge[7];
    PSXLONG edge_sxy[7];
    s32 width[7];
    CVECTOR color;
    CVECTOR tint;
    VECTOR scale;
    SVECTOR rotation;
    u8 unknown_100[0x38];
    VECTOR offset;
    s32 depth[7];
    s16 dx[7];
    s16 dy[7];
} Model386Strand;

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
} Model386Sheet;

typedef struct {
    u8 unknown_000[0x144];
    CVECTOR color[9];
    s32 stage;
    s32 dir[3];
    u8 unknown_178[4];
    s32 start;
    s32 end;
    s32 yaw;
    s32 pitch;
    s32 wave;
    s32 sway;
    u8 unknown_194[0x48];
} Model386Chain;

typedef struct {
    Model386Strand strands[5];
    Model386Sheet sheets[5];
    Model386Chain chains[3];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[6];
    u8 unknown_11A4[0x10];
    MATRIX matrix;
    SVECTOR target;
    MATRIX transforms[5];
    VECTOR positions[5];
    VECTOR directions[5];
    s16 dx[5];
    s16 dy[5];
    s32 spin;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame;
    u32 time;
    s32 last_frame;
    u32 step;
    u32 fade_span;
    u32 shrink_span;
    u32 fade;
    Model386Config *G32 config;
    u8 unknown_136C[4];
    GsCOORDUNIT *G32 parts[5];
    GsCOORDUNIT *G32 extra;
    s16 length[5];
    s16 reach[5];
    s16 mode[5];
    s16 strand_fade[5];
    s16 field_13B0;
    s16 field_13B2;
    s32 width;
    s32 sway;
    s32 twist;
    s32 phase;
    u8 unknown_13C4[8];
    s32 ambient;
    s16 slot;
    u16 command;
} Model386State;

extern GsIMAGE gModelVariant386_D2C54[];
extern Model386Config gModelVariant386_D2C8C[];

void FrenchModelVariant403_DrawStrands(u8 *context);
void FrenchModelVariant403_DrawSheets(u8 *context);
void FrenchModelVariant403_DrawChains(u8 *context);
s32 ModelVariant386_HandleRequest(u8 *context, s32 command);

#endif
