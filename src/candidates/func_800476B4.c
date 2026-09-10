/* Reclassified from matching_c (#3859). This was part of
 * src/game/sound_pending_entries.c. It is byte-exact under
 * gcc_2_8_1_g0_no_split only with 7 variables pinned to hard registers; the
 * mixed-G gcc_2_8_1_cc_g0_as_g8_no_split it was first matched at turned out
 * to be inert. Without the pins it is 52 instructions against the target's
 * 53, opcode distance 1. The source below is the match, unchanged apart
 * from its include paths. */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_pending_entries.h"

void func_800476B4(SDSeqBlock *arg0, u32 arg1)
{
    register s32 i asm("$7");
    u8 *base = (u8 *)arg0;

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
                register SDValue *state asm("$5") = g_SDValue;

                state->field_043C[id] = state->field_0440;

                {
                    u16 slot = state->field_0440;
                    SDNote *entries = state->field_0444;
                    register SDNote *dst asm("$2") =
                        (SDNote *)(slot * SD_NOTE_RECORD_SIZE + (s32)entries);
                    register SDNote *updated asm("$3");
                    register SDValue *state2 asm("$4") = state;

                    __builtin_memcpy(dst,
                                     payload + SD_PENDING_INPUT_PAYLOAD_BYTE_OFFSET,
                                     SD_NOTE_RECORD_SIZE);
                    updated = (SDNote *)
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
