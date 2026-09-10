#include "../types.h"
#include "text_encode_decimal_digits.h"
#include "text_constants.h"
#include "func_80038148.h"
#include "func_80036D70.h"

void func_80038148(u8 *p)
{
    u8 buf[8];
    u8 *e;
    u8 *bp;
    s32 r;
    s32 c;
    s32 t;
    s32 k;
    s32 i;
    s32 h;
    s32 w;

    r = func_80036D70(p);
    t = *(*(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)))++;
    c = t;
    Text_EncodeDecimalDigits(*(s32 *)r, c & 0xF, buf);

    h = 0;

    if ((c & 0x80) != 0) {
        if ((c & 0x40) == 0) {
            goto skip;
        }
        h = *(u16 *)&D_800EAFF8[0];
        e = p + 0x44;
        goto write;
    }

    if (c < 2) {
        e = p + 0x44;
        goto write;
    }

    bp = buf;
    k = c - 1;
    while (1) {
        if (bp[k] < TEXT_DECIMAL_RADIX) {
            break;
        }
        c = k;
        if (k < 2) {
            break;
        }
        k = c - 1;
    }

skip:
    e = p + 0x44;

write:
    i = (c & 0xF) - 1;
    do {
        w = h;
        if (buf[i] < TEXT_DECIMAL_RADIX) {
            w = *(u16 *)&D_800EAFF8[buf[i]];
        }
        if (w >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
            *e = (w >> 8) - 0x10;
            e[1] = w;
            e += 2;
        } else {
            *e = w;
            e += 1;
        }
        i--;
    } while (i >= 0);

    *e = TEXT_STRING_TERMINATOR;
    p[0x58] = p[0x58] + 1;
    *(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)) = p + 0x44;
}
