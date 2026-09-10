/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
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

