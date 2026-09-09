#define SDVALUE_CUSTOM_EXTERN
#include "../types.h"
#include "sound.h"
#include "sound_output_state.h"

/* The absolute spelling is load-bearing in this -G8 unit. */
#define g_SDValue (*(SDValue **)0x8009B45C)
#define D_8009B128 (*(void (**)(void))0x8009B128)

void SD_ArmBusyCallback(void) {
    g_SDValue->busy = 1;
    D_8009B128 = SD_ClearBusyFlag;
}
