/*
 * Model slot channel decoder. Reclassified from matching_c (#3859), where the
 * source was byte-exact under gcc_2_8_1_g0 only by pinning 14 variables to
 * hard registers. Current best with no hard register pin: 277/278
 * instructions and opcode distance 1 (one missing slt).
 *
 * The selector is read through kind, the argument the pinned source took from
 * $a1 through an unassigned pinned name. Every record field is addressed from
 * rec, which drops the separate maxp cursor and its extra saved register, and
 * the setup from `n = count;` through the add1/add2 constants sits in one
 * do { } while (0) block. A volatile signed halfword read still preserves the
 * distinct 0x20D switch tail.
 *
 * Residual: retail copies mode into $t7 in the prologue, and keeps the 0x20D
 * arm's own slt in its jump's delay slot where this build shares the test.
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
    s32 mode_reg;
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
    u32 adjusted;
    s32 x;
    s32 y;
    u32 k;
    s32 test;
    u16 *selector;

    mode_reg = mode;
    selector = kind;
    hdr = *(u8 **)ctx;
    rec = *(u8 **)(ctx + 0x14);
    count = *(u16 *)(hdr + 2);
    rec = rec + *(s32 *)(hdr + 4) * 4;
    do {
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
    } while (0);
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
                    x = y;
                    *best = y;
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
                    x = y;
                    *best = y;
                }
                y = *(volatile s16 *)(rec + 0x18) & 0xFFFF;
                test = x < y;
                goto apply_test;
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
                    x = y;
                    *best = y;
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
                    x = y;
                    *best = y;
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
            test = x < y;
apply_test:
            if (test) {
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

