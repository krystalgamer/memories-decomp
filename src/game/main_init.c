/*
 * Reclassified from matching_c (#3859). The original candidate required a
 * hard-register pin under gcc_2_8_1_g8_split. Disabling sched1 lets GCC keep
 * the framebuffer address ahead of the volatile initialization stores
 * without source-level register control.
 */
#define D_8009B0D8_IS_VOLATILE
#define D_8009B0C0_IS_VOLATILE
#define D_8009B0D1_IS_VOLATILE
#define D_8009B230_IN_DATA
#define GRAPHICS_ACTIVE_FRAME_BUFFER_IS_VOLATILE
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "../unmatched.h"
#include "duel_side_state.h"
#include "duel_effect_resource_setup.h"
#include "display_object_core.h"
#include "sound.h"
#include "text_init_decimal_digit_glyph_map.h"
#include "graphics_frame.h"
#include "main_frame.h"
#include "../psyq/libapi.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libetc.h"
#include "../psyq/crt.h"
#include "../psyq/setjmp.h"
#include "../psyq/rand.h"
#include "fade.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "duel_effect_reset_state.h"
#include "main_run_boot_sequence.h"
#include "main_run_frontend_loop.h"
#include "main_loop.h"
#include "main_reset_frontend_runtime.h"
#include "main_services.h"
#include "rand_constants.h"
#include "movie_playback_control.h"
#include "main_mode_state.h"
#include "../psyq/libgs.h"
#include "graphics_frame_controls.h"

s32 Main_Init(void)
{
    s32 r;
    s32 t;
    register GraphicsFrameBuffer *p;

    __main();
#ifdef VERSION_EUROPE
    SetVideoMode(1);
#endif
    EnterCriticalSection();
    ResetCallback();
    GsInitVcount();
    ExitCriticalSection();
    StopCallback();
    SetMem(2);
    SetDispMask(0);
    Fade_DisableOrderingTables();
    p = gGraphics_aFrameBuffers;
    gGraphics_dwSyncedFrameCount = 0;
    gGraphics_dwSyncVBlankCounter = 0;
    gGraphics_bSyncVBlankTarget = 0;
    gMain_dwFrameCounter = 0;
    t = gMain_dwFrameCounter;
#ifdef VERSION_EUROPE
    D_8009C02B = BUILD_LANGUAGE_INDEX;
#endif
    gMain_bVBlankSoundPending = 0;
    gGraphics_bExtraFrameStep = 0;
    gGraphics_nBaseFrameStep = 1;
    gGraphics_wFlags = 0x5000;
    D_8009B0D1 = 0;
    *(u8 *)&gDuel_wTwoPlayerCardViewMode = 1;
    gGraphics_pActiveFrameBuffer = p;
    gSaveData_dwVBlankCounter = t;
    Main_InitHardware(p);
    DisplayObject_Reset();
    Fade_Init();
    Main_ClearFrameServiceCallbacks();
    Movie_ResetPlaybackState();
    DuelEffect_ResetState();
    Text_InitDecimalDigitGlyphMap();
    SD_InitState(gFile_nMasterXaLba[0]);
    VSyncCallback(Main_VBlankCB);
    Sound_InitFrontend();
    srand(RAND_BOOT_SEED);
    SetDispMask(1);
    Main_ResetFrontendRuntime();
    Main_ResetFrontendRuntime();
    Main_RunBootSequence(0);
    r = setjmp(gMain_RecoveryJmpBuf);
    Main_ResetFrontendRuntime();
    if (r != 0) {
        File_RequestMainMenuPackage();
        File_WaitForTransfers();
    }
    Main_ApplyMenuSelection(Main_RunFrontendLoop());
    D_8009B269 = 8;
    Main_Loop();
    return 0;
}
