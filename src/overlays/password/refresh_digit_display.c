#include "../../types.h"

extern u8 gPassword_abDigits[];
extern u16 D_800EAFF8[];
extern u8 D_801B1245[];
extern u8 D_800EB0F8[];
extern void func_8003B6AC(s32, s32);
extern void func_80035BE4(s32, s32, s32, s32, s32, s32);
extern void func_80039A14(void *);

void Password_RefreshDigitDisplay(void)
{
    u8 *out;
    s32 i;
    s32 glyph;

    out = D_801B1245;
    for (i = 0; i < 8; i++) {
        glyph = D_800EAFF8[gPassword_abDigits[i]];
        if (glyph >= 0xF0) {
            out[0] = (glyph >> 8) | -0x10;
            out[1] = glyph;
            out += 2;
        } else {
            out[0] = glyph;
            out += 1;
        }
    }
    *out = 0xFF;
    func_8003B6AC(2, 1);
    func_80035BE4(2, 0xFD, 0xA8, 0x68, 0xA0, 0x10);
    D_800EB0F8[0x122] = 0x10;
    D_800EB0F8[0x123] = 0x10;
    func_80039A14(&D_800EB0F8[0xC8]);
}
