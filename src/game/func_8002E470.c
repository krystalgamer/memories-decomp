#include "../types.h"

extern u8 *D_8009B290;
extern u16 D_8009B270;
extern u16 D_8009B27C;
extern u32 D_8009B0F4 __attribute__((section(".data")));
extern s32 D_8009B134 __attribute__((section(".data")));
extern u8 D_800E9ECE[];
extern u8 D_800EAE98[];
extern s16 gGraphics_sViewportX __attribute__((section(".data")));
extern s16 gGraphics_sViewportY __attribute__((section(".data")));

extern s32 func_8002E3B4(void);
extern void func_8002E00C(void *);
extern void func_8002DF2C(void *, s32);
extern void func_8002E128(void *, s32);
extern void func_800157DC(void);

void func_8002E470(void)
{
    u8 *script;
    u8 *next;
    u8 *next2;
    s32 value;
    u16 flags;

    if (func_8002E3B4() == 0) {
        gGraphics_sViewportY = 0;
        gGraphics_sViewportX = 0;
        func_8002E00C(D_800EAE98);
        script = D_8009B290;
        next = script + 2;
        D_8009B290 = next;
        value = script[0] | (script[1] << 8);
        D_8009B270 = value;
        if ((value & 0x8000) != 0) {
            D_8009B270 = value & 0xFFF;
            next2 = script + 4;
            D_8009B290 = next2;
            gGraphics_sViewportX = script[2] | (next[1] << 8);
            D_8009B290 = script + 6;
            gGraphics_sViewportY = script[4] | (next2[1] << 8);
        }
        func_8002DF2C(D_800EAE98, D_8009B270);
    }
    flags = D_8009B27C;
    if ((flags & 0x800) == 0) {
        if (((D_8009B0F4 & 0x02000030) | D_8009B134) == 0) {
            D_8009B27C = flags | 0x800;
            func_8002E128(D_800EAE98, -1);
            func_800157DC();
        }
    } else {
        if ((D_800E9ECE[0] & 0x80) == 0) {
            D_8009B27C = 0;
        }
    }
}
