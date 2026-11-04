#define G_SDVALUE_IN_DATA
#define D_8009B128_IN_DATA
#include "../types.h"
#include "file_transfer.h"
#include "sound.h"
#include "sound_output_state.h"

/* Retail stores through both globals with the one-line macro form (lui and
 * %lo at each use, never $gp), like the other G_SDVALUE_IN_DATA units. */
void SD_ArmBusyCallback(void) {
    g_SDValue->busy = 1;
    gFile_SecondaryPayloadCompleteCallback = SD_ClearBusyFlag;
}
