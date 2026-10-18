#ifndef MEMORIES_DECOMP_FUNC_8004E7B0_H
#define MEMORIES_DECOMP_FUNC_8004E7B0_H

#include "../types.h"

/* Scrolls the model-scene background with the view. The yaw change since the
 * snapshot at D_8009B482, wrapped the short way round a full turn, moves the
 * horizontal scroll D_8009AF8E by that change * 1280 / 384 modulo the
 * background texture width; the renderer divides it by 10 for the sprite x.
 * When the pitch changed, D_8009AF90 is rederived from the absolute pitch:
 * below 0x801 it is pitch * 26 / 85, capped at 0x139 from a quarter turn up;
 * from 0x801 the pitch is read as wrapped and gives minus its distance from
 * a full turn times 26 / 85, floored at -0x139 beyond a quarter turn. The
 * renderer uses it as the sprite y offset. A non-zero argument forces both
 * updates; Model_ResetSceneView passes 1 after resetting the view and the
 * background renderer passes 0. Finally the eight-byte view snapshot at
 * D_8009B480 is refreshed from gModel_wViewDistance.
 *
 * Only the function is declared here. That unit also carries tentative
 * definitions of D_8009AF88, D_8009AF8E, D_8009AF90 and the
 * gModel_wViewDistance group, and its comment records that they are a
 * codegen input -- making them extern shortens the function by four bytes --
 * so they stay where they are.
 * Direct angle differences and a semantic absolute-value predicate remove
 * the former register bindings without changing those storage contracts. */
void Model_UpdateBackgroundScroll(s32 force);

#endif
