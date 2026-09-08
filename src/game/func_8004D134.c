#include "../types.h"

register volatile s32 mode_reg asm("$15");

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
    register u8 *hdr asm("$4");
    register u8 *rec asm("$3");
    u8 *p;
    u8 *q;
    register u8 *maxp asm("$9");
    register s32 count asm("$17");
    register s32 n asm("$14");
    s32 off1;
    s32 off2;
    register s32 stride asm("$10");
    s32 step;
    register s32 add1 asm("$5");
    s32 add2;
    s32 f;
    s32 g;
    u32 v;
    u32 adjusted;
    register s32 x asm("$4");
    register s32 y asm("$6");
    register u32 k asm("$4");
    register s32 test asm("$2");
    register u16 *selector asm("$16");
    /* Keep the saved selector distinct from the still-live incoming $a1. */
    register u16 *selector_source asm("$5");

    mode_reg = mode;
    selector = selector_source;
    hdr = *(u8 **)ctx;
    rec = *(u8 **)(ctx + 0x14);
    count = *(u16 *)(hdr + 2);
    rec = rec + *(s32 *)(hdr + 4) * 4;
    n = count;
    if ((u32)mode_reg >= 2) {
        return 0;
    }
    k = *selector;
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
    test = mode_reg * 4;
    add1 = test - 0xA;
    test = mode_reg * 0x10;
    add2 = test + 0x3BD8;
    maxp = rec + 0x1C;
    do {
        if (mode_reg < 2) {
            p = rec + off1;
            v = *(u16 *)p;
            adjusted = v;
            adjusted += add1;
            *(u16 *)p = adjusted;
            f = (v >> 7) & 3;
            if (f >= 3) {
                *(u16 *)p = adjusted & 0xFF7F;
            }
            if (f < 2) {
                q = rec + off2;
                v = *(u16 *)q;
                *(u16 *)q = v + add2;
                g = v >> 6;
                if (g >= 0x10) {
                    v = (v + add2) & 0x3F;
                    y = v + 0x10;
                    *(u16 *)q = y;
                    v = y | ((g % 0x10) << 6);
                    *(u16 *)q = v;
                }
            }
        }
        if (best != 0) {
            k = *selector;
            switch (k) {
            case 9:
                x = *best;
                y = *(u16 *)(maxp - 0x10);
                break;
            case 0x209:
                x = *best;
                y = *(u16 *)(maxp - 0xC);
                break;
            case 0xD:
                x = *best;
                y = *(u16 *)(maxp - 0x10);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(maxp - 0xC);
                if (x < y) {
                    x = y;
                    *best = y;
                }
                y = *(u16 *)(maxp - 8);
                break;
            case 0x20D:
                x = *best;
                y = *(u16 *)(maxp - 0xC);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(maxp - 8);
                if (x < y) {
                    x = y;
                    *best = y;
                }
                y = *(volatile s16 *)(maxp - 4) & 0xFFFF;
                test = x < y;
                goto apply_test;
            case 0x11:
                x = *best;
                y = *(u16 *)(maxp - 0xE);
                break;
            case 0x211:
                x = *best;
                y = *(u16 *)(maxp - 0xA);
                break;
            case 0x15:
                x = *best;
                y = *(u16 *)(maxp - 0x12);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(maxp - 0xC);
                if (x < y) {
                    x = y;
                    *best = y;
                }
                y = *(u16 *)(maxp - 8);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(maxp - 4);
                break;
            case 0x215:
                x = *best;
                y = *(u16 *)(maxp - 0xE);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)(maxp - 8);
                if (x < y) {
                    x = y;
                    *best = y;
                }
                y = *(u16 *)(maxp - 4);
                if (x < y) {
                    x = y;
                }
                *best = x;
                y = *(u16 *)maxp;
                break;
            default:
                goto next;
            }
            test = x < y;
apply_test:
            if (test) {
                x = y;
            }
            *best = x;
        }
next:
        maxp = maxp + stride;
        rec = rec + stride;
        *total = *total + step;
    } while (--n != -1);
    return count;
}
