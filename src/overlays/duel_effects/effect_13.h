#ifndef MEMORIES_DECOMP_DUEL_EFFECT_13_H
#define MEMORIES_DECOMP_DUEL_EFFECT_13_H

#include "../../types.h"
#include "effect_6.h"
#include "display_quads.h"
#include "../../game/screen_projection.h"

typedef struct {
    u8 color[3];
    u8 number_color[3];
    u16 radius;
    u16 height;
    u16 size;
    u16 particle_size;
    u16 blend;
    u16 background;
    s16 number;
} DuelEffect13Config;

typedef struct {
    DuelEffect13Config *G32 config;
    SVECTOR paths[48][8];
    SVECTOR origin;
    SVECTOR endpoints[48];
    SVECTOR offset;
    SVECTOR number_position;
    SVECTOR number_velocity;
    u16 angles[48];
    u16 states[48];
    u16 ages[48];
    u16 ready;
    u16 number_mode;
    u16 frame;
    u16 tick;
    u16 active;
    u16 bounces;
    u16 cross_frame;
    s16 number;
    CVECTOR colors[48];
    CVECTOR glow_color;
    CVECTOR background_color;
    CVECTOR number_color;
} DuelEffect13Work;

extern VECTOR D_80146178;
extern GsIMAGE D_8015AC48[21];
extern DuelEffect13Config D_8015AE94[4];

void func_8014FABC(u16 radius, u16 spread, u16 height, u16 count, SVECTOR *vertices);
void func_801503F8(void *buffer, s32 phase, s16 number);

#endif
