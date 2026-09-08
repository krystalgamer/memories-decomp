/* func_80013360 re-reads the pad word on each path and needs it out of
   small data; see the arms in input.h. */
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libmcrd.h"
#include "../psyq/rand.h"
#include "fade.h"
#include "file_set_position_table.h"
#include "graphics_constants.h"
#include "graphics_frame.h"
#include "input.h"
#include "rand_constants.h"

/* The resident system layer: the per-frame service pump, the boot-time
   graphics and input start-up that installs it, the pad-driven screen-offset
   adjustment loop, and the reset of the callback registry the pump walks.
   The four are contiguous and are the only run in the region built with
   gcc_2_8_1_g8_split - their neighbours on both sides use other profiles -
   and the pump and the reset share the D_800E9DB0 slots and D_8009B0B8. */

/* The four per-frame callback slots the service pump walks, and the single
   extra callback beside them. */
extern void (*D_800E9DB0[4])(void);
extern void (*D_8009B0B8)(void);

extern s32 runtime_gp;
extern s32 D_8009B0A4;
extern s32 D_8009B0B0;
extern s32 D_8009B0BC;
extern s32 D_8009B0D4;
extern u16 D_8009B098;

/* The init block is a run of byte stores to distinct globals; declared
   volatile so the emitted order is the source order (see Main_Init). The
   six D_8009B14x bytes are addressed %hi/%lo in retail, so they sit outside
   small data. */
extern volatile u8 D_8009B0AD;
extern volatile u8 D_8009B0D0;
extern volatile u8 D_8009B0A8;
extern volatile u8 D_8009B14C __attribute__((section(".data")));
extern volatile u8 D_8009B144 __attribute__((section(".data")));
extern volatile u8 D_8009B14B __attribute__((section(".data")));
extern volatile u8 D_8009B143 __attribute__((section(".data")));
extern volatile u8 D_8009B14A __attribute__((section(".data")));
extern volatile u8 D_8009B142 __attribute__((section(".data")));
extern volatile u8 D_800FE048[];
extern volatile u8 D_8009B0A0;
extern volatile u8 D_8009B0A1;
extern volatile u8 D_8009B0A2;
extern volatile u8 D_8009B0A3;
extern volatile u16 gInput_wPad1Held __attribute__((section(".data")));
extern DISPENV D_800FE0A8;

extern void func_800136D4(void);
extern void func_80012D4C(void);
extern void func_80014A5C(s32 a0);
extern void func_80041340(void);

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

/* Graphics and input start-up, called from Main_Init with the work area
 * in $a0. Resets the GPU, sets up a 320x240 display and the display
 * buffers, then initialises the frame flags and the six 0x8009B14x bytes
 * and the display-object slots at D_800FE048. The work area holds two
 * 0x5160-byte frame buffers; each gets its four ordering tables at
 * +0x5110 (lengths 2, 6, 0xC and 6 with their table bases at +0, +0x10,
 * +0x110 and +0x4110) cleared from the last to the first. The screen
 * block at D_800FE0A8 is copied to gGraphics_DispEnv, the GTE and the 3D
 * wrappers are initialised with a 300 projection, then the pads, the memory
 * card, the file position table and the random seed. */
void func_80013154(u8 *base)
{
    u8 *buf;
    s32 k;
    s32 off;
    s32 six;
    /* Retail copies the count into $v1 for the two byte stores. As a
       plain local the copy is folded away, since cse rates a narrowing
       subreg of the count cheaper than a fresh pseudo; only a hard
       register the user names is cheaper still. Set once, so it is
       launched right before its stores like the neighbouring constants. */
    register u8 count __asm__("$3");

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
    D_800FE048[0x18] = 1;
    D_800FE048[0x16] = 1;
    D_800FE048[0x19] = 1;
    D_800FE048[0x1A] = 1;
    D_800FE048[0x1B] = 1;
    count = six;
    D_8009B0A0 = 2;
    D_8009B0A1 = count;
    D_8009B0A2 = 0xC;
    D_8009B0A3 = count;
    /* A goto loop: a real loop would hoist the 2 and the end pointer into
       two more callee-saved registers. The counter reset is the first
       statement of the body so reorg can copy it into the back-branch
       delay slot; the entry then falls through it. */
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
        GsClearOt(0, k, buf + off);
        off -= 0x14;
        k--;
    } while (k >= 0);
    buf += 0x5160;
    /* Signed compare: retail uses slt, so the pointers compare as s32. */
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

void func_80013360(void)
{
    register s16 *p asm("$16");
    s32 step;

    p = (s16 *)&gGraphics_DispEnv;
    p[0] = 0;
    p[1] = 0;
    D_8009B098 |= 0x2000;
    goto poll;
adjust:
    if (gInput_wPad1Held & PAD_DIRECTION_MASK) {
        step = 2;
        if (gInput_wPad1Held & PAD_BUTTON_CROSS) {
            step = 4;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_RIGHT) {
            p[0] += step;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_LEFT) {
            p[0] -= step;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_UP) {
            p[1] -= step;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_DOWN) {
            p[1] += step;
        }
    }
    FntFlush(-1);
poll:
    func_80012D4C();
    if ((gInput_wPad1Pressed & PAD_BUTTON_START) == 0) {
        goto adjust;
    }
    D_8009B098 &= 0xDFFF;
    Input_ResetPads();
}

/* Zeroes D_800E9DB0[0..3] and D_8009B0B8. */
void func_800134B4(void) {
    void (**v0)(void);
    int v1;
    v1 = 3;
    v0 = &D_800E9DB0[v1];
    do {
        *v0 = 0;
        v1 -= 1;
        v0 -= 1;
    } while (v1 >= 0);
    D_8009B0B8 = 0;
}
