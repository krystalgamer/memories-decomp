#define D_8009B0D8_IS_VOLATILE
#define D_8009B0C0_IS_VOLATILE
#define D_8009B230_IN_DATA
#define MAIN_MODE_STATE_NEXT_IN_DATA
#include "../types.h"
#include "main_mode_state.h"
#include "../unmatched.h"
#include "duel_side_state.h"
#include "display_object_config.h"
#include "sound.h"
#include "func_8003B5C8.h"
#include "graphics_frame.h"
#include "main_frame.h"
#include "../psyq/libapi.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libetc.h"
#include "../psyq/setjmp.h"
#include "../psyq/rand.h"
#include "fade.h"
#include "file_transfer.h"
#include "func_8002D458.h"
#include "func_80035A64.h"
#include "main_run_boot_sequence.h"
#include "func_80043BCC.h"
#include "main_loop.h"
#include "main_reset_frontend_runtime.h"
#define FUNC_80013154_NO_ARGS
#include "main_services.h"
#include "rand_constants.h"
#include "movie_playback_control.h"

extern volatile u8 D_8009B0D1;
extern void *volatile D_8009B0B4;
extern void __main(void);

s32 Main_Init(void)
{
    s32 r;
    s32 t;
    register void *p __asm__("$4");

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
    func_8002D458(func_80043BCC());
    D_8009B269 = 8;
    Main_Loop();
    return 0;
}
