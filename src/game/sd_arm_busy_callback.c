#include "../types.h"
#include "sound.h"
#include "sound_output_state.h"

/* Retail reaches g_SDValue here through a literal address: the load has no
 * relocation, unlike the SD_ClearBusyFlag call beside it. Regional wrappers
 * supply their own address. */
#ifndef SD_ARM_BUSY_SDVALUE_ADDRESS
#define SD_ARM_BUSY_SDVALUE_ADDRESS 0x8009B45C
#endif
#ifndef gFile_SecondaryPayloadCompleteCallback
#define gFile_SecondaryPayloadCompleteCallback (*(void (**)(void))0x8009B128)
#endif

void SD_ArmBusyCallback(void) {
    (*(SDValue **)SD_ARM_BUSY_SDVALUE_ADDRESS)->busy = 1;
    gFile_SecondaryPayloadCompleteCallback = SD_ClearBusyFlag;
}
