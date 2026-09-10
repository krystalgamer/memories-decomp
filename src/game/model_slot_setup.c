#include "../types.h"
#include "model_slot_setup.h"
#include "func_8004D914.h"
#include "model_word_memory.h"
#include "model.h"
#include "model_init_light_triplet.h"
#include "model_slot_row_tables.h"
#include "model_slot_support.h"
#include "model_slot_updates.h"

/* One model slot's setup: the reset that gives it its defaults (0x8005611C)
   and the per-frame duel-side layout pass that reads them (0x80056250). The
   two are contiguous -- 0x8005611C is 0x134 bytes and ends exactly at
   0x80056250 -- and are bounded above by model_load_monster_merge.c at a
   different profile.

   They are one initializer and its consumer. The reset writes the mode byte
   at +0xE16 as 0x3E, the pair at +0xE0C/+0xE0D as 7 and 8, the halfword at
   +0xE0A as 0x1000 and the busy byte at +0xE1F as 0; the layout pass switches
   on that same +0xE16 (0x3E is one of its three cases), reads +0xE0C, +0xE0D
   and +0xE0A back, and sets +0xE1F to 1. */

void func_8004CB0C(void);
void func_800582C0(s32 arg0, s32 arg1, s32 arg2);

void func_8005611C(s32 arg0)
{
    ModelSlot *p;
    u8 *q;
    s32 i;
    s32 n;

    p = &D_800F2C40[arg0];
    func_8005B5FC((s32 *)p, 0, 0x388);

    p->field_DA0[2] = MODEL_FIXED_HALF;
    p->field_DA0[1] = MODEL_FIXED_HALF;
    p->field_DA0[0] = MODEL_FIXED_HALF;
    p->field_DB0.field_08 = MODEL_FIXED_ONE;
    p->field_DB0.field_04 = MODEL_FIXED_ONE;
    p->field_DB0.field_00 = MODEL_FIXED_ONE;
    p->field_DC0[2] = 0x80;
    p->field_DC0[1] = 0x80;
    p->field_DC0[0] = 0x80;
    p->field_E0C = 7;
    p->field_E0D = 8;
    p->field_E14 = 0xFF;
    p->field_DC0[3] = 0;
    *(s16 *)&p->field_E0A = 0x1000;
    p->field_E1D = 0;
    p->field_DFE = arg0;
    p->field_DFF = 0;
    p->field_DFC = 0xFFFF;
    p->field_DFA = 0xFFFF;

    n = 1;
    i = 3;
    q = (u8 *)p + i;
    for (; i >= 0; q--, i--) {
        q[0xBF4] = n;
    }

    for (i = 0; i < 0x40; i++) {
        *(s32 *)((u8 *)p + i * 4 + 0xBF8) &= 0x8000FFFF;
        ((u8 *)p)[i * 4 + 0xBF9] = 0;
        ((u8 *)p)[i * 4 + 0xBF8] = 0;
    }

    *(s32 *)&p->field_CF8.field_0C[2] = -1;
    *(s32 *)&p->field_CF8.field_0C[4] = -1;
    *(s32 *)&p->field_CF8.field_0C[6] = -1;
    p->field_E16 = 0x3E;
    p->field_E1F = 0;
    Model_InitLightTriplet(arg0);
}

/* Duel-side layout pass for one player's model slot: sums the hand's card
 * widths (0x14 for the cards flagged in the +0xBEC bitfield, 0xC otherwise),
 * derives the two cursor limits at +0xDF0/+0xDF4, resets each card object's
 * sprite fields, and applies the mode-dependent horizontal offset through
 * func_8005A468 before func_800582C0 draws it. */
void func_80056250(s32 arg0, u8 *arg1, s32 arg2, s32 arg3) {
    ModelSlot *p;
    ModelSlot *q;
    ModelSlotPart **c;
    ModelSlot *e;
    ModelSlot *r;
    s32 i;
    s32 j;
    s32 sum;
    s32 k;
    s32 v;
    s32 m;
    s32 n;
    u8 *b;
    u8 *b2;
    s32 w;

    func_8004CB0C();
    if (arg1 == (u8 *)0) {
        return;
    }
    func_8004D75C(arg0);
    func_8004D914(arg0);
    p = &D_800F2C40[arg0];
    sum = 0;
    if (arg0 < 2) {
        i = sum;
        if (sum < p->field_E1B) {
            m = 0xFFFF;
            n = p->field_E1B;
            do {
                if (p->field_2C8[1][i] != m) {
                    k = i / 8;
                    if ((p->field_BEC[k] >> (i - k * 8)) & 1) {
                        sum += 0x14;
                    } else {
                        sum += 0xC;
                    }
                }
                i++;
            } while (i < n);
        }
    }
    v = (p->field_DF0 = (s32)p->field_DE0 + sum);
    p->field_DF4 = v + p->field_E02 * 4;
    func_8005A4C4(p, 0, 0, 0, arg0 == 1 ? 0x800 : 0);

    q = &D_800F2C40[arg0];
    c = q->field_1E0;
    for (j = 0; j < q->field_E1B; j++) {
        (*c)->ii = 0xFFFF;
        (*c)->ti = (*c)->start;
        (*c)->aframe = 0xFFFF;
        (*c)->sid = (*c)->start_sid;
        (*c)->rframe = 0;
        (*c)->speed = 0x10;
        c++;
    }

    b = (u8 *)D_800F2C40;
    e = (ModelSlot *)(arg0 * MODEL_SLOT_SIZE + b);
    e->field_E1F = 1;
    func_800590DC(arg0);
    v = e->field_E0D * 2;
    w = v;
    switch (e->field_E16) {
    case 0x23:
        w = 0;
        func_8005A468(arg0, w);
        break;
    case 0x3E:
        w = v;
        func_8005A468(arg0, w);
        break;
    case 0x3C:
        w = -w;
        func_8005A468(arg0, w);
        break;
    }
    if (arg0 < 2) {
        b2 = (u8 *)D_800F2C40;
        r = (ModelSlot *)(arg0 * MODEL_SLOT_SIZE + b2);
        func_800582C0(arg0, r->field_E0C, r->field_E0A);
    }
}
