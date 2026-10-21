#define D_8009B318_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#define D_8009B142_IN_DATA
#include "graphics_frame.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "../overlays/main_menu/frontend.h"
#include "../psyq/rand.h"
#include "fade.h"
#include "main_run_frontend_loop.h"
#include "input.h"
#include "main_reset_frontend_runtime.h"
#include "movie_playback_control.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "main_frame.h"

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
                    if (gInput_wPad1Pressed &
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
