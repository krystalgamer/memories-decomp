#include "../types.h"
#include "sound.h"
#include "sound_state_control.h"

void func_8004695C(s32 value)
{
    SDSecondaryState *state;

    D_8009B458->field_0509 = value;
    state = D_8009B458;
    if (state->field_0509 != 0)
        state->flag_0500 = 1;
    else
        state->flag_0500 = 0;
}

void func_80046990(s32 first, s32 second, s32 third)
{
    g_SDValue->field_003C = 0;
    if (first == 0)
        g_SDValue->flags_004A &= 0xFE;
    if (second == 0)
        g_SDValue->flags_004A &= 0xFD;
    if (third == 0)
        g_SDValue->flags_004A &= 0xBF;
    g_SDValue->flags_0040 |= 0xA;
}
