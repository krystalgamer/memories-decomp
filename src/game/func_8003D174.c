#include "../types.h"

extern u32 D_8009AF64;
extern u32 D_8009AF68;

extern s32 func_8003CEB8(u8 *, s32);
extern u32 SaveData_NextMaskWord(void);

s32 func_8003D174(u8 *data)
{
    s32 seed;
    s32 i;
    s32 *word;

    seed = func_8003CEB8(data, 0x340) & 0xFFFF;
    word = (s32 *)(data + 0x378);
    i = 0xF;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    seed = func_8003CEB8(data + 0x380, 0x6C) & 0xFFFF;
    word = (s32 *)(data + 0x3F8);
    i = 4;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    seed = func_8003CEB8(data + 0x400, 0x204) & 0xFFFF;
    word = (s32 *)(data + 0x624);
    i = 8;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    return 1;
}
