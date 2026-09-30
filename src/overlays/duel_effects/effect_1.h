#ifndef MEMORIES_DECOMP_DUEL_EFFECT_1_H
#define MEMORIES_DECOMP_DUEL_EFFECT_1_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "layered_drawing.h"
#include "drawing_tail.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    CVECTOR color;
    u16 initial_rotation_step;
    u16 rotation_acceleration;
    u16 beam_rotation_step;
    u16 particle_speed;
    u16 sprite_size;
    u16 widths[2];
    u16 height;
    u16 first_end;
    u16 second_end;
} DuelEffect1Config;

typedef struct {
    DuelEffect1Config *G32 config;
    SVECTOR rotations[12];
    SVECTOR positions[64];
    SVECTOR velocities[64];
    u16 rotation_step;
    u16 stage;
    u32 growth;
    u32 frame;
    u32 tick;
    u16 cross_frame;
    CVECTOR base_color;
    CVECTOR beam_color;
    CVECTOR screen_color;
} DuelEffect1Work;

extern VECTOR D_80146208;
extern DuelEffect1Config D_8015B420[2];

void func_80157794(void *buffer, s32 phase);

#endif
