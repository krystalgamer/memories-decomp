#ifndef MEMORIES_DECOMP_DUEL_EFFECT_17_H
#define MEMORIES_DECOMP_DUEL_EFFECT_17_H

#include "../../types.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../game/display_object.h"
#include "../../game/duel_card.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "drawing_tail.h"
#include "layered_drawing.h"
#include "color_helpers.h"
#include "drawing_helpers.h"
#include "../../unmatched.h"

typedef struct {
    u8 color[3];
    u8 burst_color[3];
    u16 radius;
    u16 spin;
    u16 height;
    u16 fan_spin;
    u16 mode;
} DuelEffect17Config;

typedef struct {
    DuelEffect17Config *G32 config;
    SVECTOR center;
    SVECTOR card_velocities[20];
    SVECTOR card_rotations[20];
    SVECTOR card_particles[20][16];
    SVECTOR particles[64];
    SVECTOR velocities[64];
    SVECTOR paths[48][8];
    SVECTOR endpoints[48];
    SVECTOR origin;
    u16 angles[48];
    u16 path_states[48];
    u16 field_1D94;
    u16 tick;
    u16 stage;
    u16 card_states[20];
    u16 brightness;
    u16 card_count;
    u16 active_cards;
    u16 active_particles;
    u16 active_paths;
    u16 card_ages[20];
    u16 completed;
    u16 cross;
    u16 path_ages[48];
    CVECTOR color;
    CVECTOR burst_color;
    CVECTOR particle_colors[64];
    CVECTOR path_colors[48];
} DuelEffect17Work;

extern VECTOR D_80146034;
extern DuelEffect17Config D_8015A60C[2];
extern u32 D_8015B7A0[21];

void func_80148BA4(void *buffer, s32 phase);

#endif
