#define D_8009B098_IN_DATA
#include "../types.h"
#include "graphics_frame.h"
#include "display_object_transition.h"
#include "main_frame.h"
#include "display_object_core.h"
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
#ifdef VERSION_EUROPE
#include "duel_effect_resource_setup.h"
#endif

#if defined(VERSION_EUROPE)
/* The European boot sequence factors the debug-font setup and the boot
   package request out into two helpers: the package is one 110-sector
   package per text language, from 0x1962, and the main path asks for
   language 0 once the logos are done. */
void func_80043B7C(void)
{
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 0, 1000
    ));
}

void func_80043BC8(s32 language, s32 wait)
{
    File_RequestAsyncTransfer(
        0, 0, language * 110 + 0x1962, 110, Main_LoadBootPackageStage, 0, 0
    );
    if (wait) {
        File_WaitForTransfers();
        func_80043B7C();
    }
}

void Main_RunBootSequence(s32 mode)
{
    DisplayObject *object;
    DisplayObject *first;

    D_8009B428 = 0;
    D_8009B098 = 0;
    if (mode != 0) {
        func_80043BC8(D_8009C02B, 1);
        func_80047AD0(2);
        Main_AdvanceFrames(4);
    } else {
        File_RequestAsyncTransfer(0, 0, 0x2503, 0x25, func_800434F4, 0, 0);
        File_WaitForTransfers();
        Fade_InitIn();
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x13, 0x100,
                      D_801AF000);
        object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                         DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_8004365C(0, object);
        first = object;
        Main_HoldBootScreen(0xB4);
        CdFlush();
        func_801680F4();
        while (func_80168160(1) != 0) {
        }
        DsInit();
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x13, 0x100,
                      D_801AF000);
        object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                         DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_8004365C(first, object);
        func_80047AD0(2);
        Main_AdvanceFrames(4);
        Main_HoldBootScreen(0xB4);
        D_8009C02B = 0;
        func_80043BC8(0, 0);
        Fade_WaitInitOut();
        Main_ResetFrontendRuntime();
        while (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                D_8009B134_abs) != 0) {
            Main_AdvanceFrame();
        }
        func_80043B7C();
        File_RequestMainMenuPackage();
    }
    File_WaitForTransfers();
}
#elif defined(VERSION_JAPAN)
void Main_RunBootSequence(s32 mode)
{
    DisplayObject *object;
    DisplayObject *first;
    D_8009B428 = 0;
    if (mode == 0) {
        File_RequestAsyncTransfer(0, 0, 0x1F90, 0x22, func_800434F4, 0, 0);
        File_WaitForTransfers();
    }
    File_RequestAsyncTransfer(
        0, 0, 0x1690, 0x25, Main_LoadBootPackageStage, 0, 0
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
        Main_AdvanceFrames(4);
        File_WaitForTransfers();
        return;
    }
    Fade_InitIn();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  D_801AF000);
    first = object;
    first->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8004365C(0, first);
    Main_HoldBootScreen(4);
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 1000
    ));
    D_8009B098 = 0;
    CdFlush();
    func_801680F4();
    while (func_80168160(1) != 0) {
    }
    DsInit();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  D_801AF000);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8004365C(first, object);
    first = object;
    func_80047AD0(2);
    Main_AdvanceFrames(4);
    Main_HoldBootScreen(0xB4);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 2, 0x10, 0x100,
                  D_801AF000);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8004365C(first, object);
    File_RequestMainMenuPackage();
    Main_HoldBootScreen(0xB4);
    Fade_WaitInitOut();
    Main_ResetFrontendRuntime();
}
#else
void Main_RunBootSequence(s32 mode)
{
    register DisplayObject *object;
    register DisplayObject *first;
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
        Main_AdvanceFrames(4);
        File_WaitForTransfers();
        return;
    }
    Fade_InitIn();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  D_801AF000);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8004365C(0, object);
    Main_HoldBootScreen(4);
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
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  D_801AF000);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8004365C(first, object);
    func_80047AD0(2);
    Main_AdvanceFrames(4);
    File_RequestMainMenuPackage();
    Main_HoldBootScreen(0xB4);
    Fade_WaitInitOut();
    Main_ResetFrontendRuntime();
}
#endif
