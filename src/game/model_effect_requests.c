#include "../types.h"
#include "model_start_camera_move.h"
#include "model.h"
#include "model_transfer_flags.h"
#include "model_effect_requests.h"
#include "../unmatched.h"

void Model_RequestCameraMove(s32 value)
{
    s32 state = func_8005F174();

    if (state != 1 || Model_GetHandlerCallbackState() != state) {
        Model_StartCameraMove(value, 0, 0, 0);
    }
}

void Model_RequestCameraMoveToSlots(s32 first, s32 second, s32 third, s32 fourth)
{
    s32 adjusted_second = second < 0 ? 0 : second + 1;
    s32 adjusted_third = third < 0 ? 0 : third + 1;
    s32 state = func_8005F174();

    if (state != 1 || Model_GetHandlerCallbackState() != state) {
        Model_StartCameraMove(first, adjusted_second, adjusted_third, fourth);
    }
}

void Model_SetCameraEyeSlots(s32 first, s32 second)
{
    s32 state = func_8005F174();

    if (state == 1 && Model_GetHandlerCallbackState() == state) {
        return;
    }
    {
        ModelCameraMove *move = &D_800F2B20;

        move->eye.pair_slot = first;
        move->eye.slot = second;
    }
}

void Model_SetCameraTargetSlots(s32 first, s32 second)
{
    s32 state = func_8005F174();

    if (state == 1 && Model_GetHandlerCallbackState() == state) {
        return;
    }
    {
        ModelCameraMove *move = &D_800F2B20;

        move->target.pair_slot = first;
        move->target.slot = second;
    }
}

void Model_SetCameraMoveDurationFromMagnitude(s32 value)
{
    s32 state = func_8005F174();

    if (state == 1 && Model_GetHandlerCallbackState() == state) {
        return;
    }
    {
        ModelCameraMove *move = &D_800F2B20;

        move->duration = (value < 0 ? -value : value) * 2;
        move->elapsed = 0;
    }
}

void Model_SetCameraFollowCountdown(s32 value)
{
    s32 state = func_8005F174();

    if (state != 1 || Model_GetHandlerCallbackState() != state) {
        D_800F2B22 = value * 2;
    }
}

void Model_SetCameraOrbitStep(s32 value)
{
    s32 state = func_8005F174();
    ModelCameraMove *move;

    if (state == 1) {
        if (Model_GetHandlerCallbackState() == state) {
            return;
        }
    }
    move = &D_800F2B20;
#ifndef VERSION_JAPAN
    move->field_06 = value;
#endif
    move->field_04 = value;
}
