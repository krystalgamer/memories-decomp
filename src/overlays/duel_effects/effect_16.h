#ifndef MEMORIES_DECOMP_DUEL_EFFECT_16_H
#define MEMORIES_DECOMP_DUEL_EFFECT_16_H

#include "../../types.h"
#include "../../game/duel_effect_object_pool.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_slot_support.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "color_helpers.h"
#include "layered_drawing.h"
#include "drawing_tail.h"

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
    DuelEffect16Config *config;
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

#endif
