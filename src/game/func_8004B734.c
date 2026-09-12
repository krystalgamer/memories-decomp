/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_secondary_commands.c.
 */
#include "../types.h"
#define SDSECONDARYSTATE_BYTE_ALIAS
#include "../game/sound_spatialize_object.h"
#include "../psyq/libapi.h"

#include "../game/sound.h"
#include "../game/sound_event_runtime.h"
#include "../game/sound_spatialization.h"
#include "../game/func_8004ACE4.h"

#include "../game/sound_secondary_commands.h"
#include "../unmatched.h"

long SD_SequenceTimerCallback(void)
{
    SDSecondaryState *state = D_8009B458;
    int i;
    if (state->field_0814 == 0)
        return 1;
    if (state->flag_0500 != 0)
        return 1;
    if (state->field_0509 != 0)
        return 1;
    if (state->flag_0501 != 0)
        return 0;
    GetRCnt(RCntCNT2);
    D_8009B458->flag_0501 = 1;
    for (i = 0; i < 8; i++) {
        void (*callback)(void);
        SD_ProcessSequenceTracks();
        D_8009B458->field_0508++;
        state = D_8009B458;
        if (state->field_0508 >= 11) {
            state->field_0508 = 0;
            func_8004C84C();
            func_8004AAFC();
            callback = D_8009B458->field_050C;
            if (callback != 0)
                callback();
        }
    }
    {
        u8 *final;
        /* Keep the final state base register-backed under GCC 2.8.1. */
        do {
            final = D_8009B458_bytes;
        } while (i == 0 && i != 0);
        final[0x501] = 0;
    }
    return 0;
}
