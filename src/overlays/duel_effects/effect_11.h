#ifndef MEMORIES_DECOMP_DUEL_EFFECT_11_H
#define MEMORIES_DECOMP_DUEL_EFFECT_11_H

#include "../../types.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "drawing_tail.h"
#include "layered_drawing.h"
#include "color_helpers.h"
#include "drawing_helpers.h"
#include "image_inputs.h"
#include "../../unmatched.h"

typedef struct {
    u16 fragment_size;
    u16 speed;
    u16 lift;
    u16 chance;
    u16 spread;
    u16 field_0A;
    CVECTOR color;
    u16 ray_width;
    u16 ray_count;
    u16 ray_length;
    u16 dust_count;
    u16 dust_speed;
    u16 glow_size;
    u16 width;
    u16 shrink;
    u16 min_width;
    u16 height;
    u16 growth;
    u16 max_height;
    u16 radii[3];
    u16 ring_height;
} DuelEffect11Config;

typedef struct {
    DuelEffect11Config *G32 config;
    SVECTOR positions[4][7];
    SVECTOR velocities[4][7];
    SVECTOR rotations[4][7];
    SVECTOR rotation_steps[4][7];
    SVECTOR particles[4][7][16];
    SVECTOR rays[32];
    SVECTOR dust[64];
    SVECTOR dust_velocities[64];
    SVECTOR rings[3][32];
    SVECTOR origin;
    u16 uv[2];
    u16 states[4][7];
    u16 frames[4][7];
    u16 sizes[4][7][16];
    u16 chances[4][7];
    u16 angle_sums[4][7];
    u16 completed;
    u16 modes[4][7];
    u32 scale;
    u32 tick;
    u16 cross;
    u16 width;
    u16 height;
    u16 base_texture[2];
    u16 texture[2];
    CVECTOR colors[4][7];
    CVECTOR particle_colors[4][7];
    CVECTOR glow;
} DuelEffect11Work;

extern VECTOR D_80146004;
extern DuelEffect11Config D_8015A4E4;

void func_80146760(void *buffer, s32 phase);

#endif
