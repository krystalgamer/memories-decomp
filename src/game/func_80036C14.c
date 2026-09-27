#include "../types.h"
#include "duel_effect.h"
#include "func_80036C14.h"
#include "../unmatched.h"
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
#include "duel_effect_entry_occupancy.h"
#endif
#ifdef VERSION_JAPAN
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "japanese/duel_effect_channel.h"
#endif

/* D_801D9174: a lookup table of 0x1E-byte records, each prefixed by a
   big-endian u16 id (id field for record i lives 2 bytes apart, but the
   record itself is 0x1E bytes -- id and record strides differ, so this is
   NOT `struct { u16 id; ... } records[]`; it is a compact id table walked
   in lockstep with the 0x1E-byte record table). Terminated by an id of 0.
   Returns a pointer to the matching record, or NULL if not found / list
   ends first. */

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_TEXT_FIND_RECORD_BY_ID)) && \
    !defined(VERSION_EUROPE)
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

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_APPEND_ENTRY)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80036C14))
#ifdef VERSION_EUROPE
s32 func_80036C14(DuelEffectChannel *channel, s32 tagged_value)
{
    EuropeanDuelEffectEntry *entry;
    u16 flags;
    s32 glyph;
    s32 mode;
    s32 result;

    flags = 0;
    entry = (EuropeanDuelEffectEntry *)channel->entry_end_20;
    entry->field_12 = channel->index_57 + 1;
    entry->field_13 = 1;
    entry->field_15 = 0;
    entry->field_0C = channel->field_38;
    entry->field_0E = channel->field_3A;
    entry->code_00 = tagged_value;
    flags = channel->flags_34;
    result = 0;

    if (flags & 0x100) {
        entry->pad_0D[1] = tagged_value;
        entry->flags_11 = 0xC0;
        goto append;
    }

    glyph = (tagged_value >> 20) & 0xFF;
    mode = flags & 3;
    if (glyph == 0) {
        if (mode != 3) {
            return -1;
        }
        return 0;
    }

    entry->pad_0D[1] = glyph;
    entry->pad_14[0] = channel->field_54;
    entry->flags_11 = mode | 0x80;

    if (mode == 3) {
        entry->field_0C += 2;
    }
    if (mode == 2) {
        if (glyph == 0x28 || glyph == 0x42 || glyph == 0x45) {
            entry->field_0C -= 2;
            result = -4;
        }
        if (glyph == 0x3F || glyph == 0x43 || glyph == 0x4D) {
            entry->field_0C -= 1;
            result = -2;
        }
    }
    if (mode == 0) {
        if (glyph == 7) {
            entry->field_0C -= 3;
            result = -6;
        }
        if (glyph == 0x3F || glyph == 0x42 || glyph == 0x45 ||
            glyph == 0xE || glyph == 0xC) {
            entry->field_0C -= 1;
            result = -2;
        }
    }

append:
    if (channel->flags_34 & 0x1C00) {
        entry->field_13 = 0;
    }
    entry++;
    entry->flags_11 = 0;
    channel->entry_end_20 = (DuelEffectEntry *)entry;
    return result;
}
#else
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
#endif
