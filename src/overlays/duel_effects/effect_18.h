#ifndef MEMORIES_DECOMP_DUEL_EFFECT_18_H
#define MEMORIES_DECOMP_DUEL_EFFECT_18_H

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
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    CVECTOR color;
    u16 sprite_size;
    u16 radii[3];
    u16 widths[2];
    u16 height;
    u16 particle_speed;
    u16 outer_radii[3];
    u16 variant;
    u16 delay;
} DuelEffect18Config;

typedef struct {
    DuelEffect18Config *G32 config;
    SVECTOR rings[3][32];
    SVECTOR rotations[16];
    SVECTOR quads[2][4];
    SVECTOR positions[32];
    SVECTOR velocities[32];
    SVECTOR outer_rings[3][32];
    u16 cross_frame;
    u16 padding8C6;
    u32 scale;
    u32 frame;
    CVECTOR color;
} DuelEffect18Work;

extern VECTOR D_801461D8;
extern DuelEffect18Config D_8015B078[2];

void func_80154084(void *buffer, s32 phase);

#endif
