#ifndef MEMORIES_DECOMP_FUNC_8004E7B0_H
#define MEMORIES_DECOMP_FUNC_8004E7B0_H

#include "../types.h"

/* Advances the camera towards the pitch and yaw held in D_8009B47A/D_8009B47C
 * from the previous pair at D_8009B482/D_8009B484, wrapping the yaw delta the
 * short way round a full turn and writing the results into D_8009AF8E and
 * D_8009AF90. A non-zero argument forces both updates even when the delta is
 * zero; model_scene_setup.c passes 1.
 *
 * Only the function is declared here. That unit also carries tentative
 * definitions of D_8009AF88, D_8009AF8E, D_8009AF90 and the D_8009B478 group,
 * and its comment records that they are a codegen input -- making them extern
 * shortens the function by four bytes -- so they stay where they are. */
void func_8004E7B0(s32 force);

#endif
