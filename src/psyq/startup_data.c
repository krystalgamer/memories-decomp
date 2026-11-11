#include "../types.h"
#include "../game/main.h"
#include "startup_data.h"

u32 D_800906E0 __attribute__((section(".data"))) = 0;
u32 D_800906E4 __attribute__((section(".data"))) = 0;
u32 D_800906E8[7] = {
    0,
    (u32)entrypoint,
    0x0007DD08,
    (u32)&initialized_data_start,
    0x0000A828,
    (u32)gGraphics_aFrameBuffers,
    0x00063280,
};
