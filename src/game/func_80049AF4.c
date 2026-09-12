#include "../types.h"
#include "sound.h"
#include "sound_sequence_timing.h"

void func_80049AF4(s32 arg0)
{
    u8 one = 1;

    D_8009B458->flag_0500 = one;
    if (D_8009B458->field_07E0 == -1) {
        D_8009B458->flag_0500 = 0;
        return;
    }
    D_8009B458->field_07EC = 0x10000;
    D_8009B458->field_07DC = D_8009B458->field_07E8;
    SD_StartSequenceTracks();

    arg0 = (u16)arg0;
    if ((arg0 & 0xFF) == 0) {
        D_8009B458->field_07E2 = 4;
    } else {
        D_8009B458->flag_0502 = one;
        D_8009B458->field_07E2 = 1;
    }
    D_8009B458->flag_0500 = 0;
}
