#ifndef MEMORIES_DECOMP_SPANISH_MODEL_PRIMARY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_PRIMARY_H

#include "../../types.h"
#include "../../game/model_control.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/func_8005D994.h"

typedef struct {
    u16 frames;
    u16 delay;
    u16 destination_x;
    u16 destination_y;
    u16 source_x;
    u16 source_y;
    u16 width;
    u16 height;
} ModelPrimaryCopyConfig;

typedef struct {
    ModelPrimaryCopyConfig *G32 config;
    u32 tick;
    RECT rectangle;
} ModelPrimaryCopyState;

typedef struct {
    s16 arg1;
    s16 arg2;
    s16 arg3;
    s16 arg5;
    SVECTOR offset;
} ModelPrimaryEffectConfig;

typedef struct {
    ModelPrimaryEffectConfig *G32 config;
    s32 animation;
} ModelPrimaryEffectState;

extern ModelPrimaryCopyConfig D_8013A118[];
extern ModelPrimaryEffectConfig D_8013A0DC[];

#endif
