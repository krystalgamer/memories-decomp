#include "../types.h"

extern u8 D_8009B3ED;
extern u8 D_8009B3C0;
extern u8 D_8009B3EA;
extern u8 D_801D1200[];
extern s32 D_801D5608[];
extern u8 D_801D160C[];
extern u8 D_801B122B[];
extern u8 D_801B1238[];
extern s32 func_8003FCD8(void);
extern void Text_SjisToGlyphCodes();

s32 func_8003FD14(void)
{
    s32 result;
    s32 i;
    u8 *left;
    u8 *right;
    s32 *state;

    if ((D_8009B3ED & 128) == 0) {
        D_8009B3ED |= 128;
        D_8009B3C0 = 40;
    }
    result = func_8003FCD8();
    if (result == 1) {
        left = D_801D1200;
        right = D_801D1200 + 4096;
        i = 0;
        state = D_801D5608;
        D_8009B3EA = 10;
        D_8009B3C0 = 36;
        for (; i < 40; i++, left += 2, right += 2) {
            if (*(u16 *)left == 0) {
                state[16] = 1;
                return 0;
            }
            if (*(u16 *)right == 0) {
                state[16] = 2;
                return 0;
            }
        }
        Text_SjisToGlyphCodes(D_801B122B, D_801D160C, 6);
        Text_SjisToGlyphCodes(D_801B1238, D_801D160C + 4096, 6);
    }
    return result;
}
