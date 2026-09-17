#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "main_frame.h"
#include "fade.h"
#include "main_modes.h"
#include "main_debug.h"
#include "main_loop.h"
#include "main_reset_frontend_runtime.h"
#include "../unmatched.h"
#include "main_mode_state.h"

void Main_Loop(void) {
    Main_PrepareFrontendLoop();
    for (;;) {
        u8 v;
        Main_AdvanceFrame();
        v = D_8009B26C;
        if ((v & 0x80) == 0) {
            D_8009B26C = v | 0x80;
            Main_ResetFrontendRuntime();
        } else {
            gMain_apfnModeRunner[v & 0x1F]();
            if ((D_8009B26C & 0x40) == 0) Fade_WaitOut();
        }
    }
}
