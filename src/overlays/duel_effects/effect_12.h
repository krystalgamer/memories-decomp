#ifndef MEMORIES_DECOMP_DUEL_EFFECT_12_H
#define MEMORIES_DECOMP_DUEL_EFFECT_12_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "drawing_tail.h"
#include "layered_drawing.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    CVECTOR color;
    u16 radii[3];
    u16 ring_height;
    u16 sprite_size;
    u16 particle_speed;
} DuelEffect12Config;

typedef struct {
    DuelEffect12Config *G32 config;
    SVECTOR rings[3][32];
    SVECTOR positions[32];
    SVECTOR velocities[32];
    SVECTOR origin;
    u32 scale;
    u32 frame;
    CVECTOR color;
} DuelEffect12Work;

#endif
