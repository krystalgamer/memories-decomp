#include "../types.h"
#include "model_handler_state.h"

u8 D_8009AFE4 __attribute__((section(".sdata"))) = 0;
u8 D_8009AFE5 __attribute__((section(".sdata"))) = 0;
u16 D_8009AFE6 __attribute__((section(".sdata"))) = 0;
u8 D_8009AFE8 __attribute__((section(".sdata"))) = 0;
u8 D_8009AFE9 __attribute__((section(".sdata"))) = 0;
static u16 sModelHandlerState_PadAFEA
    __attribute__((section(".sdata"))) = 0;
u32 D_8009AFEC[2] __attribute__((section(".sdata"))) = {
    0x10000000,
    0x00040000,
};
char D_8009AFF4[8] __attribute__((section(".sdata"))) = "WHY?\n";
u32 D_8009AFFC[2] __attribute__((section(".sdata"))) = {
    0xFA240000,
    0,
};
