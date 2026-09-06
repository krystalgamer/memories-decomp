#include "../../types.h"

extern s8 D_8016D42C;
extern u8 *D_8016D43C;
extern void NameEntry_UpdateCaretTween(void);
extern void SD_SEPlayFull(s32);

s32 NameEntry_AdjustLength(s32 delta, s32 arg)
{
    u8 *object;

    if (delta < 0) {
        if (D_8016D42C == 0) {
            return 0;
        }
    } else {
        if (D_8016D42C == 5) {
            return 0;
        }
    }
    D_8016D42C += delta;
    object = D_8016D43C;
    *(s16 *)(object + 0x44) = D_8016D42C * 16 + 0x6B;
    *(void **)(object + 0x24) = NameEntry_UpdateCaretTween;
    object[0x6C] = 2;
    *(s16 *)(object + 0x60) = arg;
    *(s16 *)(object + 0x46) = *(u16 *)(object + 0x32);
    SD_SEPlayFull(0xC);
    return 1;
}
