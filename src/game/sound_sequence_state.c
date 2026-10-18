#include "../types.h"
#include "sound.h"
#include "sound_sequence_state.h"

void tent_SD_StartBgmFadeCopy(s16 value, u8 flag)
{
    g_SDValue->field_1582 = value;
    g_SDValue->field_1584 = flag;
}

void SD_StartBgmFade(s16 step, u8 target)
{
    g_SDValue->field_1582 = step;
    g_SDValue->field_1584 = target;
}

s32 SD_IsBgmFadeActive(void)
{
    return g_SDValue->field_1582 != 0;
}
