#ifndef MEMORIES_DECOMP_DUEL_EFFECT_7_H
#define MEMORIES_DECOMP_DUEL_EFFECT_7_H

#include "../../types.h"
#include "effect_6.h"

typedef struct {
    u8 color[3];
    u8 number_color[3];
    u16 count;
    u16 size;
    u16 duration;
    s16 start_y;
    u16 growth;
    u16 radii[3];
    u16 particle_speed;
    u16 particle_count;
    u16 particle_size;
    s16 number;
} DuelEffect7Config;

typedef struct {
    DuelEffect7Config *G32 config;
    SVECTOR trails[16][4];
    SVECTOR velocities[16];
    SVECTOR rings[3][32];
    SVECTOR particles[16][32];
    SVECTOR particle_velocities[16][32];
    SVECTOR number_position;
    SVECTOR number_velocity;
    u16 sizes[16];
    u32 scales[16];
    u32 frame;
    u32 tick;
    u16 active;
    u16 bounces;
    u16 states[16];
    u16 number_mode;
    u16 ages[16];
    u16 cross_frame;
    CVECTOR colors[16];
    CVECTOR background_color;
    CVECTOR number_color;
} DuelEffect7Work;

extern VECTOR D_80146228;
extern DuelEffect7Config D_8015B5B8[5];

void func_801587D8(void *buffer, s32 phase);

#endif
