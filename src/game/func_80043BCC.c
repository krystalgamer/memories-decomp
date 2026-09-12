#define D_8009B318_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#define D_8009B142_IN_DATA
#include "graphics_frame.h"
#include "duel_effect.h"
#include "../overlays/main_menu/entrypoints.h"
#include "../psyq/rand.h"
#include "fade.h"
#include "func_80043BCC.h"
#include "input.h"
#include "main_reset_frontend_runtime.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "main_frame.h"
#include "movie_playback_control.h"

s32 func_80043BCC(void) {
    s32 r;
    u32 f;
    s32 g;

    Fade_WaitInitIn();

    for (;;) {
        D_8009B428 = 0;
        for (;;) {
            rand();
            func_80012D4C();
            f = D_8009B428;
            if ((f & 1) == 0) {
                if ((f & 0x80) == 0) {
                    D_8009B428 = f | 0x80;
                    Main_ResetFrontendRuntime();
                    func_8003594C(0);
                    D_8009B142 = 0xFF;
                    D_8009B143 = 0xFF;
                    D_8009B144 = 0xFF;
                    continue;
                }
                if (f & 0x40) {
                    if (gInput_wPad1Pressed &
                        (PAD_BUTTON_START | PAD_BUTTON_CONFIRM_MASK)) {
                        TextBox_Destroy(D_800EB0F8);
                        D_8009B428 = 1;
                    }
                    continue;
                }
                g = D_8009B318;
                if (g & 0x80) {
                    continue;
                }
                D_8009B428 = 1;
                if ((g & 0x40) == 0) {
                    func_800156DC();
                }
                Fade_WaitInitOut();
                continue;
            }
            if ((f & 0x80) == 0) {
                D_8009B428 = f | 0x80;
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
