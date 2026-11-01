#define D_8009B098_IN_DATA
#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B428-0x8009B430, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 gMain_bFrontendPhase;
static u8 D_8009B429;
static u16 D_8009B42A;
static u32 D_8009B42C;

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
#include "file_transfer_state.h"
#include "main_boot_load_stages.h"
#include "main_run_boot_sequence.h"
#include "graphics_constants.h"
#include "display_object_helpers.h"
#include "main_reset_frontend_runtime.h"
#include "sound_voice_selection.h"
#include "../external_funcs.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#ifdef VERSION_EUROPE
#include "duel_effect_resource_setup.h"
#endif

#if defined(VERSION_EUROPE)
/* The European boot sequence factors the debug-font setup and the boot
   package request out into two helpers: the package is one 110-sector
   package per text language, from 0x1962, and the main path asks for
   BUILD_LANGUAGE_INDEX once the logos are done. */
void Main_InitDebugFont(void)
{
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 0, 1000
    ));
}

void Main_RequestLanguageBootPackage(s32 language, s32 wait)
{
    File_RequestAsyncTransfer(
        0, 0, language * 110 + 0x1962, 110, Main_LoadBootPackageStage, 0, 0
    );
    if (wait) {
        File_WaitForTransfers();
        Main_InitDebugFont();
    }
}

void Main_RunBootSequence(s32 mode)
{
    DisplayObject *object;
    DisplayObject *first;

    gMain_bFrontendPhase = 0;
    gGraphics_wFlags = 0;
    if (mode != 0) {
        Main_RequestLanguageBootPackage(gText_bLanguageIndex, 1);
        SD_ReplaceValueLink(2);
        Main_AdvanceFrames(4);
    } else {
        File_RequestAsyncTransfer(0, 0, 0x2503, 0x25, Main_LoadBootImageStage, 0, 0);
        File_WaitForTransfers();
        Fade_InitIn();
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x13, 0x100,
                      gDisplay_abResourceBank0);
        object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                         DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_CrossfadeReplace(0, object);
        first = object;
        Main_HoldBootScreen(0xB4);
        CdFlush();
        func_801680F4();
        while (func_80168160(1) != 0) {
        }
        DsInit();
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x13, 0x100,
                      gDisplay_abResourceBank0);
        object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                         DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_CrossfadeReplace(first, object);
        SD_ReplaceValueLink(2);
        Main_AdvanceFrames(4);
        Main_HoldBootScreen(0xB4);
        gText_bLanguageIndex = BUILD_LANGUAGE_INDEX;
        Main_RequestLanguageBootPackage(BUILD_LANGUAGE_INDEX, 0);
        Fade_WaitInitOut();
        Main_ResetFrontendRuntime();
        while (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState_abs) != 0) {
            Main_AdvanceFrame();
        }
        Main_InitDebugFont();
        File_RequestMainMenuPackage();
    }
    File_WaitForTransfers();
}
#elif defined(VERSION_JAPAN)
void Main_RunBootSequence(s32 mode)
{
    DisplayObject *object;
    DisplayObject *first;
    gMain_bFrontendPhase = 0;
    if (mode == 0) {
        File_RequestAsyncTransfer(0, 0, 0x1F90, 0x22, Main_LoadBootImageStage, 0, 0);
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
        gGraphics_wFlags = 0;
        SD_ReplaceValueLink(2);
        Main_AdvanceFrames(4);
        File_WaitForTransfers();
        return;
    }
    Fade_InitIn();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    first = object;
    first->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(0, first);
    Main_HoldBootScreen(4);
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 1000
    ));
    gGraphics_wFlags = 0;
    CdFlush();
    func_801680F4();
    while (func_80168160(1) != 0) {
    }
    DsInit();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(first, object);
    first = object;
    SD_ReplaceValueLink(2);
    Main_AdvanceFrames(4);
    Main_HoldBootScreen(0xB4);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 2, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(first, object);
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
    gMain_bFrontendPhase = 0;
    if (mode == 0) {
        File_RequestAsyncTransfer(0, 0, 0x1F85, 0x22, Main_LoadBootImageStage, 0, 0);
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
        gGraphics_wFlags = 0;
        SD_ReplaceValueLink(2);
        Main_AdvanceFrames(4);
        File_WaitForTransfers();
        return;
    }
    Fade_InitIn();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(0, object);
    Main_HoldBootScreen(4);
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 1000
    ));
    gGraphics_wFlags = 0;
    CdFlush();
    first = object;
    func_801680F4();
    while (func_80168160(1) != 0) {
    }
    DsInit();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(first, object);
    SD_ReplaceValueLink(2);
    Main_AdvanceFrames(4);
    File_RequestMainMenuPackage();
    Main_HoldBootScreen(0xB4);
    Fade_WaitInitOut();
    Main_ResetFrontendRuntime();
}
#endif
