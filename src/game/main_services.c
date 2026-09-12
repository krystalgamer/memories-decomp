#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libmcrd.h"
#include "../psyq/rand.h"
#include "fade.h"
#include "../unmatched.h"
#include "func_800136D4.h"
#include "func_80041340.h"
#include "graphics_constants.h"
#include "graphics_frame.h"
#include "main_frame.h"
#include "input.h"
#include "rand_constants.h"
#include "main_services.h"

/* The resident system layer's per-frame service pump. It is the first of
   four contiguous functions that are the only run in the region built with
   gcc_2_8_1_g8_split - their neighbours on both sides use other profiles.
   The next two are the boot-time graphics and input start-up that installs
   the pump, in func_80013154.c, and the pad-driven screen-offset adjustment
   loop in src/candidates/func_80013360.c. The last, the reset of the callback
   registry the pump walks, is in
   func_800134B4.c; the pump and the reset share the D_800E9DB0 slots and
   D_8009B0B8. */

s32 runtime_gp __attribute__((section(".sdata"))) = 0x3C;
extern s32 D_8009B0A4;
extern s32 D_8009B0B0;
extern s32 D_8009B0BC;
extern s32 D_8009B0D4;

/* Per-frame dispatcher: runs the two fixed housekeeping calls, then each of
   the 4 slots in D_800E9DB0 and the single D_8009B0B8 callback if set. If
   neither of the two progress pairs (f1A8/f19C, f1B4/f1CC) has advanced and
   the watchdog counter D_8009AF08 underflows, resets the counter to 0x3C
   and re-syncs both progress pairs. Finishes with func_80014A5C/func_800136D4. */
void func_8001306C(void) {
    void (*fn)(void);
    s32 i;
    s32 cnt;

    Fade_DrawOverlay();
    func_80041340();

    for (i = 0; i < 4; i++) {
        fn = D_800E9DB0[i];
        if (fn != 0) {
            fn();
        }
    }

    fn = D_8009B0B8;
    if (fn != 0) {
        fn();
    }

    if (D_8009B0B0 < D_8009B0A4 || D_8009B0BC < D_8009B0D4) {
        goto reset;
    }
    cnt = runtime_gp - 1;
    runtime_gp = cnt;
    if (cnt < 0) {
    reset:
        runtime_gp = 0x3C;
        D_8009B0B0 = D_8009B0A4;
        D_8009B0BC = D_8009B0D4;
    }

    func_80014A5C(0);
    func_800136D4();
}
