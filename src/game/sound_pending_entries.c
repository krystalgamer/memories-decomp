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
void SD_ResetPendingEntries(void) {
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

/* Imports one pending-input block: each live key is bound to the next free
   note index at f440, its note record is copied into f444's table and the
   copy's +6 field is advanced by rate / 16. */
void func_800476B4(SDSeqBlock *input, u32 rate) {
    s32 i;

    for (i = 0; i < input->count; i++) {
        if (input->keys[i] != SD_PENDING_ENTRY_NONE) {
            g_SDValue->field_043C[input->keys[i]] = g_SDValue->field_0440;
            g_SDValue->field_0444[g_SDValue->field_0440] = input->data[i];
            g_SDValue->field_0444[g_SDValue->field_0440].field_0006 +=
                rate >> 4;
            g_SDValue->field_0440++;
        }
    }
}
