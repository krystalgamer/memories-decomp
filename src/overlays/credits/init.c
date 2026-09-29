#include "../../types.h"
#include "credits.h"

void func_801807B0(void)
{
    RECT rect0;
    RECT rect1;
    RECT rect2;
    u16 clut0[16];
    u16 clut1[16];
    u16 clut2[16];
    s32 i;

    rect0 = D_80180784;
    rect1 = D_8018078C;
    rect2 = D_80180794;
    clut0[0] = 0;
    clut0[1] = 0x8421;
    clut0[2] = 0x9084;
    clut0[3] = 0x9CE7;
    clut0[4] = 0xA529;
    clut0[5] = 0xAD6B;
    clut0[6] = 0xB5AD;
    clut0[7] = 0xBDEF;
    clut0[8] = 0xC631;
    clut0[9] = 0xCE73;
    clut0[10] = 0xD6B5;
    clut0[11] = 0xDEF7;
    clut0[12] = 0xE739;
    clut0[13] = 0xEF7B;
    clut0[14] = 0xF7BD;
    clut0[15] = 0xFFFF;
    clut1[0] = 0;
    clut1[1] = 0x8421;
    clut1[2] = 0x8844;
    clut1[3] = 0x8C67;
    clut1[4] = 0x9089;
    clut1[5] = 0x94AB;
    clut1[6] = 0x98CD;
    clut1[7] = 0x9CEF;
    clut1[8] = 0xA111;
    clut1[9] = 0xA533;
    clut1[10] = 0xA955;
    clut1[11] = 0xAD77;
    clut1[12] = 0xB199;
    clut1[13] = 0xB5BB;
    clut1[14] = 0xB9DD;
    clut1[15] = 0xBDFF;
    clut2[0] = 0;
    clut2[1] = 0x8421;
    clut2[2] = 0x8882;
    clut2[3] = 0x8CE3;
    clut2[4] = 0x9124;
    clut2[5] = 0x9565;
    clut2[6] = 0x99A6;
    clut2[7] = 0x9DE7;
    clut2[8] = 0xA228;
    clut2[9] = 0xA669;
    clut2[10] = 0xAAAA;
    clut2[11] = 0xAEEB;
    clut2[12] = 0xB32C;
    clut2[13] = 0xB76D;
    clut2[14] = 0xBBAE;
    clut2[15] = 0xBFEF;
    while (IsIdleGPU(3) != 0) {
    }
    while (LoadImage2(&rect0, (u32 *)clut0) != 0) {
    }
    while (LoadImage2(&rect1, (u32 *)clut1) != 0) {
    }
    while (LoadImage2(&rect2, (u32 *)clut2) != 0) {
    }
    while (IsIdleGPU(3) != 0) {
    }
    for (i = 1; i >= 0; i--) {
        D_8018220C[i].entries = 0;
    }
    D_80182208 = 0;
}
