#ifndef MEMORIES_DECOMP_DUEL_EFFECT_0_H
#define MEMORIES_DECOMP_DUEL_EFFECT_0_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "layered_drawing.h"
#include "drawing_helpers.h"
#include "color_helpers.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"

typedef struct {
    CVECTOR color;
    u16 sprite_size;
    u16 radii[3];
    u16 widths[2];
    u16 height;
    u16 delay;
} DuelEffect0Config;

typedef struct {
    DuelEffect0Config *G32 config;
    SVECTOR rings[3][32];
    SVECTOR rotations[16];
    SVECTOR quads[2][4];
    u32 scale;
    u32 frame;
    u16 cross_frame;
    CVECTOR color;
} DuelEffect0Work;

extern VECTOR D_801461E8;
extern DuelEffect0Config D_8015B0B4[30];

void func_80154688(void *buffer, s32 phase);

#endif
