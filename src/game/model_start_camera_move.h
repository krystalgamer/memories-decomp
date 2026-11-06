#ifndef MEMORIES_DECOMP_MODEL_START_CAMERA_MOVE_H
#define MEMORIES_DECOMP_MODEL_START_CAMERA_MOVE_H

#include "../types.h"
#include "model.h"

/* Camera move request. It fills the gModel_CameraMove transition record from the live
 * camera at gModel_RView and a model slot's +0xDD0 vector, and writes the result
 * back immediately when the duration is zero.
 *
 * Both consumers already spelled it this way. model_scene_services.c wraps it
 * in two guarded requests, described in its own header; Model_ResetSceneView,
 * in model_scene_controllers.c beside the definition, asks for the all-zero move.
 *
 * The transition record is the ModelCameraMove gModel_CameraMove, and its guarded
 * wrappers are Model_RequestCameraMove and Model_RequestCameraMoveToSlots, so
 * the request is named with that Model_ owner. */
void Model_StartCameraMove(s32 mode_request, s32 eye_transport, s32 target_transport, s32 timing_work);

#endif
