#include "../types.h"
#include "sound_output_state.h"

typedef struct {
    u8 pad_0000[0x1618];
    u8 busy;
} SoundCommandState;

#define g_SDValue (*(SoundCommandState **)0x8009B45C)
#define D_8009B128 (*(void (**)(void))0x8009B128)

void SD_ArmBusyCallback(void) {
    g_SDValue->busy = 1;
    D_8009B128 = SD_ClearBusyFlag;
}
