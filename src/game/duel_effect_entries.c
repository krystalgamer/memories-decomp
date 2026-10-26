#include "../types.h"
#include "text_box_blocking_build.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_entries.h"
#include "../unmatched.h"
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
#include "duel_effect_entry_occupancy.h"
#endif
#ifdef VERSION_JAPAN
#include "../psyq/libapi.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "japanese/duel_effect_channel.h"
#endif

/* The big-endian key and payload cursors advance by 2 and 0x1E independently.
   Their North American aliases start at the same address; Japan binds them
   separately. Neither walk establishes an interleaved record layout.
   A zero key terminates before the match test; the payload stays opaque. */

#ifndef VERSION_EUROPE
u8 *Text_FindRecordById(s32 id) {
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
        return rec;
    }
    key += 2;
    rec += 0x1E;
    goto loop_check;
}
#endif

#ifdef VERSION_JAPAN
/* Rasterises one 16x16 glyph into a 4bpp bitmap for the Japanese text
 * boxes. The glyph comes from the game's own text records when the value
 * is negative and from the BIOS kanji ROM otherwise; it has fifteen rows of
 * sixteen bits. Each nibble of a row selects one of sixteen pixel patterns,
 * masked to the channel's colour, and a copy shifted one pixel is ORed into
 * the row below as a shadow. */
void DuelEffect_RasterizeGlyph(DuelEffectChannel *channel, s32 tagged_value, s32 slot, s16 *bitmap)
{
    u16 *glyph;
    u16 *lut;
    u16 *p;
    u16 *row;
    u32 w;
    s32 v;
    s32 i;

    gDuelEffect_aOccupancyValues[slot] = channel->channel_index + 1;
    if (tagged_value < 0) {
        glyph = (u16 *)Text_FindRecordById(tagged_value & 0xFFFF);
    } else {
        glyph = (u16 *)Krom2RawAdd2(tagged_value & 0xFFFF);
    }
    lut = (u16 *)0x1F800000;
    w = gText_awGlyphColorMasks[channel->color_index_54];
    p = lut;
    for (i = 0; i < 16; i++) {
        *p++ = gText_awGlyphNibblePatterns[i] & w;
    }
    p = lut;
    row = (u16 *)0x1F800040;
    lut = (u16 *)0x1F800040;
    *(u32 *)&row[0] = 0;
    *(u32 *)&row[2] = 0;
    for (i = 15; i != 0; i--) {
        w = *glyph++;
        v = p[(w >> 4) & 0xF];
        row[0] |= v;
        row[4] = v & ~0x1111;
        row[1] |= row[4] >> 12;
        v = p[w & 0xF];
        row[1] |= v;
        row[5] = v & ~0x1111;
        row[2] |= row[5] >> 12;
        v = p[w >> 12];
        row[2] |= v;
        row[6] = v & ~0x1111;
        row[3] |= row[6] >> 12;
        v = p[(w >> 8) & 0xF];
        row[3] |= v;
        row[7] = v & ~0x1111;
        row += 4;
    }
    row = lut;
    for (i = 16; i != 0; i--) {
        ((u32 *)bitmap)[0] = ((u32 *)row)[0];
        ((u32 *)bitmap)[1] = ((u32 *)row)[1];
        row += 4;
        bitmap += 4;
    }
}
#endif

#ifdef VERSION_EUROPE
s32 DuelEffect_AppendEntry(DuelEffectChannel *channel, s32 tagged_value)
{
    EuropeanDuelEffectEntry *entry;
    u16 flags;
    s32 glyph;
    s32 mode;
    s32 result;

    flags = 0;
    entry = (EuropeanDuelEffectEntry *)channel->entry_end_20;
    entry->owner_channel_tag = channel->channel_index + 1;
    entry->handler_state = 1;
    entry->render_mode = 0;
    entry->local_x = channel->pen_x;
    entry->local_y = channel->pen_y;
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
    entry->pad_14[0] = channel->color_index_54;
    entry->flags_11 = mode | 0x80;

    if (mode == 3) {
        entry->local_x += 2;
    }
    if (mode == 2) {
        if (glyph == 0x28 || glyph == 0x42 || glyph == 0x45) {
            entry->local_x -= 2;
            result = -4;
        }
        if (glyph == 0x3F || glyph == 0x43 || glyph == 0x4D) {
            entry->local_x -= 1;
            result = -2;
        }
    }
    if (mode == 0) {
        if (glyph == 7) {
            entry->local_x -= 3;
            result = -6;
        }
        if (glyph == 0x3F || glyph == 0x42 || glyph == 0x45 ||
            glyph == 0xE || glyph == 0xC) {
            entry->local_x -= 1;
            result = -2;
        }
    }

append:
    if (channel->flags_34 & 0x1C00) {
        entry->handler_state = 0;
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
    q->owner_channel_tag = p->channel_index + 1;
    q->handler_state = 1;
    q->render_mode = 0;
    r = gText_GlyphUploadScratch;
    f = p->flags_34;
    if (f & 0x80) {
        q->glyph_index = a;
#ifndef VERSION_JAPAN
        b = p->field_62;
#endif
        q->flags_11 = 0xA0;
#ifndef VERSION_JAPAN
        q->icon_variant = b;
#endif
    } else if (f & 0x100) {
        v = (a >> 20) & 0xFF;
        if (v == 0) {
            return;
        }
        q->glyph_index = v;
        q->flags_11 = 0xC0;
        c = p->color_index_54;
        q->handler_state = 0;
        q->palette_index = c;
    } else {
        a &= 0x8000FFFF;
#ifdef VERSION_JAPAN
        v = DuelEffect_FindFreeOccupancySlot();
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
        q->glyph_index = v;
        DuelEffect_RasterizeGlyph(p, a, v, r + 4);
#else
        if (f & 0x200) {
            r = (s16 *)(p->field_60 * 0x88 + (u8 *)r);
        }
        q->glyph_index = 0;
        b = p->color_index_54;
#endif
        q->flags_11 = 0x80;
        *(s32 *)q = a;
#ifndef VERSION_JAPAN
        q->palette_index = b;
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
        q->handler_state = 0;
    }
    q->x_0C = p->pen_x;
    q->y_0E = p->pen_y;
#ifdef VERSION_JAPAN
    q = (DuelEffectEntry *)((u8 *)q + 0x18);
#else
    q++;
#endif
    q->flags_11 = 0;
    p->entry_end_20 = q;
}
#endif
