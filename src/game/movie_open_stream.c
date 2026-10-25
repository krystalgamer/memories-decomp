#include "../types.h"
#define D_8009B142_IN_DATA
#include "movie_open_stream.h"
#include "movie_seek_and_start_read.h"
#include "movie_on_stream_end.h"
#include "graphics_frame.h"
#include "movie_frame_pipeline.h"
#include "movie_handoff_state.h"
#include "../psyq/libcd.h"
#include "../psyq/libpress.h"
#include "graphics_constants.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "../psyq/libgs.h"
#include "../psyq/libetc.h"

s32 Movie_OpenStream(CdlLOC *src, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    RECT rect;
    s32 m;
    s32 r;

    gMovie_bStreamEnded = 0;
    gMovie_bFadingOut = 0;
    gMovie_bCdVolume = 0xFF;
    gMovie_dwEndSector = a3;
    gMovie_bRgb24 = a4;
    gMovie_pWorkArea = gRuntime_pSharedWorkBase;
    gMovie_bCopyDisplayOnStop = a5;
    if (gMovie_bRgb24 != 0) {
        /* Retail sets $a0 and $a1 here even though GsGetActiveBuff takes
           no arguments. The call goes through a two-argument view of the
           libgs.h declaration so that setup survives with the prototype in
           scope, instead of through an implicit declaration. */
        m = ((int (*)(void *, s32))GsGetActiveBuff)(gRuntime_pSharedWorkBase, a5);
        rect.x = m * 0x140;
        rect.y = 0;
        rect.w = gGraphics_CurrentWidth.pixels;
        rect.h = gGraphics_CurrentHeight.word;
        ClearImage(&rect, gGraphics_bClearRed, gGraphics_bClearGreen, gGraphics_bClearBlue);
        DrawSync(0);
        VSync(0);
        GsSwapDispBuff();
        rect.x = (m ^ 1) * 0x140;
        rect.w = gGraphics_CurrentWidth.pixels;
        rect.y = 0;
        rect.h = gGraphics_CurrentHeight.word;
        ClearImage(&rect, gGraphics_bClearRed, gGraphics_bClearGreen, gGraphics_bClearBlue);
        rect.x = 0;
        rect.y = 0x100;
        rect.w = gGraphics_CurrentWidth.word * 0x1800 / 4096;
        rect.h = gGraphics_CurrentHeight.word;
        ClearImage(&rect, gGraphics_bClearRed, gGraphics_bClearGreen, gGraphics_bClearBlue);
        DrawSync(0);
        VSync(0);
        gLibGs_sActiveBufferIndex = 1;
        GsSwapDispBuff();
#ifdef VERSION_EUROPE
        GsInitGraph2(GRAPHICS_DEFAULT_WIDTH, 0x100, 4, 1, 1);
        GsDefDispBuff(0, 0, 0, 0x100);
        GsDISPENV.screen.y = 20;
        GsDISPENV.screen.h = 0x100;
#else
        GsDefDispBuff(0, 0, 0, 0x100);
        GsInitGraph2(GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 4, 1, 1);
#endif
    }
    if (src != 0) {
        gMovie_StreamLoc = *src;
    }
    if (a1 != 0) {
        gMovie_dwStartFrame = a1;
    } else {
        gMovie_dwStartFrame = 1;
    }
    if (a2 != 0) {
        gMovie_dwEndFrame = a2;
    } else {
        gMovie_dwEndFrame = 0xFFFF;
    }
    gMovie_bInputSlot = 0;
    gMovie_bOutputSlot = 0;
    DecDCTReset(0);
    DecDCToutCallback(Movie_OnDecodeOutput);
    DecDCTvlcBuild((u16 *)gMovie_pWorkArea);
    StSetRing((u32 *)(gMovie_pWorkArea + sizeof(DECDCTTAB)), 0x14);
    StClearRing();
    StSetStream(gMovie_bRgb24, gMovie_dwStartFrame, gMovie_dwEndFrame, 0, Movie_OnStreamEnd);
    if (gMovie_dwEndFrame >= 5) {
        gMovie_dwEndFrame = gMovie_dwEndFrame - 4;
    }
    Movie_SeekAndStartRead(&gMovie_StreamLoc);
    r = Movie_WaitAndDecodeFrame(0);
    if (r != 0) {
        return r;
    }
    return 0;
}
