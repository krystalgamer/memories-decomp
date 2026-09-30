#ifndef MEMORIES_DECOMP_DUEL_EFFECT_22_H
#define MEMORIES_DECOMP_DUEL_EFFECT_22_H

#include "../../types.h"
#include "effect_6.h"
#include "../../game/display_object.h"
#include "../../game/duel_check_ritual.h"
#include "../../game/screen_projection.h"
#include "../../game/sound.h"

typedef struct {
    u8 color[3];
    u8 fan_color[3];
    u16 point_size;
    u16 spread;
    u16 inner_widths[2];
    u16 inner_height;
    u16 outer_widths[2];
    u16 outer_height;
    u16 duration;
    u16 field_18;
} DuelEffect22Config;

typedef struct {
    DuelEffect22Config *G32 config;
    SVECTOR positions[2];
    SVECTOR card_velocities[3];
    SVECTOR rings[2][4];
    SVECTOR points[32];
    SVECTOR rotations[32];
    SVECTOR line_inner[64];
    SVECTOR line_outer[64];
    SVECTOR line_velocities[64];
    SVECTOR particles[64];
    SVECTOR particle_velocities[64];
    SVECTOR card_rotations[3];
    u16 texture_frames[2];
    u16 fan_speed;
    u16 active_cards;
    u16 active_particles;
    u16 ring_scale;
    u16 frame;
    u16 hold;
    u16 stage;
    u16 card_states[3];
    u16 sound_played;
    u16 brightness;
    u16 cross_frame;
    CVECTOR portal_color;
    CVECTOR flame_color;
    CVECTOR flash_color;
    CVECTOR particle_color;
    CVECTOR fan_color;
    CVECTOR colors[64];
} DuelEffect22Work;

extern VECTOR D_80146044;
extern DuelEffect22Config D_8015A658[24];
extern u32 D_8015B7A0[21];

void func_8014A8E4(void *buffer, s32 phase);

#endif
