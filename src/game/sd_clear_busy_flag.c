#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_state.h"

#include "sound_buffer_init.h"

void SD_ClearBusyFlag(void)
{
    g_SDValue->busy = 0;
}
