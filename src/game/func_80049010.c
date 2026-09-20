#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "func_80049010.h"
#include "sound.h"

/* Retires the driver's two pending requests, in the same *g_SDValue struct
   whose field_1582/field_1584 func_800490F0 and func_80049108 write.

   If the 0x157E slot is active, it stops that sequence through SD_StopSequence
   when the 0x80 flag bit is set (clearing the bit), releases the slot through
   func_80049CB0, and marks it inactive. Separately, if the VAB slot at 0x157A
   is ready for release, it resets that VAB transfer state and marks
   0x157A/0x1578 inactive. Always zeroes 0x1586/0x1588. */

void SD_ResetMusicState(void) {
    if (g_SDValue->field_157E != -1) {
        if (g_SDValue->flags_0040 & 0x80) {
            SD_StopSequence(g_SDValue->field_157E);
            g_SDValue->flags_0040 =
                g_SDValue->flags_0040 & 0xFF7F;
        }
        func_80049CB0(g_SDValue->field_157E);
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
