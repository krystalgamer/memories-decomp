#include "../types.h"
#include "graphics_frame.h"
#include "main_frame.h"
#include "input.h"
#include "sound_output.h"
#include "main_services.h"

/* Small data at 0x8009AF0C: prevents a nested VBlank callback from calling
   SD_VSync while the previous call is still running. */
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
u8 gMain_bSoundVSyncBusy __attribute__((section(".sdata"))) = 0;
#endif

void Main_VBlankCB(void)
{
    gMain_dwFrameCounter++;
    gSaveData_dwVBlankCounter++;
    gMain_bVBlankSoundPending = 1;
    gGraphics_dwSyncVBlankCounter++;
    Input_ReadRawPads();

    if (gMain_bSoundVSyncBusy == 0) {
        gMain_bSoundVSyncBusy = 1;
        SD_VSync();
        gMain_bSoundVSyncBusy = 0;
        gMain_bVBlankSoundPending = 0;
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
