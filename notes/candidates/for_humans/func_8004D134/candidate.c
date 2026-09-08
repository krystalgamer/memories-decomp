#include "../../../../src/types.h"

/* Advances one animation channel across every record of a track and reports
   the record count. The u16 at the second argument selects the record layout:
   its low nibble picks the pair of halfword fields the channel steps, and the
   0x200 bit selects the wider of the two field pairs. For mode 0 and 1 the
   first field is stepped by mode * 4 - 10 and cleared of bit 0x80 once its
   two-bit tag reaches 3, and the second is stepped by mode * 16 + 0x3BD8 and
   rebuilt from a six-bit remainder plus a four-bit quotient once its own tag
   reaches 0x10. When a maximum pointer is supplied, the same selector picks
   between one and four halfwords per record and the largest is kept there. */
s32 func_8004D134(s32 mode, u16 *kind, u8 *ctx, s32 *best, s32 *total)
{
    u8 *hdr;
    u8 *rec;
    u8 *p;
    u8 *q;
    s32 count;
    s32 n;
    s32 off1;
    s32 off2;
    s32 stride;
    s32 step;
    s32 add1;
    s32 add2;
    s32 f;
    s32 g;
    u32 v;
    s32 x;
    s32 y;
    u32 k;

    hdr = *(u8 **)ctx;
    rec = *(u8 **)(ctx + 0x14);
    count = *(u16 *)(hdr + 2);
    rec = rec + *(s32 *)(hdr + 4) * 4;
    if ((u32)mode >= 2) {
        return 0;
    }
    n = count;
    k = *kind;
    switch (k) {
    case 9:
        off1 = 6;
        off2 = 2;
        stride = 0x14;
        step = 0x20;
        break;
    case 0x209:
        off1 = 0xA;
        off2 = 6;
        stride = 0x18;
        step = 0x20;
        break;
    case 0xD:
    case 0x11:
        off1 = 6;
        off2 = 2;
        stride = 0x18;
        step = 0x28;
        break;
    case 0x20D:
    case 0x211:
        off1 = 0xA;
        off2 = 6;
        stride = 0x1C;
        step = 0x28;
        break;
    case 0x15:
        off1 = 6;
        off2 = 2;
        stride = 0x1C;
        step = 0x34;
        break;
    case 0x215:
        off1 = 0xA;
        off2 = 6;
        stride = 0x20;
        step = 0x34;
        break;
    default:
        return 0;
    }

    n--;
    if (n == -1) {
        return count;
    }
    add1 = mode * 4 - 0xA;
    add2 = mode * 0x10 + 0x3BD8;
    do {
        if (mode < 2) {
            p = rec + off1;
            v = *(u16 *)p;
            *(u16 *)p = v + add1;
            f = (v >> 7) & 3;
            if (f >= 3) {
                *(u16 *)p = (v + add1) & 0xFF7F;
            }
            if (f < 2) {
                q = rec + off2;
                v = *(u16 *)q;
                *(u16 *)q = v + add2;
                g = v >> 6;
                if (g >= 0x10) {
                    y = ((v + add2) & 0x3F) + 0x10;
                    *(u16 *)q = y;
                    *(u16 *)q = y | ((g % 0x10) << 6);
                }
            }
        }
        if (best != 0) {
            k = *kind;
            switch (k) {
            case 9:
                x = *best;
                y = *(u16 *)(rec + 0xC);
                break;
            case 0x209:
                x = *best;
                y = *(u16 *)(rec + 0x10);
                break;
            case 0xD:
                x = *best;
                y = *(u16 *)(rec + 0xC);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(rec + 0x10);
                if (x < y) {
                    *best = y;
                    x = y;
                }
                y = *(u16 *)(rec + 0x14);
                break;
            case 0x20D:
                x = *best;
                y = *(u16 *)(rec + 0x10);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(rec + 0x14);
                if (x < y) {
                    *best = y;
                    x = y;
                }
                y = *(u16 *)(rec + 0x18);
                break;
            case 0x11:
                x = *best;
                y = *(u16 *)(rec + 0xE);
                break;
            case 0x211:
                x = *best;
                y = *(u16 *)(rec + 0x12);
                break;
            case 0x15:
                x = *best;
                y = *(u16 *)(rec + 0xA);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(rec + 0x10);
                if (x < y) {
                    *best = y;
                    x = y;
                }
                y = *(u16 *)(rec + 0x14);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(rec + 0x18);
                break;
            case 0x215:
                x = *best;
                y = *(u16 *)(rec + 0xE);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(rec + 0x14);
                if (x < y) {
                    *best = y;
                    x = y;
                }
                y = *(u16 *)(rec + 0x18);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(rec + 0x1C);
                break;
            default:
                goto next;
            }
            if (x < y) {
                x = y;
            }
            *best = x;
        }
    next:
        rec = rec + stride;
        *total = *total + step;
    } while (--n != -1);
    return count;
}
