#ifndef MEMORIES_DECOMP_DUEL_EFFECT_24_H
#define MEMORIES_DECOMP_DUEL_EFFECT_24_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "layered_drawing.h"
#include "color_helpers.h"
#include "display_quads.h"
#include "../../game/display_object.h"
#include "../../game/display_object_work_slots.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    SVECTOR targets[5];
    SVECTOR points[5];
    SVECTOR point_steps[5];
    SVECTOR center;
    SVECTOR heads[64];
    SVECTOR tails[64];
    SVECTOR velocities[64];
    SVECTOR rotations[32];
    u16 half_width;
    u16 half_height;
    u16 widths[2];
    u8 field_788[32];
    u16 beam_width;
    u16 arrival;
    u16 connection_tick;
    u16 line_count;
    u16 ray_count;
    u16 stage;
    u32 scale;
    u32 ray_scales[32];
    u32 tick;
    CVECTOR center_color;
    CVECTOR flash_color;
    CVECTOR line_color;
} DuelEffect24Work;

#endif
