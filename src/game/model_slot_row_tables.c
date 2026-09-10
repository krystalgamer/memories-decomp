#include "../types.h"
#include "model.h"
#include "func_8004D914.h"
#include "model_slot_row_tables.h"

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

void func_8004D58C(s32 arg0, u8 *arg1)
{
    ModelSlot *ch;
    u8 *t;
    u8 *c;
    u8 *q;
    u8 *e;
    register u8 *p3 asm("$11");
    u8 *p2;
    u8 *g;
    u8 *k;
    u8 *s;
    u8 *u;
    u8 *v;
    s32 ff;
    s32 one;
    s32 i;
    s32 j;
    s32 n;
    s32 m;
    s32 a;
    s32 b;
    s32 w;
    s32 d;
    s32 x;
    s32 y;
    register s32 flag asm("$3");

    p3 = (u8 *)0;
    p2 = (u8 *)0;
    i = 0;
    ff = 0xFFFF;
    n = 0;
    m = 0;
    ch = &D_800F2C40[arg0];
    t = (u8 *)ch;
    c = t;
    ch->field_E06 = 0;
    ch->field_E08 = 0;
    ch->field_DD8 = 0;
    *(s32 *)(t + 0xDDC) = 0;
    *(s32 *)(t + 0xDE0) = 0;
    *(s32 *)(t + 0xDE4) = 0;
    ch->field_DF0 = 0;
    /* The reset walks keys and rows with running byte offsets rather than
       indexing ch->field_2C8[i][j] / ch->field_750[i].values[j]. That is not
       a missed cleanup: the indexed form is larger, and the link fails with
       .initialized_data overlapping .text. The walkers are load-bearing.

       The margin is one instruction, which is worth knowing before touching
       anything else here. The seven writes below that still go through `t`
       reach members this record declares as pointers, so naming them costs a
       *(s32 *)& cast; measured, that is the single instruction that pushes
       .text into .initialized_data again. The three named above are free
       because they land on plain members. */
    do {
        ((ModelSlotRow *)(c + 0x750))->max = 0;
        j = 0;
        a = n;
        b = m;
        do {
            u = t + a;
            a += 2;
            v = t + b;
            b += 2;
            j++;
            *(u16 *)(v + 0x2C8) = ff;
            *(s16 *)(u + 0x750) = 0;
        } while (j < 0x3A);
        n += 0x76;
        m += 0x74;
        i++;
        c += 0x76;
    } while (i < 0xA);
    i = 7;
    q = t + i;
    do {
        q[0xBEC] = 0;
        i--;
        q--;
    } while (i >= 0);
    e = *(u8 **)(arg1 + 0x10);
    if (e == (u8 *)0) {
        return;
    }
    do {
        if (*(s32 *)(e + 8) != 0) {
            w = e[0xF];
            if (w == 3) {
                p3 = *(u8 **)(e + 4);
            }
            if (w == 2) {
                p2 = *(u8 **)(e + 4);
            }
        }
        e = *(u8 **)e;
    } while (e != (u8 *)-1);
    if (p3 != (u8 *)0) {
        p3 += 8;
        k = *(u8 **)p3;
        p3 += 4;
        i = 0;
        if (*(u16 *)k != 0) {
            one = 1;
            g = k;
            do {
                d = i / 8;
                s = t + d;
                d <<= 3;
                y = s[0xBEC];
                flag = *(volatile s32 *)(g + 4) & 0x100;
                if (flag != 0) {
                    x = y | (one << (i - d));
                } else {
                    x = y;
                }
                s[0xBEC] = x;
                g += 4;
                i++;
            } while ((u32)i < *(u16 *)k);
        }
        *(s32 *)(t + 0xDD8) = *(s32 *)p3;
        *(s32 *)(t + 0xDDC) = *(s32 *)(p3 + 4);
    }
    if (p2 != (u8 *)0) {
        p2 += 4;
        *(s32 *)(t + 0xDE0) = *(s32 *)p2;
        *(s32 *)(t + 0xDE4) = *(s32 *)(p2 + 4);
    }
}

