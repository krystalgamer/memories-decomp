/* Reclassified from matching_c (#3859). This was
 * src/game/script_update_viewport_tween.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g4_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G4). Under gcc_2_8_1_g8, a single threshold,
 * it is 73 instructions against the target's 79, opcode distance 6. The
 * source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../game/graphics_frame.h"
#include "../game/script_state.h"
#include "../unmatched.h"
#include "../game/script_command_busy.h"

extern u16 gGraphics_uViewportX[4] asm("gGraphics_sViewportX");
extern u16 gGraphics_uViewportY[4] asm("gGraphics_sViewportY");

/* Two-axis smooth scroll stepper: on the first frame derives the per-frame
 * 16.16 deltas from the distance to the target over the remaining frame
 * count, then advances both accumulators, publishes their high halves as
 * the camera position, and snaps to the target when the counter runs out. */
void Script_UpdateViewportTween(void) {
    s32 sx;
    s32 sy;
    s32 n;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;

    if (func_8002E3B4() == 0) {
        sx = *(s16 *)&gGraphics_uViewportX[0];
        n = *(s16 *)&D_8009B29C;
        D_8009B294 = ((*(s16 *)&D_8009B2A8 - sx) << 16) / n;
        sy = *(s16 *)&gGraphics_uViewportY[0];
        D_8009B298 = ((*(s16 *)&D_8009B2AA - sy) << 16) / n;
        D_8009B284 = (sx << 16) | 0x8000;
        D_8009B288 = (sy << 16) | 0x8000;
    }

    a = D_8009B284;
    do { b = D_8009B294; } while (0);
    c = D_8009B288;
    d = D_8009B298;
    a = a + b;
    D_8009B284 = a;
    gGraphics_uViewportX[0] = a >> 16;
    c = c + d;
    D_8009B288 = c;
    gGraphics_uViewportY[0] = c >> 16;

    v = D_8009B29C - 1;
    D_8009B29C = v;
    if ((s16)v <= 0) {
        D_8009B27C = 0;
        gGraphics_uViewportX[0] = D_8009B2A8;
        gGraphics_uViewportY[0] = D_8009B2AA;
    }
}
