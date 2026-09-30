#ifndef MEMORIES_DECOMP_DUEL_EFFECT_16_H
#define MEMORIES_DECOMP_DUEL_EFFECT_16_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "color_helpers.h"
#include "layered_drawing.h"
#include "drawing_tail.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    CVECTOR color;
    u16 widths[4];
    u16 vertical_step;
    u16 spread;
    u16 glow_sizes[4];
    u16 particle_spread;
    u16 line_width;
    u16 ring_radii[3];
    u16 ring_height;
} DuelEffect16Config;

typedef struct {
    DuelEffect16Config *G32 config;
    SVECTOR paths[5][4][5];
    SVECTOR clouds[5][32];
    SVECTOR rings[3][32];
    SVECTOR positions[160];
    SVECTOR velocities[160];
    SVECTOR origins[5];
    u16 activated[5];
    u16 ages[5];
    u16 scales[5];
    u16 hits;
    u16 count;
    u16 tick;
    u16 cross_frame;
    CVECTOR primary[5];
    CVECTOR secondary[5];
} DuelEffect16Work;

extern VECTOR D_80146198;
extern DuelEffect16Config D_8015AEF4;

void func_80151558(void *buffer, s32 phase);

#endif
