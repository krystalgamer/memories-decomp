#include "../types.h"
#include "func_80049010.h"
#include "sound.h"

/* Same *g_SDValue struct as set_8009b45c_1582_1584.c/clear_8009b45c_busy.c.
   If the 0x157E slot is active, notifies func_80049C40() (and clears the
   0x80 flag bit) if that flag was set, resets the slot via
   reset_slot_7E0_if_active(), and marks it inactive. Separately, if the
   0x157A counter has run out, clears it via clear_8009b458_4A4_if_set()
   and resets 0x157A/0x1578. Always zeroes 0x1586/0x1588. */

/* Takes an argument here on purpose, same as func_80049C40 above/below:
   defined void (void), but the retail call site sets up $a0 first and
   this declaration is what keeps that setup. Measured; see
   notes/research/matching-evidence.md. */
extern void func_800498F8(s32 a0);
/* Takes an argument here on purpose. func_80049C40 is defined
   void (void) in sound_secondary_playback.c and ignores it, but the
   retail call site computes g_SDValue->field_157E into $a0 first, and
   this declaration is what keeps that computation alive. Making it
   agree with the definition costs four instructions; see
   notes/research/matching-evidence.md. */
extern void func_80049C40(s16 a0);
/* Takes an argument here on purpose, same as func_80049C40 above/below:
   defined void (void), but the retail call site sets up $a0 first and
   this declaration is what keeps that setup. Measured; see
   notes/research/matching-evidence.md. */
extern void func_80049CB0(s16 a0);

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
