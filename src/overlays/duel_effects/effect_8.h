#ifndef MEMORIES_DECOMP_DUEL_EFFECT_8_H
#define MEMORIES_DECOMP_DUEL_EFFECT_8_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
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
    CVECTOR color;
    u16 initial_width;
    u16 width_step;
    u16 minimum_width;
    u16 initial_height;
    u16 height_step;
    u16 maximum_height;
    u16 sprite_size;
    u16 ray_count;
    u16 ray_range;
    u16 ray_width;
    u16 particle_speed;
    u16 particle_count;
    u16 draw_rings;
    u16 radii[3];
    u16 ring_height;
} DuelEffect8Config;

typedef struct {
    DuelEffect8Config *G32 config;
    SVECTOR rays[32];
    SVECTOR positions[64];
    SVECTOR velocities[64];
    SVECTOR rings[3][32];
    SVECTOR origin;
    u16 width;
    u16 height;
    u32 scale;
    u32 frame;
    u16 cross_frame;
    CVECTOR color;
} DuelEffect8Work;

extern VECTOR D_80146014;
extern DuelEffect8Config D_8015A514[6];

void func_80147B18(void *buffer, s32 phase);

#endif
