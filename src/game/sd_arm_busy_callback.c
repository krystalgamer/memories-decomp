#include "../types.h"
#include "file_transfer.h"
#include "sound.h"
#include "sound_output_state.h"

/* Retail stores through both globals with the one-line macro form (lui and
 * %lo at each use, never $gp). */
void SD_ArmBusyCallback(void) {
    g_SDValue->s_bSD_BUSY = 1;
    gFile_SecondaryPayloadCompleteCallback = SD_ClearBusyFlag;
}
