#ifndef MEMORIES_DECOMP_DUEL_EFFECT_10_H
#define MEMORIES_DECOMP_DUEL_EFFECT_10_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"
#include "layered_drawing.h"
#include "drawing_tail.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    CVECTOR color;
    u16 height_step;
    u16 maximum_height;
    u16 particle_speed;
    u16 mode;
    u16 field_0C;
} DuelEffect10Config;

typedef struct {
    DuelEffect10Config *G32 config;
    SVECTOR rings[2][4];
    SVECTOR plane[4];
    SVECTOR particles[64];
    SVECTOR columns[64];
    u16 speeds[64];
    u16 ages[64];
    u16 height;
    u16 stage;
    u16 color_ready;
    u16 ring_ready;
    u16 spawned;
    u32 frame;
    u32 tick;
    u16 cross_frame;
    CVECTOR color;
    CVECTOR screen_color;
    CVECTOR ring_color;
    CVECTOR column_colors[64];
    CVECTOR plane_color;
} DuelEffect10Work;

extern VECTOR D_80146138;
extern GsIMAGE D_8015A8C8[21];
extern DuelEffect10Config D_8015AB14[6];

void func_8014C8FC(void *buffer, s32 phase);

#endif
