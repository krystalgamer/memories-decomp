#include "../types.h"
#include "duel_effect.h"
#include "func_80036C14.h"
#include "../unmatched.h"

/* D_801D9174: a lookup table of 0x1E-byte records, each prefixed by a
   big-endian u16 id (id field for record i lives 2 bytes apart, but the
   record itself is 0x1E bytes -- id and record strides differ, so this is
   NOT `struct { u16 id; ... } records[]`; it is a compact id table walked
   in lockstep with the 0x1E-byte record table). Terminated by an id of 0.
   Returns a pointer to the matching record, or NULL if not found / list
   ends first. */

s32 func_80036BCC(s32 id) {
    u8 *rec;
    u8 *key;
    s32 v;

    rec = D_801D9174;
    key = D_801D9174_b;

loop_check:
    v = key[0] << 8;
    v |= key[1];
    if (v == 0) {
        return 0;
    }
    if (v == id) {
        return (s32) rec;
    }
    key += 2;
    rec += 0x1E;
    goto loop_check;
}

extern s16 D_801DA000[];

void func_80036C14(DuelEffectChannel *p, s32 a)
{
    DuelEffectEntry *q;
    s16 *r;
    u16 f;
    s32 v;
    s32 b;
    s32 c;

    q = p->entry_end_20;
    q->field_12 = p->index_57 + 1;
    q->field_13 = 1;
    q->field_15 = 0;
    r = D_801DA000;
    f = p->flags_34;
    if (f & 0x80) {
        q->field_10 = a;
        b = p->field_62;
        q->flags_11 = 0xA0;
        q->field_17 = b;
    } else if (f & 0x100) {
        v = (a >> 20) & 0xFF;
        if (v == 0) {
            return;
        }
        q->field_10 = v;
        q->flags_11 = 0xC0;
        c = p->field_54;
        q->field_13 = 0;
        q->field_16 = c;
    } else {
        a &= 0x8000FFFF;
        if (a == 0) {
            return;
        }
        if (f & 0x200) {
            r = (s16 *)(p->field_60 * 0x88 + (u8 *)r);
        }
        q->field_10 = 0;
        b = p->field_54;
        q->flags_11 = 0x80;
        *(s32 *)q = a;
        q->field_16 = b;
        r[0] = 0x280;
        r[2] = 4;
        r[1] = 0;
        r[3] = 0x10;
    }
    if (p->flags_34 & 0x1C00) {
        q->field_13 = 0;
    }
    q->x_0C = p->field_38;
    q->y_0E = p->field_3A;
    q++;
    q->flags_11 = 0;
    p->entry_end_20 = q;
}
