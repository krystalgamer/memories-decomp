#define D_8009B318_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libetc.h"
#define D_8009B142_IN_DATA
#define D_8009B0C0_IS_VOLATILE
#include "graphics_frame.h"
#include "ordering_tables.h"
#define MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE
#include "model_graphics_state.h"
#include "movie_playback_control.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the store below. c_symbols.ld overrides this
   common symbol, so no storage is allocated here. */
u8 D_8009B0C1;

extern void func_800862C0(void *);

/* gp-relative in the target, so this unit defines them */
u8 D_8009B0A8;
u8 D_8009B0AD;
u8 D_8009B0D0;
u16 D_8009B098;
u8 D_8009B0A0[4];
u8 gGraphics_bActiveBuffer;
u8 *D_8009B0B4;
s16 gGraphics_sViewportX __attribute__((section(".sbss"))) = 0;
s16 gGraphics_sViewportY __attribute__((section(".sbss"))) = 0;

extern u8 D_8009B141 __attribute__((section(".data")));

/* Waits for the current GPU/VBlank boundary and publishes the bounded number
   of frame advances consumed by the next game update. */
void Graphics_SyncFrame(void)
{
    if ((D_8009B098 & 0x8000) == 0) {
        DrawSync(0);
    }
    while (D_8009B0C8 < D_8009B0C0) {
    }

    D_8009B0C1 = D_8009B0C8;
    if (D_8009B0C1 & 0xFF) {
        D_8009B0C1 = 1;
    }
    D_8009B0D8 = D_8009B0C1 + 1;

    D_8009AFA3 = D_8009AFA4 ? 2 : D_8009B0D8;

    D_8009AFA4 = 0;
    D_8009B0C8 = -1;

    VSync(0);

    D_8009B0CC++;
}

void Graphics_BeginFrame(void)
{
    s32 i;
    s32 off;
    GsOT **slot;
    s32 src;
    s32 idx;
    GsOT *ptr;
    u8 *arg;
    GsOT **base;

    if (D_8009B0A8 == 0) {
        D_800FE048[0].isbg = D_8009B0D0;
        D_800FE048[0].dtd = D_8009B0AD;
        D_800FE048[0].r0 = D_8009B144;
        D_800FE048[0].g0 = D_8009B143;
        D_800FE048[0].b0 = D_8009B142;
        GsSwapDispBuff();
        if ((D_8009B098 & 0x2000) != 0) {
            PutDispEnv(&gGraphics_DispEnv);
        }
    }
    ResetGraph(1);
    if ((D_8009B318 & 0x80) != 0) {
        func_800359B0();
    }
    if ((D_8009B318 & 0x80) == 0) {
        if (D_8009B141 != 0) {
            GsSortOt((GsOT *)(D_8009B0B4 + 0x5124),
                     (GsOT *)(D_8009B0B4 + 0x5110));
            if ((D_8009B141 & 0x80) == 0) {
                GsSortOt((GsOT *)(D_8009B0B4 + 0x5138),
                         (GsOT *)(D_8009B0B4 + 0x5110));
                GsSortOt((GsOT *)(D_8009B0B4 + 0x514C),
                         (GsOT *)(D_8009B0B4 + 0x5110));
            }
            GsDrawOt((GsOT *)(D_8009B0B4 + 0x5110));
        }
    }
    idx = GsGetActiveBuff();
    src = (s32)D_8009B0A0;
    i = 3;
    gGraphics_bActiveBuffer = idx;
    arg = &D_800A5768[gGraphics_bActiveBuffer * 140000];
    D_8009AFA2 = idx;
    D_8009B0B4 = &D_8009B4A8[gGraphics_bActiveBuffer * 20832];
    func_800862C0(arg);
    off = 0x514C;
    base = D_800E9D90;
    slot = base + 3;
    do {
        ptr = (GsOT *)(D_8009B0B4 + off);
        *slot = ptr;
        slot--;
        ptr->length = *(u8 *)(i + src);
        GsClearOt(0, 0, ptr);
        off -= 0x14;
        i--;
    } while (i >= 0);
}
