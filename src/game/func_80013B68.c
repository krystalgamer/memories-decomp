#include "../types.h"

struct FileState {
    char pad00[0x24];
    s32 f24;
    char pad28[0x2C - 0x28];
    s32 f2C;
    char pad30[0x34 - 0x30];
    s32 f34;
    u8 f38;
    u8 f39;
    char pad3A[0x46 - 0x3A];
    u8 f46;
};

extern volatile u32 D_8009B0F4;
extern struct FileState D_800E9E18;
extern void func_80015010(void);

struct FileState *func_80013B68(s32 a, s32 b, s32 c, s32 d)
{
    struct FileState *p;

    D_8009B0F4 &= ~0x20;
    p = &D_800E9E18;
    if (D_8009B0F4 & 0x10) {
        if (D_8009B0F4 & 0x80000) {
            func_80015010();
        }
    }
    p->f24 = a;
    p->f34 = b;
    p->f38 = (u8)c;
    p->f39 = (u8)d;
    p->f46 = 4;
    p->f2C = 0x80000;
    D_8009B0F4 |= 0x20;
    return p;
}
