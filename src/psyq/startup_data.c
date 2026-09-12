#include "../types.h"
#include "startup_data.h"

extern void entrypoint(void);
extern u8 D_8009B4A8[];

u32 D_800906E0 __attribute__((section(".data"))) = 0;
u32 D_800906E4 __attribute__((section(".data"))) = 0;
u32 D_800906E8[7] = {
    0,
    (u32)entrypoint,
    0x0007DD08,
    (u32)&initialized_data_start,
    0x0000A828,
    (u32)D_8009B4A8,
    0x00063280,
};
