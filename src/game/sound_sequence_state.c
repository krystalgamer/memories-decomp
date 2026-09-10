#include "../types.h"
#include "sound.h"
#include "sound_sequence_state.h"

void func_800490F0(s16 value, u8 flag)
{
    g_SDValue->field_1582 = value;
    g_SDValue->field_1584 = flag;
}

void func_80049108(s16 value, u8 flag)
{
    g_SDValue->field_1582 = value;
    g_SDValue->field_1584 = flag;
}

s32 func_80049120(void)
{
    return g_SDValue->field_1582 != 0;
}
