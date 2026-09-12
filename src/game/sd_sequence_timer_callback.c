#include "../types.h"
#include "../psyq/libapi.h"
#include "sound.h"
#include "sound_event_runtime.h"

long SD_SequenceTimerCallback(void)
{
    SDSecondaryState *state = D_8009B458;
    s32 i;

    if (state->field_0814 == 0)
        return 1;
    if (state->flag_0500 != 0)
        return 1;
    if (state->field_0509 != 0)
        return 1;
    if (state->flag_0501 == 0) {
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
        D_8009B458->flag_0501 = 0;
    }
    return 0;
}
