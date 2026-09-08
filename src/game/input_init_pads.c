#include "../types.h"
#include "../psyq/libapi.h"
#include "input.h"

void Input_InitPads(void)
{
    InitPAD(
        gInput_abRawPadBuffers,
        INPUT_RAW_PAD_BUFFER_SIZE,
        gInput_abRawPadBuffers + INPUT_RAW_PAD_BUFFER_SIZE,
        INPUT_RAW_PAD_BUFFER_SIZE
    );
    StartPAD();
    gInput_bRepeatDelay = INPUT_REPEAT_THRESHOLD;
    gInput_bRepeatInterval = INPUT_REPEAT_RELOAD_VALUE;
    Input_ResetPads();
}
