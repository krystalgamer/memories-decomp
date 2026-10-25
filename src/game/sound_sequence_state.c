#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "sound.h"
#include "sound_sequence_state.h"
#include "sd_reset_music_state.h"

/* Retires the driver's two pending requests, in the same *g_SDValue struct
   whose field_1582/field_1584 tent_SD_StartBgmFadeCopy and SD_StartBgmFade
   write.

   If the 0x157E slot is active, it stops that sequence through SD_StopSequence
   when the 0x80 flag bit is set (clearing the bit), releases the slot through
   SD_CloseSequence, and marks it inactive. Separately, if the VAB slot at 0x157A
   is ready for release, it resets that VAB transfer state and marks
   0x157A/0x1578 inactive. Always zeroes 0x1586/0x1588. */

void SD_ResetMusicState(void) {
    if (g_SDValue->field_157E != -1) {
        if (g_SDValue->flags_0040 & 0x80) {
            SD_StopSequence(g_SDValue->field_157E);
            g_SDValue->flags_0040 =
                g_SDValue->flags_0040 & 0xFF7F;
        }
        SD_CloseSequence(g_SDValue->field_157E);
        g_SDValue->field_157E = -1;
    }

    if (g_SDValue->field_157A == 0) {
        SD_ResetVabTransferState(0);
        g_SDValue->field_157A = -1;
        g_SDValue->field_1578 = -1;
    }

    g_SDValue->field_1588 = 0;
    g_SDValue->field_1586 = 0;
}

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
