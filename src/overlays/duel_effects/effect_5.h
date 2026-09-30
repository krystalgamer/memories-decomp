#ifndef MEMORIES_DECOMP_DUEL_EFFECT_5_H
#define MEMORIES_DECOMP_DUEL_EFFECT_5_H

#include "../../types.h"
#include "effect_6.h"

typedef struct {
    u8 color[3];
    u8 number_color[3];
    u16 rise_speed;
    u16 particle_speed;
    u16 strip_count;
    u16 widths[2];
    u16 height;
    u16 radii[3];
    u16 sprite_size;
    u16 bounce_speed;
    s32 number;
    u16 particle_count;
    u16 field_22;
} DuelEffect5Config;

typedef struct {
    DuelEffect5Config *G32 config;
    SVECTOR rising_positions[64];
    SVECTOR positions[64];
    SVECTOR velocities[64];
    SVECTOR rings[3][32];
    SVECTOR rotations[64];
    SVECTOR number_position;
    SVECTOR number_velocity;
    u32 scale;
    u32 frame;
    u32 tick;
    u16 number_state;
    u16 spawned;
    u16 bounces;
    u16 hold_frames;
    u16 variant;
    u16 cross_frame;
    CVECTOR rising_colors[64];
    CVECTOR background_color;
    CVECTOR color;
    CVECTOR number_color;
} DuelEffect5Work;

extern VECTOR D_80146218;
extern DuelEffect5Config D_8015B450[10];

void func_80157E10(void *buffer, s32 phase);

#endif