void func_8004D75C(s32 index)
{
    ModelSlot *ch;
    ModelSlotPart *slot;
    s32 *cmd;
    u32 word;
    s32 row;
    s32 i;
    s32 key;

    ch = &D_800F2C40[index];
    if (ch->field_DD8 == 0) {
        return;
    }
    i = 0;
    if (i < ch->field_E1B) {
        for (; i < ch->field_E1B; i++) {
            slot = ch->field_1E0[i];
            if (slot == 0) {
                break;
            }
            row = 1;
            slot->start_sid = row;
            key = ch->field_1E0[i]->start;
            cmd = &ch->field_DD8[key];
            ch->field_2C8[row][i] = key;
            while (1) {
                word = *cmd;
                if ((s32)word < 0) {
                    row = word >> 16;
                    row = row & 0x7F;
                    if (row == 0) {
                        break;
                    }
                    if (ch->field_2C8[row][i] != 0xFFFF) {
                        cmd++;
                    } else {
                        ch->field_2C8[row][i] = *(u16 *)cmd;
                        cmd = &ch->field_DD8[*(u16 *)cmd];
                    }
                } else {
                    ch->field_750[row].values[i] =
                        ch->field_750[row].values[i] + *((u8 *)cmd + 2);
                    cmd++;
                }
            }
        }
    }
    for (row = 1; row < MODEL_SLOT_ROW_COUNT; row++) {
        ch->field_750[row].max = 0;
        for (i = 0; i < ch->field_E1B; i++) {
            if (ch->field_750[row].max < ch->field_750[row].values[i]) {
                ch->field_750[row].max = ch->field_750[row].values[i];
            }
        }
    }
}

void func_8004D914(s32 arg0)
{
    u8 *b;
    u8 *e;
    s32 *a;
    s32 *c;
    s32 *t;
    s32 *g;
    s32 i;
    s32 j;
    s32 k;
    s32 o;
    s32 w;
    s32 y;
    s32 x;
    u16 *yp;
    s32 v;
    s32 hi;
    s32 one;
    s32 ff;
    s32 mask;
    s32 bit;

    b = (u8 *)D_800F2C40 + arg0 * MODEL_SLOT_SIZE;
    o = 0;
    if (*(volatile s32 *)(b + 0xDD8) == 0) {
        return;
    }
    i = 0;
    if (b[0xE1B] == 0) {
        return;
    }

    ff = 0xFFFF;
    mask = 0xFF80FFFF;
    bit = 0x10000;
    one = 1;
    o = i;
    e = b;

    do {
        j = 1;
        k = o + 0x74;
        g = (s32 *)(*(volatile s32 *)(b + 0xDD8)
            + *(u16 *)(e + 0x33C) * 4);

        do {
            w = *(u16 *)(b + k + 0x2C8);
            a = (s32 *)(*(volatile s32 *)(b + 0xDD8) + w * 4);
            if (w != ff) {
                t = a - 1;
                while (1) {
                    x = *a;
                    if (x < 0) {
                        hi = (u32)x >> 16;
                        hi = hi & 0x7F;
                        yp = (u16 *)(b + (o + hi * 0x74) + 0x2C8);
                        y = *yp;
                        c = (s32 *)(*(volatile s32 *)(b + 0xDD8) + y * 4);
                        if (hi == 0) {
                            goto zero;
                        }
                        *a = (x & 0xC07FFFFF)
                            | ((((u32)x >> 16) & 0x7F) << 23);
                        if (y != ff) {
                            if (hi >= 2) {
                                if (c != (s32 *)0) {
                                    do {
                                        v = *c;
                                        if (v < 0) {
                                            if ((((u32)v >> 16) & 0x7F)
                                                == hi) {
                                                *c = (v & mask) | bit;
                                            }
                                            if ((*(u16 *)((u8 *)c + 2)
                                                & 0x7F) == one) {
                                                goto hit;
                                            }
                                        }
                                        c++;
                                    } while (c != (s32 *)0);
                                }
                            }
                        }
                    }
cont:
                    a++;
                }
hit:
                *(s16 *)c =
                    g - (s32 *)*(volatile s32 *)(b + 0xDD8);
                *(s16 *)a =
                    c - (s32 *)*(volatile s32 *)(b + 0xDD8);
                goto cont;
zero:
                *(s16 *)t =
                    (a - (s32 *)*(volatile s32 *)(b + 0xDD8)) - 1;
                *(s16 *)a =
                    (t - (s32 *)*(volatile s32 *)(b + 0xDD8)) + 1;
            }
            j++;
            k += 0x74;
        } while (j < MODEL_SLOT_ROW_COUNT);

        o += 2;
        e += 2;
        i++;
    } while (i < b[0xE1B]);
}
