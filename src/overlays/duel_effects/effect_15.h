#ifndef MEMORIES_DECOMP_DUEL_EFFECT_15_H
#define MEMORIES_DECOMP_DUEL_EFFECT_15_H

#include "../../types.h"
#include "../../game/duel_card.h"
#include "../../game/display_object.h"
#include "../../game/duel_effect_request.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../psyq/string.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "textured_quads.h"
#include "color_helpers.h"
#include "../../unmatched.h"

typedef struct {
    CVECTOR color;
    s32 selector;
} DuelEffect15Config;

typedef struct {
    DuelEffect15Config *G32 config;
    u16 count;
    u16 scale;
    u16 state;
    u16 visible;
    u16 timer;
    u16 cross_frame;
    CVECTOR color;
} DuelEffect15Work;

extern VECTOR D_80146168;
extern DuelEffect15Config D_8015AC08[8];
extern u32 D_8015B7A0[21];

void func_8014FF40(void *buffer, s32 phase);

#endif
