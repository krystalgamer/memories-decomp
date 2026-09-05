#include "../types.h"

extern u32 D_8009B10C;
extern volatile u32 D_8009B0F4;
extern volatile u16 D_8009B112;
extern u32 D_8009B134;
extern u8 D_800E9E18[];
extern u8 D_800E9E60[];

extern u8 *File_InitTransferDescriptor(
    u8 *, s32, u8 *, s32, s32, void *, s32, s32
);
void func_80015010(void);

u8 *func_80014EEC(s32 arg0, u8 *arg1, s32 arg2, s32 arg3, void *arg4, s32 arg5,
                  s32 arg6) {
    if (D_8009B10C == 0) {
        if (((D_8009B0F4 & 0x2000030) | D_8009B134) != 0) {
            return (u8 *)0;
        }
    } else {
        ((void (*)(void))D_8009B10C)();
    }
    File_InitTransferDescriptor(D_800E9E60, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    return D_800E9E60;
}

void func_80014FA4(void)
{
    s32 value;

    if ((D_8009B0F4 & 0x2000030) | D_8009B134) {
        value = 0x80;
        if ((D_8009B0F4 & 0x10) && (D_8009B0F4 & 0x80000)) {
            func_80015010();
        }
        D_8009B134 = value;
    }
}

void func_80015010(void)
{
    D_8009B112 &= 0x3FFC;
    D_8009B112 |= 2;
}

void func_80015038(void)
{
    if ((D_8009B0F4 & 0x10) && (D_8009B0F4 & 0x80000)) {
        func_80015010();
    }
}

u8 *func_80015078(s32 arg0, u8 *arg1, s32 arg2, s32 arg3, void *arg4,
                  s32 arg5, s32 arg6)
{
    u8 *state;

    D_8009B0F4 &= ~0x20;
    if ((D_8009B0F4 & 0x10) && (D_8009B0F4 & 0x80000)) {
        func_80015010();
    }

    state = D_800E9E18;
    File_InitTransferDescriptor(
        state, arg0, arg1, arg2, arg3, arg4, arg5, arg6
    );
    D_8009B0F4 |= 0x20;
    return state;
}
