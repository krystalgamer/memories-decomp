#ifndef MEMORIES_DECOMP_DUEL_EFFECT_9_H
#define MEMORIES_DECOMP_DUEL_EFFECT_9_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../game/sound.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "layered_drawing.h"
#include "drawing_helpers.h"
#include "drawing_tail.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    u8 color[3];
    u8 number_color[3];
    u16 particle_speed;
    u16 rotation_count;
    u16 widths[2];
    u16 height;
    u16 radii[3];
    u16 sprite_size;
    u16 bounce_speed;
    u16 unused_1A;
    s32 value;
    u32 unused_20;
} DuelEffect9Config;

typedef struct {
    DuelEffect9Config *G32 config;
    SVECTOR negative;
    SVECTOR positions[32];
    SVECTOR velocities[32];
    SVECTOR rings[3][32];
    SVECTOR rotations[64];
    SVECTOR positive;
    s16 positive_step;
    s16 negative_step;
    u32 scale;
    u32 frame;
    u32 tick;
    u16 negative_mode;
    u16 positive_mode;
    u16 stage;
    u16 bounce_count;
    u16 unused_72C;
    u16 cross_frame;
    CVECTOR negative_color;
    CVECTOR background_color;
    CVECTOR effects_color;
    CVECTOR positive_color;
} DuelEffect9Work;

extern VECTOR D_80146238;
extern DuelEffect9Config D_8015B650[5];

void func_801593A8(void *buffer, s32 phase);

#endif
