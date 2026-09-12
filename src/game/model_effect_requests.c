#include "../types.h"
#include "func_80052D2C.h"
#include "model.h"
#include "model_transfer_flags.h"
#include "model_effect_requests.h"
#include "../unmatched.h"

void func_80059EBC(s32 value)
{
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        func_80052D2C(value, 0, 0, 0);
    }
}

void func_80059F18(s32 first, s32 second, s32 third, s32 fourth)
{
    s32 adjusted_second = second < 0 ? 0 : second + 1;
    s32 adjusted_third = third < 0 ? 0 : third + 1;
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        func_80052D2C(first, adjusted_second, adjusted_third, fourth);
    }
}

void func_80059FAC(s32 first, s32 second)
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

void func_8005A074(s32 value)
{
    s32 state = func_8005F174();

    if (state == 1 && func_8005F18C() == state) {
        return;
    }
    {
        ModelCameraMove *move = &D_800F2B20;

        move->duration = (value < 0 ? -value : value) * 2;
        move->elapsed = 0;
    }
}

void func_8005A0DC(s32 value)
{
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        D_800F2B22 = value * 2;
    }
}

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
    move->field_06 = value;
    move->field_04 = value;
}
