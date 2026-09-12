#define D_8009B0A3_IS_VOLATILE_SCALAR
#define D_8009B142_IN_DATA_VOLATILE
#define GRAPHICS_DRAW_ENV_IS_VOLATILE
#define D_8009B14A_IN_DATA_VOLATILE
#define GRAPHICS_INIT_STATE_IS_VOLATILE_SCALAR
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libmcrd.h"
#include "../psyq/rand.h"
#include "fade.h"
#include "../unmatched.h"
#include "file_transfer.h"
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
   The boot-time graphics and input start-up that installs the pump follows in
   this unit; the pad-driven screen-offset adjustment loop remains a candidate
   in src/candidates/func_80013360.c. The last, the reset of the callback
   registry the pump walks, is in func_800134B4.c; the pump and the reset share
   the D_800E9DB0 slots and D_8009B0B8. */

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
/* Boot-time graphics and input startup. The work area contains two 0x5160
 * byte frame buffers; each receives four ordering tables before the display
 * environment and frontend services are initialized. */
void func_80013154(u8 *base)
{
    u8 *buf;
    s32 k;
    s32 off;
    s32 six;
    u16 count;

    ResetGraph(0);
    GsInitGraph(GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 4, 1, 0);
    GsDefDispBuff(0, 0, 0x140, 0);
    six = 6;
    buf = base;
    D_8009B0AD = 1;
    D_8009B0D0 = 1;
    D_8009B0A8 = 0;
    D_8009B14C = 1;
    D_8009B144 = 1;
    D_8009B14B = 1;
    D_8009B143 = 1;
    D_8009B14A = 1;
    D_8009B142 = 1;
    D_800FE048[0].isbg = 1;
    D_800FE048[0].dtd = 1;
    D_800FE048[0].r0 = 1;
    D_800FE048[0].g0 = 1;
    D_800FE048[0].b0 = 1;
    count = six;
    D_8009B0A0 = 2;
    D_8009B0A1 = count;
    D_8009B0A2 = 0xC;
    D_8009B0A3 = count;
next:
    k = 3;
    off = 0x514C;
    *(s32 *)(buf + 0x5110) = 2;
    *(u8 **)(buf + 0x5128) = buf + 0x10;
    *(s32 *)(buf + 0x5138) = 0xC;
    *(u8 **)(buf + 0x513C) = buf + 0x110;
    *(u8 **)(buf + 0x5114) = buf;
    *(s32 *)(buf + 0x5124) = six;
    *(s32 *)(buf + 0x514C) = six;
    *(u8 **)(buf + 0x5150) = buf + 0x4110;
    do {
        GsClearOt(0, k, (GsOT *)(buf + off));
        off -= 0x14;
        k--;
    } while (k >= 0);
    buf += 0x5160;
    if ((s32)buf < (s32)(base + 0xA2C0)) {
        goto next;
    }
    gGraphics_DispEnv = D_800FE0A8;
    InitGeom();
    GsInit3D();
    GsSetOrign(0, 0);
    SetGeomScreen(0x12C);
    Input_InitPads();
    MemCardInit(1);
    File_SetPositionTable();
    srand(RAND_GRAPHICS_INIT_SEED);
}
