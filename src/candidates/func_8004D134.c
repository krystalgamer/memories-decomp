/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 14 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/model_slot_row_tables.c.
 */
#include "../types.h"
#include "../game/model.h"
#include "../game/func_8004D914.h"
#include "../game/model_slot_row_tables.h"

/* One model slot's animation channel and row-table helpers: the channel
   decoder, the reset that imports the command list, and the walk that
   consumes it. The three form the complete gcc_2_8_1_g0 run below
   func_8004D914.

   The model loader resets the tables before sending each event through the
   decoder; slot setup later walks the imported list. The reset fills keys
   with 0xFFFF, zeroes rows and maxima, and stores the command list at 0xDD8;
   the walk claims those keys, accumulates rows, and reads the same list. */

s32 func_8004D134(s32 mode, u16 *kind, u8 *ctx, s32 *best, s32 *total)
{
    /* File scope perturbs func_8004D58C; local volatile adds one instruction. */
    register s32 mode_reg asm("$15");
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
    test = mode_reg << 2;
    add1 = test - 0xA;
    test = mode_reg << 4;
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

