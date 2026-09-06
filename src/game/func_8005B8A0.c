#include "../types.h"
#include "../psyq/libcd.h"

extern u8 *D_80010000 __attribute__((section(".data")));
extern u8 D_8009B060;
extern u8 D_8009B061;
extern u8 D_8009B063;
extern u8 D_8009B064;
extern u8 D_8009B065;
extern u8 D_8009B066;
extern u8 D_8009B067;
extern u32 D_8009B068;
extern u32 D_8009B06C;
extern u32 D_8009B070;
extern u8 D_8009B142 __attribute__((section(".data")));
extern u8 D_8009B143 __attribute__((section(".data")));
extern u8 D_8009B144 __attribute__((section(".data")));
extern u8 *D_8009B498;
extern u8 D_8009B49C;
extern s16 D_800FE0CC __attribute__((section(".data")));
extern u16 D_800FE0D0 __attribute__((section(".data")));
extern s32 D_800FE0D4 __attribute__((section(".data")));

void func_8005C1F4(void);
void func_8005C690(void);

typedef struct {
    u8 b[4];
} Bytes4;

s32 func_8005B8A0(u8 *src, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    u16 rect[4];
    s32 m;
    s32 r;

    D_8009B063 = 0;
    D_8009B064 = 0;
    D_8009B065 = 0xFF;
    D_8009B070 = a3;
    D_8009B060 = a4;
    D_8009B498 = D_80010000;
    D_8009B061 = a5;
    if (D_8009B060 != 0) {
        m = func_80085320(D_80010000, a5);
        rect[0] = m * 0x140;
        rect[1] = 0;
        rect[2] = D_800FE0D0;
        rect[3] = D_800FE0D4;
        ClearImage(rect, D_8009B144, D_8009B143, D_8009B142);
        DrawSync(0);
        VSync(0);
        func_80085500();
        rect[0] = (m ^ 1) * 0x140;
        rect[2] = D_800FE0D0;
        rect[1] = 0;
        rect[3] = D_800FE0D4;
        ClearImage(rect, D_8009B144, D_8009B143, D_8009B142);
        rect[0] = 0;
        rect[1] = 0x100;
        rect[2] = *(s32 *)&D_800FE0D0 * 0x1800 / 4096;
        rect[3] = D_800FE0D4;
        ClearImage(rect, D_8009B144, D_8009B143, D_8009B142);
        DrawSync(0);
        VSync(0);
        D_800FE0CC = 1;
        func_80085500();
        func_800856A0(0, 0, 0, 0x100);
        GsInitGraph2(0x140, 0xF0, 4, 1, 1);
    }
    if (src != (u8 *)0) {
        *(Bytes4 *)&D_8009B49C = *(Bytes4 *)src;
    }
    if (a1 != 0) {
        D_8009B068 = a1;
    } else {
        D_8009B068 = 1;
    }
    if (a2 != 0) {
        D_8009B06C = a2;
    } else {
        D_8009B06C = 0xFFFF;
    }
    D_8009B066 = 0;
    D_8009B067 = 0;
    DecDCTReset(0);
    DecDCToutCallback(func_8005C1F4);
    DecDCTvlcBuild(D_8009B498);
    StSetRing((u32 *)(D_8009B498 + 0x11000), 0x14);
    StClearRing();
    StSetStream(D_8009B060, D_8009B068, D_8009B06C, 0, func_8005C690);
    if (D_8009B06C >= 5) {
        D_8009B06C = D_8009B06C - 4;
    }
    func_8005C62C(&D_8009B49C);
    r = func_8005BFC8(0);
    if (r != 0) {
        return r;
    }
    return 0;
}
