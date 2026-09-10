#include "../types.h"
#include "model.h"
#include "model_slot_row_tables.h"

/* One model slot's row tables: the reset that clears them and imports the
   command list (0x8004D58C), and the walk that consumes both (0x8004D75C).
   The two are contiguous -- 0x8004D58C is 0x1D0 bytes and ends exactly at
   0x8004D75C -- and share the gcc_2_8_1_g0 profile with func_8004D134 below
   them, bounded above by func_8004D914 at gcc_2_8_1_g0_split.

   They are one initializer and its consumer, over the same fields of the same
   record: the reset fills keys with 0xFFFF, zeroes rows and their maxima, and
   stores the command list at 0xDD8; the walk then tests keys against 0xFFFF,
   accumulates rows, and reads that same list. */

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
    *(s16 *)(t + 0xE06) = 0;
    *(s16 *)(t + 0xE08) = 0;
    ch->field_DD8 = 0;
    *(s32 *)(t + 0xDDC) = 0;
    *(s32 *)(t + 0xDE0) = 0;
    *(s32 *)(t + 0xDE4) = 0;
    *(s32 *)(t + 0xDF0) = 0;
    /* The reset walks keys and rows with running byte offsets rather than
       indexing ch->field_2C8[i][j] / ch->field_750[i].values[j]. That is not
       a missed cleanup: the indexed form is larger, and the link fails with
       .initialized_data overlapping .text. The walkers are load-bearing. */
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
            slot->field_1A = row;
            key = ch->field_1E0[i]->field_18;
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
