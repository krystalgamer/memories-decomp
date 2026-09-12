/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/main_init.c.
 */
#define D_8009B0D8_IS_VOLATILE
#define D_8009B0C0_IS_VOLATILE
#define D_8009B230_IN_DATA
#define GRAPHICS_ACTIVE_FRAME_BUFFER_IS_VOLATILE
#include "../types.h"
#include "../unmatched.h"
#include "../game/duel_side_state.h"
#include "../game/display_object_api.h"
#include "../game/sound.h"
#include "../game/func_8003B5C8.h"
#include "../game/graphics_frame.h"
#include "../game/main_frame.h"
#include "../psyq/libapi.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libetc.h"
#include "../psyq/setjmp.h"
#include "../psyq/rand.h"
#include "../game/fade.h"
#include "../game/file_transfer.h"
#include "../game/main_menu_selection.h"
#include "../game/func_80035A64.h"
#include "../game/main_run_boot_sequence.h"
#include "../game/func_80043BCC.h"
#include "../game/main_loop.h"
#include "../game/main_reset_frontend_runtime.h"
#define FUNC_80013154_NO_ARGS
#include "../game/main_services.h"
#include "../game/rand_constants.h"
#include "../game/movie_playback_control.h"

extern volatile u8 D_8009B0D1;
extern u8 D_8009B269 __attribute__((section(".data")));
extern void __main(void);

s32 Main_Init(void)
{
    s32 r;
    s32 t;
    register GraphicsFrameBuffer *p __asm__("$4");

    __main();
    EnterCriticalSection();
    ResetCallback();
    GsInitVcount();
    ExitCriticalSection();
    StopCallback();
    SetMem(2);
    SetDispMask(0);
    func_80015D0C();
    p = gGraphics_aFrameBuffers;
    D_8009B0CC = 0;
    D_8009B0C8 = 0;
    D_8009B0C0 = 0;
    D_8009B09C = 0;
    t = D_8009B09C;
    D_8009B0C3 = 0;
    D_8009B0C1 = 0;
    D_8009B0D8 = 1;
    D_8009B098 = 0x5000;
    D_8009B0D1 = 0;
    *(u8 *)&D_8009B230 = 1;
    gGraphics_pActiveFrameBuffer = p;
    D_8009B0C4 = t;
    func_80013154();
    func_800403F0();
    func_800151B0();
    func_800134B4();
    func_80035A58();
    func_80035A64();
    func_8003B5C8();
    SD_InitState(D_800E9EC0[0]);
    VSyncCallback(Main_VBlankCB);
    Sound_InitFrontend();
    srand(RAND_BOOT_SEED);
    SetDispMask(1);
    Main_ResetFrontendRuntime();
    Main_ResetFrontendRuntime();
    Main_RunBootSequence(0);
    r = setjmp(D_800E9DC0);
    Main_ResetFrontendRuntime();
    if (r != 0) {
        File_RequestMainMenuPackage();
        File_WaitForTransfers();
    }
    Main_ApplyMenuSelection(func_80043BCC());
    D_8009B269 = 8;
    Main_Loop();
    return 0;
}
