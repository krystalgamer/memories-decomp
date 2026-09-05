#include "../types.h"

struct FileState {
    char pad24[0x24];
    s32 f24;
    char pad46[0x46 - 0x28];
    u8 f46;
};

extern volatile u32 D_8009B0F4;
extern volatile u32 D_8009B134;
extern struct FileState D_800E9E60;
extern s32 gFile_anLba[];

struct FileState *func_80013B04(s32 index, s32 offset)
{
    if (((D_8009B0F4 & 0x02000030) | D_8009B134) == 0) {
        D_800E9E60.f24 = gFile_anLba[index] + offset;
        D_800E9E60.f46 = 0;
        D_8009B0F4 = 0x100010;
        return &D_800E9E60;
    }
    return 0;
}
