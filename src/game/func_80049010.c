#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "func_80049010.h"
#include "sound.h"

/* Retires the driver's two pending requests, in the same *g_SDValue struct
   whose field_1582/field_1584 func_800490F0 and func_80049108 write.

   If the 0x157E slot is active, it notifies func_80049C40 with that slot when
   the 0x80 flag bit is set (clearing the bit), releases the slot through
   func_80049CB0, and marks it inactive. Separately, if the 0x157A counter has
   run out, it stops the sequence through func_800498F8 and resets
   0x157A/0x1578. Always zeroes 0x1586/0x1588. */

void func_80049010(void) {
    if (g_SDValue->field_157E != -1) {
        if (g_SDValue->flags_0040 & 0x80) {
            func_80049C40(g_SDValue->field_157E);
            g_SDValue->flags_0040 =
                g_SDValue->flags_0040 & 0xFF7F;
        }
        func_80049CB0(g_SDValue->field_157E);
        g_SDValue->field_157E = -1;
    }

    if (g_SDValue->field_157A == 0) {
        func_800498F8(0);
        g_SDValue->field_157A = -1;
        g_SDValue->field_1578 = -1;
    }

    g_SDValue->field_1588 = 0;
    g_SDValue->field_1586 = 0;
}
