#include "../types.h"
#include "graphics_frame.h"
#include "main_frame.h"
#include "input.h"
#include "sound_output.h"
#include "main_services.h"

/* Small data at 0x8009AF0C: prevents a nested VBlank callback from calling
   SD_VSync while the previous call is still running. */
u8 D_8009AF0C __attribute__((section(".sdata"))) = 0;

void Main_VBlankCB(void)
{
    D_8009B09C++;
    D_8009B0C4++;
    D_8009B0C3 = 1;
    D_8009B0C8++;
    Input_ReadRawPads();

    if (D_8009AF0C == 0) {
        D_8009AF0C = 1;
        SD_VSync();
        D_8009AF0C = 0;
        D_8009B0C3 = 0;
    }
}

void Main_AdvanceFrame(void)
{
    Main_RunFrameServices();
    Graphics_SyncFrame();
    Graphics_BeginFrame();
    Input_UpdatePads();
}

void Main_AdvanceFrames(s32 count)
{
    do {
        Main_AdvanceFrame();
    } while (--count != 0);
}
