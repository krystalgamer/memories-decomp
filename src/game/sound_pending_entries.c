#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_pending_entries.h"

/* Same *g_SDValue struct as other siblings. Fills the u16 array at f43C
   from index f4 up to (but not including) f0 with 0xFFFF, always sets
   f442=0xFFFF regardless of whether the fill ran, then propagates f4 into
   f440 and recomputes f438 from f448's own f4 field plus 0x1010. The
   final field group is read via a fresh g_SDValue dereference rather
   than the cached local (see project_d8009b45c_reload_pointer_trick). */
void func_8004763C(void) {
    SDValue *a1 = g_SDValue;
    s32 i = a1->field_0004;

    a1->field_0442 = SD_VALUE_LINK_INDEX_NONE;
    if (i < a1->field_0000) {
        do {
            a1->field_043C[i] = SD_PENDING_ENTRY_NONE;
            i++;
        } while (i < a1->field_0000);
    }
    g_SDValue->field_0440 = g_SDValue->field_0004;
    g_SDValue->field_0438 =
        g_SDValue->field_0448->field_0004 + 0x1010;
}
