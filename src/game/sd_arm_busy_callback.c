#define SDVALUE_CUSTOM_EXTERN
#include "../types.h"
#include "sound.h"
#include "sound_output_state.h"

/* The absolute spellings are load-bearing in this -G8 unit. */
#ifndef SD_VALUE_POINTER_ADDRESS
#define SD_VALUE_POINTER_ADDRESS 0x8009B45C
#endif

#ifndef SD_BUSY_CALLBACK_SLOT_ADDRESS
#define SD_BUSY_CALLBACK_SLOT_ADDRESS 0x8009B128
#endif

#define g_SDValue (*(SDValue **)SD_VALUE_POINTER_ADDRESS)
#define D_8009B128 (*(void (**)(void))SD_BUSY_CALLBACK_SLOT_ADDRESS)

void SD_ArmBusyCallback(void) {
    g_SDValue->busy = 1;
    D_8009B128 = SD_ClearBusyFlag;
}
