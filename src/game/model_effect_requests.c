#include "../types.h"
#include "func_80052D2C.h"
#include "model.h"
#include "model_transfer_flags.h"
#include "model_effect_requests.h"
#include "../unmatched.h"

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_EFFECT_REQUEST)
void func_80059EBC(s32 value)
{
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        func_80052D2C(value, 0, 0, 0);
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_EFFECT_REQUEST_ARGS)
void func_80059F18(s32 first, s32 second, s32 third, s32 fourth)
{
    s32 adjusted_second = second < 0 ? 0 : second + 1;
    s32 adjusted_third = third < 0 ? 0 : third + 1;
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        func_80052D2C(first, adjusted_second, adjusted_third, fourth);
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_CAMERA_EYE_SLOTS)
void Model_SetCameraEyeSlots(s32 first, s32 second)
{
    s32 state = func_8005F174();

    if (state == 1 && func_8005F18C() == state) {
        return;
    }
    {
        ModelCameraMove *move = &D_800F2B20;

        move->eye.pair_slot = first;
        move->eye.slot = second;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_CAMERA_TARGET_SLOTS)
void func_8005A010(s32 first, s32 second)
{
    s32 state = func_8005F174();

    if (state == 1 && func_8005F18C() == state) {
        return;
    }
    {
        ModelCameraMove *move = &D_800F2B20;

        move->target.pair_slot = first;
        move->target.slot = second;
    }
}
#endif

#ifndef MODEL_CAMERA_DURATION_FIELD
#define MODEL_CAMERA_DURATION_FIELD duration
#endif
#ifndef MODEL_CAMERA_ELAPSED_FIELD
#define MODEL_CAMERA_ELAPSED_FIELD elapsed
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_CAMERA_DURATION)
void func_8005A074(s32 value)
{
    s32 state = func_8005F174();

    if (state == 1 && func_8005F18C() == state) {
        return;
    }
    {
        ModelCameraMove *move = &D_800F2B20;

        move->MODEL_CAMERA_DURATION_FIELD = (value < 0 ? -value : value) * 2;
        move->MODEL_CAMERA_ELAPSED_FIELD = 0;
    }
}

#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_CAMERA_SPEED)
void func_8005A0DC(s32 value)
{
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        D_800F2B22 = value * 2;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_CAMERA_FIELD_04)
void func_8005A130(s32 value)
{
    s32 state = func_8005F174();
    ModelCameraMove *move;

    if (state == 1) {
        if (func_8005F18C() == state) {
            return;
        }
    }
    move = &D_800F2B20;
#ifndef VERSION_JAPAN
    move->field_06 = value;
#endif
    move->field_04 = value;
}
#endif
