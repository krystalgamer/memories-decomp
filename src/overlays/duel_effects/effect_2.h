#ifndef MEMORIES_DECOMP_DUEL_EFFECT_2_H
#define MEMORIES_DECOMP_DUEL_EFFECT_2_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "drawing_tail.h"
#include "layered_drawing.h"
#include "drawing_helpers.h"
#include "textured_quads.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    u8 color[3];
    u8 number_red[2];
    u8 number_green[2];
    u8 number_blue[2];
    u16 sprite_size;
    u16 radii[3];
    s16 heights[4];
    s16 widths[5];
    u16 particle_speed;
    u16 particle_count;
    u16 variant;
    u16 strip_widths[2];
    u16 strip_height;
    u16 field_30;
} DuelEffect2Config;

typedef struct {
    DuelEffect2Config *G32 config;
    SVECTOR rings[3][32];
    SVECTOR positions[64];
    SVECTOR velocities[64];
    SVECTOR rotations[64];
    u32 scale;
    u32 frame;
    u16 stage;
    u16 number_state;
    s16 number;
    u16 cross_frame;
    CVECTOR color;
    CVECTOR background_color;
    CVECTOR number_color;
} DuelEffect2Work;

extern VECTOR D_801461B8;
extern DuelEffect2Config D_8015AF18[7];
void func_801566D4(s32 value, u8 *color, SVECTOR *offset, u16 mode,
                   u16 size, u16 bias);

void func_80153200(void *buffer, s32 phase, s16 number);

#endif
