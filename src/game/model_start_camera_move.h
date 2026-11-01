#ifndef MEMORIES_DECOMP_MODEL_START_CAMERA_MOVE_H
#define MEMORIES_DECOMP_MODEL_START_CAMERA_MOVE_H

#include "../types.h"
#include "model.h"

/* Camera move request. It fills the D_800F2B20 transition record from the live
 * camera at gModel_RView and a model slot's +0xDD0 vector, and writes the result
 * back immediately when the duration is zero.
 *
 * Both consumers already spelled it this way. model_effect_requests.c wraps it
 * in two guarded requests, described in its own header; Model_ResetSceneView,
 * in model_scene_controllers.c beside the definition, asks for the all-zero move.
 *
 * The transition record is the ModelCameraMove D_800F2B20, and its guarded
 * wrappers are Model_RequestCameraMove and Model_RequestCameraMoveToSlots, so
 * the request is named with that Model_ owner. */
void Model_StartCameraMove(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

#endif
