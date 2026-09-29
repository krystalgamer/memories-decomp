#ifndef MEMORIES_DECOMP_DUEL_EFFECT_4_H
#define MEMORIES_DECOMP_DUEL_EFFECT_4_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "layered_drawing.h"
#include "drawing_tail.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    CVECTOR color;
    u16 sprite_size;
    u16 radii[3];
    u16 widths[2];
    u16 height;
    u16 card_height;
    u16 card_width;
    u16 texture;
    u16 particle_speed;
    u16 duration;
} DuelEffect4Config;

typedef struct {
    DuelEffect4Config *G32 config;
    SVECTOR rings[3][32];
    SVECTOR rotations[16];
    SVECTOR positions[24];
    SVECTOR velocities[24];
    SVECTOR card_rings[2][4];
    u32 scale;
    u32 frame;
    u16 stage;
    u16 cross_frame;
    CVECTOR color;
    CVECTOR card_color;
    CVECTOR screen_color;
} DuelEffect4Work;

extern VECTOR D_80146248;
extern DuelEffect4Config D_8015B704[2];

void func_80159AAC(void *buffer, s32 phase);

#endif
