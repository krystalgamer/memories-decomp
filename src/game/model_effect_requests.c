#include "../types.h"
#include "model_start_camera_move.h"
#include "model.h"
#include "model_transfer_flags.h"
#include "model_effect_requests.h"
#include "../unmatched.h"
#include "model_camera_state.h"

void Model_RequestCameraMove(s32 mode_request)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state != 1 || Model_GetHandlerCallbackState() != hold_state) {
        Model_StartCameraMove(mode_request, 0, 0, 0);
    }
}

void Model_RequestCameraMoveToSlots(s32 mode_request, s32 eye_slot, s32 target_slot, s32 timing_request)
{
    s32 eye_selector = eye_slot < 0 ? 0 : eye_slot + 1;
    s32 target_selector = target_slot < 0 ? 0 : target_slot + 1;
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state != 1 || Model_GetHandlerCallbackState() != hold_state) {
        Model_StartCameraMove(mode_request, eye_selector, target_selector, timing_request);
    }
}

void Model_SetCameraEyeSlots(s32 pair_slot, s32 slot_index)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state == 1 && Model_GetHandlerCallbackState() == hold_state) {
        return;
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->eye.pair_slot = pair_slot;
        camera_move->eye.slot = slot_index;
    }
}

void Model_SetCameraTargetSlots(s32 pair_slot, s32 slot_index)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state == 1 && Model_GetHandlerCallbackState() == hold_state) {
        return;
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->target.pair_slot = pair_slot;
        camera_move->target.slot = slot_index;
    }
}

void Model_SetCameraMoveDurationFromMagnitude(s32 magnitude_request)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state == 1 && Model_GetHandlerCallbackState() == hold_state) {
        return;
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->duration = (magnitude_request < 0 ? -magnitude_request : magnitude_request) * 2;
        camera_move->elapsed = 0;
    }
}

void Model_SetCameraFollowCountdown(s32 countdown_request)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state != 1 || Model_GetHandlerCallbackState() != hold_state) {
        gModel_wCameraFollowCountdown = countdown_request * 2;
    }
}

void Model_SetCameraOrbitStep(s32 orbit_step_request)
{
    s32 hold_state = Model_GetCameraHoldState();
    ModelCameraMove *camera_move;

    if (hold_state == 1) {
        if (Model_GetHandlerCallbackState() == hold_state) {
            return;
        }
    }
    camera_move = &gModel_CameraMove;
#ifndef VERSION_JAPAN
    camera_move->base_orbit_step = orbit_step_request;
#endif
    camera_move->orbit_step = orbit_step_request;
}
