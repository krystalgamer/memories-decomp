#define D_8009B098_IN_DATA
#include "../types.h"
#include "graphics_frame.h"
#include "display_object_transition.h"
#include "main_frame.h"
#include "display_object_api.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "fade.h"
#include "file_transfer.h"
#include "main_run_boot_sequence.h"
#include "graphics_constants.h"
#include "display_object_helpers.h"
#include "main_reset_frontend_runtime.h"
#include "sound_voice_selection.h"
#include "../external_funcs.h"
#include "../unmatched.h"

void Main_RunBootSequence(s32 mode)
{
    register u8 *object;
    register u8 *first;
    D_8009B428 = 0;
    if (mode == 0) {
        File_RequestAsyncTransfer(0, 0, 0x1F85, 0x22, func_800434F4, 0, 0);
        File_WaitForTransfers();
    }
    File_RequestAsyncTransfer(
        0, 0, 0x1690, 0x36, Main_LoadBootPackageStage, 0, 0
    );
    if (mode != 0) {
        int display;
        File_WaitForTransfers();
        FntLoad(0x2C0, 0);
        display = FntOpen(
            0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
            0, 1000
        );
        SetDumpFnt(display);
        D_8009B098 = 0;
        func_80047AD0(2);
        func_80012D84(4);
        File_WaitForTransfers();
        return;
    }
    Fade_InitIn();
    object = func_800400AC(func_8004002C(), 2);
    func_800428A8(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  D_801AF000);
    *(u16 *)(object + 8) |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8004365C(0, (DisplayObject *)object);
    func_800438B8(4);
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 1000
    ));
    D_8009B098 = 0;
    CdFlush();
    first = object;
    func_801680F4();
    while (func_80168160(1) != 0) {
    }
    DsInit();
    object = func_800400AC(func_8004002C(), 2);
    func_800428A8(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  D_801AF000);
    *(u16 *)(object + 8) |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8004365C((DisplayObject *)first, (DisplayObject *)object);
    func_80047AD0(2);
    func_80012D84(4);
    File_RequestMainMenuPackage();
    func_800438B8(0xB4);
    Fade_WaitInitOut();
    Main_ResetFrontendRuntime();
}
