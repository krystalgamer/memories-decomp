#ifndef MEMORIES_DECOMP_DUEL_EFFECT_19_H
#define MEMORIES_DECOMP_DUEL_EFFECT_19_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "drawing_tail.h"
#include "layered_drawing.h"
#include "drawing_helpers.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    SVECTOR rotations[64];
    SVECTOR rings[3][32];
    SVECTOR positions[32];
    SVECTOR velocities[32];
    u16 widths[3];
    u16 padding706;
    u32 scale;
    u32 frame;
    u16 fade_started;
    u16 spawned;
    CVECTOR color;
} DuelEffect19Work;

extern VECTOR D_801461C8;

void func_80153ADC(void *buffer, s32 phase);

#endif
