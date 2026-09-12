/*
 * Main_Init at 0x80012B50 matches all 388 bytes under the existing uniform
 * gcc_2_8_1_g8_split profile. The historical pointer pin hid a missing
 * argument: func_80013154 consumes the graphics work-area base in a0.
 * Passing that base through its canonical u8 * prototype retains the
 * pointer across the volatile initialization stores and recovers every
 * instruction without a register binding or no-argument ABI view.
 *
 * The boot callback order, repeated frontend reset, setjmp recovery,
 * package reload and final menu dispatch remain unchanged. Existing
 * historical attempts are retained; the explicit-argument result is
 * recorded separately as reclassification evidence.
 */
#define GRAPHICS_WORK_AREA_IS_VOLATILE
#define D_8009B0D1_IS_VOLATILE
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define D_8009B0D8_IS_VOLATILE
#define D_8009B0C0_IS_VOLATILE
#define D_8009B230_IN_DATA
#include "../types.h"
#include "../unmatched.h"
#include "../psyq/gcc_runtime.h"
#include "../psyq/libapi.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libetc.h"
#include "../psyq/setjmp.h"
#include "../psyq/rand.h"
#include "main_init.h"
#include "main_mode_state.h"
#include "duel_side_state.h"
#include "display_object_api.h"
#include "sound.h"
#include "func_8003B5C8.h"
#include "graphics_frame.h"
#include "main_frame.h"
#include "fade.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "func_80035A64.h"
#include "main_run_boot_sequence.h"
#include "func_80043BCC.h"
#include "main_loop.h"
#include "main_reset_frontend_runtime.h"
#include "main_services.h"
#include "rand_constants.h"
#include "movie_playback_control.h"

s32 Main_Init(void)
{
    s32 r;
    s32 t;
    u8 *p;

    __main();
    EnterCriticalSection();
    ResetCallback();
    GsInitVcount();
    ExitCriticalSection();
    StopCallback();
    SetMem(2);
    SetDispMask(0);
    func_80015D0C();
    p = D_8009B4A8;
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
    D_8009B0B4 = p;
    D_8009B0C4 = t;
    func_80013154(p);
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
