#include "../types.h"
#include "duel_effect.h"
#include "func_80036C14.h"
#include "../unmatched.h"
#ifdef VERSION_JAPAN
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "duel_effect_entry_occupancy.h"
#include "japanese/duel_effect_channel.h"
#endif

/* D_801D9174: a lookup table of 0x1E-byte records, each prefixed by a
   big-endian u16 id (id field for record i lives 2 bytes apart, but the
   record itself is 0x1E bytes -- id and record strides differ, so this is
   NOT `struct { u16 id; ... } records[]`; it is a compact id table walked
   in lockstep with the 0x1E-byte record table). Terminated by an id of 0.
   Returns a pointer to the matching record, or NULL if not found / list
   ends first. */

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_TEXT_FIND_RECORD_BY_ID)
s32 Text_FindRecordById(s32 id) {
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
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_APPEND_ENTRY)
void DuelEffect_AppendEntry(DuelEffectChannel *p, s32 a)
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
#ifndef VERSION_JAPAN
        b = p->field_62;
#endif
        q->flags_11 = 0xA0;
#ifndef VERSION_JAPAN
        q->field_17 = b;
#endif
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
#ifdef VERSION_JAPAN
        v = func_80035D10();
#endif
        if (a == 0) {
            return;
        }
#ifdef VERSION_JAPAN
        if (v < 0) {
            return;
        }
        if (p->flags_34 & 0x200) {
            r = (s16 *)(((JapaneseDuelEffectChannel *)p)->pad_5E * 0x88 + (u8 *)r);
        }
        q->field_10 = v;
        func_800362AC(p, a, v, r + 4);
#else
        if (f & 0x200) {
            r = (s16 *)(p->field_60 * 0x88 + (u8 *)r);
        }
        q->field_10 = 0;
        b = p->field_54;
#endif
        q->flags_11 = 0x80;
        *(s32 *)q = a;
#ifndef VERSION_JAPAN
        q->field_16 = b;
#endif
#ifdef VERSION_JAPAN
        r[0] = 0x280 + ((v & 0xF) << 2);
        r[1] = v & 0xF0;
#else
        r[0] = 0x280;
#endif
        r[2] = 4;
#ifndef VERSION_JAPAN
        r[1] = 0;
#endif
        r[3] = 0x10;
#ifdef VERSION_JAPAN
        if (p->flags_34 & 0x200) {
            LoadImage((RECT *)r, (u32 *)(r + 4));
        } else {
            LoadImage2((RECT *)r, (u32 *)(r + 4));
        }
#endif
    }
    if (p->flags_34 & 0x1C00) {
        q->field_13 = 0;
    }
    q->x_0C = p->field_38;
    q->y_0E = p->field_3A;
#ifdef VERSION_JAPAN
    q = (DuelEffectEntry *)((u8 *)q + 0x18);
#else
    q++;
#endif
    q->flags_11 = 0;
    p->entry_end_20 = q;
}
#endif
