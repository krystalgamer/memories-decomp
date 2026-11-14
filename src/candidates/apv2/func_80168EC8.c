#include "../../types.h"
#include "apv2.h"
#include "../../psyq/libapi.h"
#include "../../psyq/strings.h"

extern s32 D_801694D0;

u32 func_801691F0(u8 *);
void func_8016903C(Apv2Rect *, u8 *, s32, u32);

void func_80168EC8(u8 *str, Apv2Rect *rect, s32 color)
{
    s32 len;
    s32 x;
    u8 *start;
    u8 *glyph;
    u32 range;
    s32 hi;
    u8 c;

    len = strlen(str);
    start = str;
    x = rect->x;
    for (; str < start + len; str++) {
        c = *str;
        if (c == '\n') {
            rect->x = x;
            rect->y += 18;
        } else if (D_801694D0 == 0) {
            hi = *str++ << 8;
            glyph = (u8 *)Krom2RawAdd(*str | hi);
            func_8016903C(rect, glyph, color, 7);
            rect->x += rect->w + 1;
        } else if (c == ' ') {
            rect->x += 8;
        } else {
            glyph = (u8 *)0xBFC7F8DE + (*str - 0x21) * 15;
            range = func_801691F0(glyph);
            rect->w = range - (range >> 16) + 1;
            func_8016903C(rect, glyph, color, range);
            rect->x += rect->w + 2;
        }
    }
}
