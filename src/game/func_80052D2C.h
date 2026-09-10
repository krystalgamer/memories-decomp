#ifndef MEMORIES_DECOMP_FUNC_80052D2C_H
#define MEMORIES_DECOMP_FUNC_80052D2C_H

#include "../types.h"

/* Camera move request. It fills the D_800F2B20 transition record from the live
 * camera at D_800F56F0 and a model slot's +0xDD0 vector, and writes the result
 * back immediately when the duration is zero.
 *
 * Both consumers already spelled it this way. model_effect_requests.c wraps it
 * in two guarded requests, described in its own header; model_scene_setup.c
 * asks for the all-zero move.
 *
 * The address-based name is kept: functions.csv describes what the request
 * does but assigns no name, and which subsystem owns the transition record is
 * not settled by that description alone. */
void func_80052D2C(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

#endif
