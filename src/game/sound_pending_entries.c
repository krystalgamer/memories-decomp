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

    a1->value_link_request_index = SD_VALUE_LINK_INDEX_NONE;
    if (i < a1->field_0000) {
        do {
            a1->note_index_by_id[i] = SD_PENDING_ENTRY_NONE;
            i++;
        } while (i < a1->field_0000);
    }
    g_SDValue->next_note_index = g_SDValue->field_0004;
    g_SDValue->next_link_spu_address =
        g_SDValue->value_links->spu_size + 0x1010;
}

/* Imports one pending-input block: each live key is bound to the next free
   note index at f440, its note record is copied into f444's table and the
   copy's +6 SPU address (in 16-byte units) is rebased by spu_addr / 16.
   SD_StepActiveCommand calls it for command 0x51 with the SPU destination
   and RAM staging block that SD_QueueValueLinkTransfer queued. */
void SD_RegisterSeqBlockNotes(SDSeqBlock *input, u32 spu_addr) {
    s32 i;

    for (i = 0; i < input->count; i++) {
        if (input->keys[i] != SD_PENDING_ENTRY_NONE) {
            g_SDValue->note_index_by_id[input->keys[i]] = g_SDValue->next_note_index;
            g_SDValue->note_table[g_SDValue->next_note_index] = input->data[i];
            g_SDValue->note_table[g_SDValue->next_note_index].spu_address_div16 +=
                spu_addr >> 4;
            g_SDValue->next_note_index++;
        }
    }
}
