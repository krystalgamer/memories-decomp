/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/main_services.c.
 */
#define D_8009B0A3_IS_VOLATILE_SCALAR
#define D_8009B142_IN_DATA_VOLATILE
#define GRAPHICS_DRAW_ENV_IS_VOLATILE
/* func_80013360 re-reads the pad word on each path and needs it out of
   small data; see the arms in input.h. */
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
/* The fade target colour is part of the same init block; see the arm in
   fade.h. */
#define D_8009B14A_IN_DATA_VOLATILE
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libmcrd.h"
#include "../psyq/rand.h"
#include "../game/fade.h"
#include "../unmatched.h"
#include "../game/func_800136D4.h"
#include "../game/func_80041340.h"
#include "../game/graphics_constants.h"
#include "../game/graphics_frame.h"
#include "../game/main_frame.h"
#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#include "../game/input.h"
#include "../game/rand_constants.h"
#include "../game/main_services.h"

/* The resident system layer: the per-frame service pump, the boot-time
   graphics and input start-up that installs it, the pad-driven screen-offset
   adjustment loop, and the reset of the callback registry the pump walks.
   The four are contiguous and are the only run in the region built with
   gcc_2_8_1_g8_split - their neighbours on both sides use other profiles -
   and the pump and the reset share the D_800E9DB0 slots and D_8009B0B8. */

/* The init block is a run of byte stores to distinct globals; its volatile
   views (including the shared header arms) keep the emitted order the source
   order (see Main_Init). That is measured, not assumed, and it does not
   extend to the whole run: the first three stores (D_8009B0AD, D_8009B0D0,
   D_8009B0A8) build byte-identical without it and now take plain
   declarations from graphics_frame.h, the unit that defines them. The
   six D_8009B14x bytes are addressed %hi/%lo in retail, so they sit outside
   small data. */
extern volatile u8 D_8009B0A0;
extern volatile u8 D_8009B0A1;
extern volatile u8 D_8009B0A2;

/* Graphics and input start-up, called from Main_Init with the work area
 * in $a0. Resets the GPU, sets up a 320x240 display and the display
 * buffers, then initialises the frame flags and the six 0x8009B14x bytes
 * and the drawing environment at D_800FE048. The work area holds two
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
        GsClearOt(0, k, (GsOT *)(buf + off));
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

