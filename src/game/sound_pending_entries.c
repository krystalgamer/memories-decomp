#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

typedef struct {
    u8 pad0000[6];
    u16 field_0006;
} SoundPendingEntry;

typedef char SoundPendingEntry_size_must_match_note_record[
    sizeof(SoundPendingEntry) == SD_NOTE_RECORD_SIZE ? 1 : -1
];
typedef char SoundPendingInput_id_size_must_be_halfword[
    SD_PENDING_INPUT_ID_ENTRY_SIZE == sizeof(u16) ? 1 : -1
];

typedef struct {
    u8 pad0000[0x43C];
    u16 *field_043C;
    u16 field_0440;
    u8 pad0442[2];
    SoundPendingEntry *field_0444;
} SoundPendingState;

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

void func_800476B4(void *arg0, u32 arg1)
{
    register s32 i asm("$7");
    u8 *base = arg0;

    i = 0;
    if (*(s32 *)base > 0) {
        u8 *payload;
        register u8 *id_cursor asm("$6");
        u32 rate;
        register s32 sentinel asm("$11");

        sentinel = SD_PENDING_ENTRY_NONE;
        rate = arg1 >> 4;
        payload = base;
        id_cursor = base;

        do {
            u16 id = *(u16 *)(id_cursor + SD_PENDING_INPUT_IDS_BYTE_OFFSET);

            if (id != sentinel) {
                register SoundPendingState *state asm("$5") =
                    (SoundPendingState *)g_SDValue;

                state->field_043C[id] = state->field_0440;

                {
                    u16 slot = state->field_0440;
                    SoundPendingEntry *entries = state->field_0444;
                    register SoundPendingEntry *dst asm("$2") =
                        (SoundPendingEntry *)(slot * SD_NOTE_RECORD_SIZE +
                                              (s32)entries);
                    register SoundPendingEntry *updated asm("$3");
                    register SoundPendingState *state2 asm("$4") = state;

                    __builtin_memcpy(dst,
                                     payload + SD_PENDING_INPUT_PAYLOAD_BYTE_OFFSET,
                                     SD_NOTE_RECORD_SIZE);
                    updated = (SoundPendingEntry *)
                        (state2->field_0440 * SD_NOTE_RECORD_SIZE +
                         (s32)state2->field_0444);
                    updated->field_0006 += rate;
                    state2->field_0440++;
                }
            }

            payload += SD_NOTE_RECORD_SIZE;
            id_cursor += SD_PENDING_INPUT_ID_ENTRY_SIZE;
            i++;
        } while (i < *(s32 *)base);
    }
}
