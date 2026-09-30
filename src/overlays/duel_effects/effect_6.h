#ifndef MEMORIES_DECOMP_DUEL_EFFECT_6_H
#define MEMORIES_DECOMP_DUEL_EFFECT_6_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
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
    u8 column_color[3];
    u16 count;
    u16 size;
    u16 duration;
    s16 target_y;
    u16 shrink;
    u16 radii[3];
    u16 particle_speed;
    u16 particle_count;
    u16 particle_size;
    s16 column_value;
} DuelEffect6Config;

typedef struct {
    DuelEffect6Config *G32 config;
    SVECTOR trails[16][4];
    SVECTOR velocities[16];
    SVECTOR rings[3][32];
    SVECTOR particles[16][32];
    SVECTOR particle_velocities[16][32];
    SVECTOR column;
    u16 column_step;
    u16 column_size;
    u16 sizes[16];
    u32 scales[16];
    u32 frame;
    u32 tick;
    u16 active;
    u16 hold;
    u16 states[16];
    u16 column_mode;
    u16 ages[16];
    u16 variant;
    u16 cross_frame;
    CVECTOR colors[16];
    CVECTOR background_color;
    CVECTOR column_color;
} DuelEffect6Work;

extern VECTOR D_801461F8;
extern DuelEffect6Config D_8015B30C[6];

void func_80154B30(void *buffer, s32 phase);

#endif
