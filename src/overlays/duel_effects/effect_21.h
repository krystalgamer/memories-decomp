#ifndef MEMORIES_DECOMP_DUEL_EFFECT_21_H
#define MEMORIES_DECOMP_DUEL_EFFECT_21_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/high_memory_addresses.h"
#include "../../game/screen_projection.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"
#include "display_quads.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    SVECTOR positions[10];
    u16 tiles[10];
    u16 count;
    CVECTOR color;
    u16 enabled;
    u16 offsets[10];
} DuelEffect21Work;

extern VECTOR D_80146158;
extern SVECTOR D_8015AB68[10][2];

void func_8014E3EC(void *buffer, s32 phase);

#endif
