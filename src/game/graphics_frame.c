#define D_8009B318_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libetc.h"
#define D_8009B142_IN_DATA
#define D_8009B0C0_IS_VOLATILE
#include "graphics_frame.h"
#include "movie_playback_control.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the store below. c_symbols.ld overrides this
   common symbol, so no storage is allocated here. */
u8 D_8009B0C1;
extern unsigned char D_8009AFA3 __attribute__((section(".data")));
extern unsigned char D_8009AFA4 __attribute__((section(".data")));

extern void func_80085500(void);
extern void func_80085E10(void *, void *);
extern void func_80085D80(void *);
extern s32 func_80085320(void);
extern void func_800862C0(void *);
extern void func_80085DB0(s32, s32, u32 *);

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

extern u8 D_800FE048[];
extern u8 D_8009B141 __attribute__((section(".data")));
extern u8 D_8009AFA2 __attribute__((section(".data")));
extern u32 *D_800E9D90[4];

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
    u32 **slot;
    s32 src;
    s32 idx;
    u32 *ptr;
    u8 *arg;
    register u32 **base asm("$2");

    if (D_8009B0A8 == 0) {
        D_800FE048[0x18] = D_8009B0D0;
        D_800FE048[0x16] = D_8009B0AD;
        D_800FE048[0x19] = D_8009B144;
        D_800FE048[0x1A] = D_8009B143;
        D_800FE048[0x1B] = D_8009B142;
        func_80085500();
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
            func_80085E10(D_8009B0B4 + 0x5124, D_8009B0B4 + 0x5110);
            if ((D_8009B141 & 0x80) == 0) {
                func_80085E10(D_8009B0B4 + 0x5138, D_8009B0B4 + 0x5110);
                func_80085E10(D_8009B0B4 + 0x514C, D_8009B0B4 + 0x5110);
            }
            func_80085D80(D_8009B0B4 + 0x5110);
        }
    }
    idx = func_80085320();
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
        ptr = (u32 *)(D_8009B0B4 + off);
        *slot = ptr;
        slot--;
        *ptr = *(u8 *)(i + src);
        func_80085DB0(0, 0, ptr);
        off -= 0x14;
        i--;
    } while (i >= 0);
}
