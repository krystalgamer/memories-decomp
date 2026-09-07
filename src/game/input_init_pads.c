#include "../types.h"
#include "../psyq/libapi.h"
#include "input.h"

extern u8 gInput_abRawPadBuffers[];
extern void Input_ResetPads(void);
extern u8 gInput_bRepeatDelay, gInput_bRepeatInterval;

void Input_InitPads(void)
{
    InitPAD(
        gInput_abRawPadBuffers,
        0x22,
        gInput_abRawPadBuffers + 0x22,
        0x22
    );
    StartPAD();
    gInput_bRepeatDelay = INPUT_REPEAT_INITIAL_DELAY;
    gInput_bRepeatInterval = INPUT_REPEAT_INTERVAL;
    Input_ResetPads();
}
