#ifndef MEMORIES_DECOMP_DUEL_EFFECT_23_H
#define MEMORIES_DECOMP_DUEL_EFFECT_23_H

#include "../../types.h"
#include "effect_6.h"
#include "../../game/duel_card.h"
#include "../../game/display_object.h"
#include "../../game/screen_projection.h"
#include "../../game/sound.h"

typedef struct {
    SVECTOR position;
    SVECTOR rotation;
    SVECTOR slots[5];
    SVECTOR card_velocities[5];
    SVECTOR card_rotations[5];
    SVECTOR particles[5][3];
    SVECTOR velocities[5][3];
    s16 speed;
    u16 field_17A;
    u16 stage;
    u16 swing;
    u16 states[5];
    u16 ages[5];
    u16 active;
    u16 hidden;
    u16 count;
    CVECTOR color;
} DuelEffect23Work;

extern VECTOR D_801461A8;
extern u32 D_8015B7A0[21];

void func_80152048(void *buffer, s32 phase);

#endif
