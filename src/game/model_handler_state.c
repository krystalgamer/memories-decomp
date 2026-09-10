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
u32 D_8009B004 __attribute__((section(".sdata"))) = 0;
u32 D_8009B008 __attribute__((section(".sdata"))) = 0;
char D_8009B00C[8] __attribute__((section(".sdata"))) = "~cff0";
char D_8009B014[8] __attribute__((section(".sdata"))) = "~cf00";
char D_8009B01C[8] __attribute__((section(".sdata"))) = "%s%d";
char D_8009B024[8] __attribute__((section(".sdata"))) = " VS ";
char D_8009B02C[4] __attribute__((section(".sdata"))) = "\n";
char D_8009B030[8] __attribute__((section(".sdata"))) = "BACK\n";
char D_8009B038[8] __attribute__((section(".sdata"))) = "STONE\n";
char D_8009B040[8] __attribute__((section(".sdata"))) = "HIT\n";
char D_8009B048[8] __attribute__((section(".sdata"))) = "OUCH\n";
char D_8009B050[8] __attribute__((section(".sdata"))) = "TERM\n";
