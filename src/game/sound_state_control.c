#include "../types.h"
#include "sound.h"
#include "sound_state_control.h"

void SD_SetSequenceVSyncMode(s32 value)
{
    SDSecondaryState *state;

    gSD_pSecondaryState->vsync_update_mode = value;
    state = gSD_pSecondaryState;
    if (state->vsync_update_mode != 0)
        state->timer_update_inhibit = 1;
    else
        state->timer_update_inhibit = 0;
}

void SD_InitDataSourceFlags(s32 first, s32 second, s32 third)
{
    g_SDValue->bank_load_phase = 0;
    if (first == 0)
        g_SDValue->data_source_flags &= 0xFE;
    if (second == 0)
        g_SDValue->data_source_flags &= 0xFD;
    if (third == 0)
        g_SDValue->data_source_flags &= 0xBF;
    g_SDValue->status_flags |= 0xA;
}
