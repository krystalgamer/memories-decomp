#ifndef MEMORIES_DECOMP_DUEL_DRAW_STATUS_NUMBERS_H
#define MEMORIES_DECOMP_DUEL_DRAW_STATUS_NUMBERS_H

#include "../ygo_types.h"
#include "display_object.h"
#include "duel_side_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

void func_80016D2C(
    DisplayObject *style, GsSPRITE *digit, s32 value, s32 count
);

void Duel_UpdateLifePointDisplay(DuelSideState *side);

void Duel_DrawLifePointsAndDeckCounts(DisplayObject *widget);

#endif
