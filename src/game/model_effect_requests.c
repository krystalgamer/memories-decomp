#include "../types.h"
#include "model_start_camera_move.h"
#include "model.h"
#include "model_transfer_flags.h"
#include "model_effect_requests.h"
#include "../unmatched.h"
#include "model_camera_state.h"

void Model_RequestCameraMove(s32 value)
{
    s32 state = Model_GetCameraHoldState();

    if (state != 1 || Model_GetHandlerCallbackState() != state) {
        Model_StartCameraMove(value, 0, 0, 0);
    }
}

void Model_RequestCameraMoveToSlots(s32 first, s32 second, s32 third, s32 fourth)
{
    s32 adjusted_second = second < 0 ? 0 : second + 1;
    s32 adjusted_third = third < 0 ? 0 : third + 1;
    s32 state = Model_GetCameraHoldState();

    if (state != 1 || Model_GetHandlerCallbackState() != state) {
        Model_StartCameraMove(first, adjusted_second, adjusted_third, fourth);
    }
}

void Model_SetCameraEyeSlots(s32 first, s32 second)
{
    s32 state = Model_GetCameraHoldState();

    if (state == 1 && Model_GetHandlerCallbackState() == state) {
        return;
    }
    {
        ModelCameraMove *move = &gModel_CameraMove;

        move->eye.pair_slot = first;
        move->eye.slot = second;
    }
}

void Model_SetCameraTargetSlots(s32 first, s32 second)
{
    s32 state = Model_GetCameraHoldState();

    if (state == 1 && Model_GetHandlerCallbackState() == state) {
        return;
    }
    {
        ModelCameraMove *move = &gModel_CameraMove;

        move->target.pair_slot = first;
        move->target.slot = second;
    }
}

void Model_SetCameraMoveDurationFromMagnitude(s32 value)
{
    s32 state = Model_GetCameraHoldState();

    if (state == 1 && Model_GetHandlerCallbackState() == state) {
        return;
    }
    {
        ModelCameraMove *move = &gModel_CameraMove;

        move->duration = (value < 0 ? -value : value) * 2;
        move->elapsed = 0;
    }
}

void Model_SetCameraFollowCountdown(s32 value)
{
    s32 state = Model_GetCameraHoldState();

    if (state != 1 || Model_GetHandlerCallbackState() != state) {
        gModel_wCameraFollowCountdown = value * 2;
    }
}

void Model_SetCameraOrbitStep(s32 value)
{
    s32 state = Model_GetCameraHoldState();
    ModelCameraMove *move;

    if (state == 1) {
        if (Model_GetHandlerCallbackState() == state) {
            return;
        }
    }
    move = &gModel_CameraMove;
#ifndef VERSION_JAPAN
    move->base_orbit_step = value;
#endif
    move->orbit_step = value;
}
