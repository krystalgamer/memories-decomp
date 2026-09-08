#include "../types.h"
#include "model.h"

extern s16 D_800F2B22;

extern s32 func_8005F174(void);
extern s32 func_8005F18C(void);

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
