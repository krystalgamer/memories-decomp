#include "../types.h"
#include "model.h"
#include "func_8004D914.h"
#include "model_slot_row_tables.h"

/* One model slot's row-table reset, payload import, and command-list walk.
   func_8004D58C and func_8004D75C are the last two functions in the
   gcc_2_8_1_g0 run below func_8004D914; the channel decoder ahead of them,
   func_8004D134, remains a candidate.

   The model loader resets the tables before sending each event through the
   decoder; slot setup later walks the imported list. The reset fills keys
   with 0xFFFF, zeroes rows and maxima, and stores the command list at 0xDD8;
   the walk claims those keys, accumulates rows, and reads the same list. */

void func_8004D58C(s32 arg0, u8 *arg1)
{
    ModelSlot *ch;
    u8 *t;
    u8 *c;
    u8 *q;
    u8 *e;
    u8 *p3;
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
    s32 flag;
    s32 qd;

    do {
        do {
            p3 = (u8 *)0;
        } while (0);
    } while (0);
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
    *(s32 *)&ch->field_DDC = 0;
    *(s32 *)&ch->field_DE0 = 0;
    *(s32 *)&ch->field_DE4 = 0;
    ch->field_DF0 = 0;
    /* The reset walks keys and rows with running byte offsets rather than
       indexing ch->field_2C8[i][j] / ch->field_750[i].values[j]. That is not
       a missed cleanup: the indexed form is larger, and the link fails with
       .initialized_data overlapping .text. The walkers are load-bearing.

       The margin is one instruction, which is worth knowing before touching
       anything else here. These three pointer members cost nothing to name:
       they are stored as zero, so the *(s32 *)& that keeps the store a
       non-struct reference is free. The four copies out of the command
       blocks at the end are not -- direct member stores push .text into
       .initialized_data. Their byte-cursor form remains, with each
       displacement derived from the corresponding member instead. */
    do {
        ((ModelSlotRow *)(c + (u32)&((ModelSlot *)0)->field_750))->max = 0;
        j = 0;
        a = n;
        b = m;
        do {
            u = t + a;
            a += 2;
            v = t + b;
            b += 2;
            j++;
            *(u16 *)(v + (u32)&((ModelSlot *)0)->field_2C8) = ff;
            *(s16 *)(u + (u32)&((ModelSlot *)0)->field_750) = 0;
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
                do {
                    do {
                        qd = i / 8;
                    } while (0);
                } while (0);
                s = t + qd;
                d = qd << 3;
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
        *(s32 *)(t + (u32)&((ModelSlot *)0)->field_DD8) = *(s32 *)p3;
        *(s32 *)(t + (u32)&((ModelSlot *)0)->field_DDC) = *(s32 *)(p3 + 4);
    }
    if (p2 != (u8 *)0) {
        p2 += 4;
        *(s32 *)(t + (u32)&((ModelSlot *)0)->field_DE0) = *(s32 *)p2;
        *(s32 *)(t + (u32)&((ModelSlot *)0)->field_DE4) = *(s32 *)(p2 + 4);
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

/* The relink pass between the two above. Every reach now goes through
   ModelSlot: the command list is ch->field_DD8, the channel count is
   ch->field_E1B, and row 1's key for the current channel is
   ch->field_2C8[1][i].

   The two key reads inside the loops keep a byte-offset sum instead of
   ch->field_2C8[j][i]. Their address has to stay `ch + <running offset> +
   MODEL_SLOT_ROW_KEY_TABLE_OFFSET`, because that way the channel offset and
   the row stride share one induction variable; indexing the member gives the
   compiler a second one, which costs a callee-saved register and two
   instructions. Written left to right so the base is the first addend, which
   is also what the retail sum does. */
void func_8004D914(s32 arg0)
{
    ModelSlot *ch;
    s32 *a;
    s32 *c;
    s32 *t;
    s32 *g;
    s32 i;
    s32 j;
    s32 o;
    s32 w;
    s32 y;
    s32 x;
    u16 *yp;
    s32 k;
    s32 v;
    s32 hi;
    s32 one;
    s32 ff;
    s32 mask;
    s32 bit;

    ch = &D_800F2C40[arg0];
    o = 0;
    if (ch->field_DD8 == 0) {
        return;
    }
    i = 0;
    if (ch->field_E1B == 0) {
        return;
    }

    ff = 0xFFFF;
    mask = 0xFF80FFFF;
    bit = 0x10000;
    one = 1;
    o = i;

    do {
        j = 1;
        k = o + sizeof(ch->field_2C8[0]);
        g = &ch->field_DD8[ch->field_2C8[1][i]];

        do {
            w = *(u16 *)((u8 *)ch + k + MODEL_SLOT_ROW_KEY_TABLE_OFFSET);
            a = &ch->field_DD8[w];
            if (w != ff) {
                t = a - 1;
                while (1) {
                    x = *a;
                    if (x < 0) {
                        hi = (u32)x >> 16;
                        hi = hi & 0x7F;
                        yp = (u16 *)((u8 *)ch
                            + (o + hi * sizeof(ch->field_2C8[0]))
                            + MODEL_SLOT_ROW_KEY_TABLE_OFFSET);
                        y = *yp;
                        c = &ch->field_DD8[y];
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
                *(s16 *)c = g - ch->field_DD8;
                *(s16 *)a = c - ch->field_DD8;
                goto cont;
zero:
                /* Reloaded rather than reusing the base the links above
                   already hold: retail reads the member again here. */
                *(s16 *)t = (a - (s32 *)*(volatile s32 *)&ch->field_DD8) - 1;
                *(s16 *)a = (t - ch->field_DD8) + 1;
            }
            j++;
            k += sizeof(ch->field_2C8[0]);
        } while (j < MODEL_SLOT_ROW_COUNT);

        o += 2;
        i++;
    } while (i < ch->field_E1B);
}
