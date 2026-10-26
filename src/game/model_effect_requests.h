#ifndef MEMORIES_DECOMP_MODEL_EFFECT_REQUESTS_H
#define MEMORIES_DECOMP_MODEL_EFFECT_REQUESTS_H

#include "../types.h"

/* Two guarded requests into Model_StartCameraMove.
 *
 * Both consult Model_GetCameraHoldState and Model_GetHandlerCallbackState first and do nothing when the
 * state is 1 and the two agree, so a caller cannot assume the request was
 * issued. Neither reports whether it was.
 *
 * Model_RequestCameraMoveToSlots biases its middle two arguments before passing them on: a
 * negative becomes 0 and anything else is incremented, so the values reaching
 * Model_StartCameraMove are not the ones the caller wrote. */
void Model_RequestCameraMove(s32 value);
void Model_RequestCameraMoveToSlots(s32 first, s32 second, s32 third, s32 fourth);

/* Stores the camera move's two target-slot selectors, under the
 * same Model_GetCameraHoldState/Model_GetHandlerCallbackState guard: nothing is written when the state
 * is 1 and the two agree. */
void Model_SetCameraTargetSlots(s32 first, s32 second);

/* Stores twice `value` into the camera move's D_800F2B22, under that same
 * guard -- written with the test inverted, so the store sits inside
 * `state != 1 || Model_GetHandlerCallbackState() != state` rather than after an early return
 * (model_effect_requests.c:73).
 *
 * This unit defines seven functions and this header declares the ones with
 * consumers outside it. Model_SetCameraFollowCountdown was the fourth such case and the only
 * one whose caller wrote the prototype by hand: Model_UpdateAnimatedBattle,
 * the model scene mode 15 controller, calls it once with 0x78
 * (src/game/model_scene_controllers.c:413) while already including this header.
 * Its two immediate neighbours in the file, Model_SetCameraMoveDurationFromMagnitude and Model_SetCameraOrbitStep,
 * have no uses outside the unit at all, which is why they are still not
 * declared here. The latter writes both base_orbit_step and orbit_step in the US
 * camera record, but only orbit_step in the compact Japanese record. */
void Model_SetCameraFollowCountdown(s32 value);

#endif
