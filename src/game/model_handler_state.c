#include "../types.h"
#include "model_handler_state.h"
#include "model_camera_state.h"

u8 gModel_bCurrentDrawMode __attribute__((section(".sdata"))) = 0;
u8 gModel_bBlendRateActive __attribute__((section(".sdata"))) = 0;
u16 gModel_wDrawTintRedGreen __attribute__((section(".sdata"))) = 0;
u8 gModel_bDrawTintBlue __attribute__((section(".sdata"))) = 0;
u8 gModel_bPresentationTimer __attribute__((section(".sdata"))) = 0;
static u16 sModelHandlerState_PadAFEA
    __attribute__((section(".sdata"))) = 0;
u32 gModel_AnimatedBattleCameraEndpoint[2] __attribute__((section(".sdata"))) = {
    0x10000000,
    0x00040000,
};
char gModel_szWhyTrace[8] __attribute__((section(".sdata"))) = "WHY?\n";
u32 gModel_ExodiaOrbitOffset[2] __attribute__((section(".sdata"))) = {
    0xFA240000,
    0,
};
