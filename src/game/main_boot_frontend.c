#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B428-0x8009B430, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 gMain_bFrontendPhase;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
static u32 D_8009B42C;

#include "graphics_frame.h"
#include "main_boot_screen.h"
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
#include "display_asset_banks.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "../overlays/main_menu/frontend.h"
#include "../psyq/rand.h"
#include "main_run_frontend_loop.h"
#include "input.h"
#include "movie_playback_control.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#ifdef VERSION_EUROPE
#include "duel_effect_resource_setup.h"
#endif

/* The boot sequence and the front-end loop that follows it, one translation
 * unit (#2602). Retail reads gMain_bFrontendPhase gp-relative only from these
 * two functions; both compile at gcc_2_8_1_g8_split. */

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
        while (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState) != 0) {
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

#ifndef MAIN_FRONTEND_CONFIRM_MASK
#define MAIN_FRONTEND_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

s32 Main_RunFrontendLoop(void) {
    s32 r;
    u32 f;
    s32 g;

#ifdef VERSION_EUROPE
    /* The European loop resets the frontend runtime before the fade. */
    Main_ResetFrontendRuntime();
#endif
    Fade_WaitInitIn();

    for (;;) {
        gMain_bFrontendPhase = 0;
        for (;;) {
            rand();
            Main_AdvanceFrame();
            f = gMain_bFrontendPhase;
            if ((f & 1) == 0) {
                if ((f & 0x80) == 0) {
                    gMain_bFrontendPhase = f | 0x80;
                    Main_ResetFrontendRuntime();
                    Movie_Play(0);
                    gGraphics_bClearBlue = 0xFF;
                    gGraphics_bClearGreen = 0xFF;
                    gGraphics_bClearRed = 0xFF;
                    continue;
                }
                if (f & 0x40) {
                    if (gInput_awPadPressed[0] &
                        (PAD_BUTTON_START | MAIN_FRONTEND_CONFIRM_MASK)) {
                        TextBox_Destroy(gText_aChannels);
                        gMain_bFrontendPhase = 1;
                    }
                    continue;
                }
                g = gMovie_bPlaybackState;
                if (g & 0x80) {
                    continue;
                }
                gMain_bFrontendPhase = 1;
                if ((g & 0x40) == 0) {
                    Fade_CutToWhite();
                }
                Fade_WaitInitOut();
                continue;
            }
            if ((f & 0x80) == 0) {
                gMain_bFrontendPhase = f | 0x80;
                Main_ResetFrontendRuntime();
                MainMenu_InitFrontendMenu(0, 0);
                Fade_StartIn();
            }
            r = MainMenu_UpdateFrontendMenu();
            if (r != -1) {
                break;
            }
        }
        SD_BGMFadeOut();
        Fade_WaitOut();
        MainMenu_DestroyFrontendMenu();
        Main_ResetFrontendRuntime();
        if (r != -2) {
            return r;
        }
    }
}
