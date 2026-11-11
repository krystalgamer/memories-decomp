/* Duel effect text: channel setup (the state reset, text-box rectangle and
 * entry initialisation), the text-box lifecycle, the occupancy table and
 * entry markers, the glyph draw callback, entry appending, the
 * little-endian stream readers, the choice widget layout, cursor and update,
 * the pulse colour, entry control, the text state callbacks, line wrapping
 * and the extended glyph code; then the text commands: the stream state
 * command, the secondary duel-effect command table entries, the campaign
 * duel start, the sound, fade and object commands, the primary control-byte
 * handlers and text-box layout helpers, TextBox_BuildStep, the text-box
 * runtime, the display-object fade helpers and callbacks, entry processing
 * and the menu record reset. In every executable these are one contiguous
 * run of text (0x80035A64-0x80039F1C in the US build). */
#include "../types.h"

#include "duel_effect_states.h"
#include "duel_effect_text.h"
#include "duel_effect_entry_ranges.h"
#include "display_object_system.h"

void DuelEffect_ResetState(void)
{
    s32 i = 4;
    DuelEffectChannel *entry = gText_aChannels;

    do {
        entry->flags_34 = 0;
        entry->prompt_object = 0;
        entry->aux_object = 0;
        entry->render_object = 0;
#ifdef VERSION_JAPAN
        entry = (DuelEffectChannel *)((u8 *)entry + 0x60);
#elif defined(VERSION_EUROPE)
        /* The European channel is 100 bytes. */
        entry = (DuelEffectChannel *)((u8 *)entry + 100);
#else
        entry++;
#endif
    } while (--i != 0);
#ifndef VERSION_EUROPE
    /* The European build makes no occupancy reset here. */
    DuelEffect_ResetOccupancy();
#endif
    DuelEffect_ResetEntryMarkers();
}

void TextBox_SetRect(int index, int x, int y, int width, int height)
{
    DuelEffectChannel *entry = &gText_aChannels[index];
    entry->rect_x = x;
    entry->rect_y = y;
    entry->rect_width = width;
    entry->rect_height = height;
}

#ifdef VERSION_EUROPE
/* The European channel is 100 bytes. The low two flag bits pick one of
   four text sizes, the same four as the European Text_SetFontMode (8 and 16
   by default), and the halfword at 0x62 is cleared. */
DuelEffectChannel *DuelEffect_InitEntry(int index, int value, int flags)
{
    DuelEffectChannel *e;
    unsigned short *range;
    s32 narrow;
    s32 wide;
    s32 size;

    e = (DuelEffectChannel *)((u8 *)gText_aChannels + index * 100);
    e->flags_34 = flags | DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
    e->initial_string_id = value;
    narrow = 8;
    wide = 16;
    size = flags & 3;
    e->channel_index = index;
    e->color_index_54 = 0;
    e->pen_x = 0;
    e->pen_y = 0;
    e->glyph_advance_x = narrow;
    e->line_advance_y = wide;
    switch (size) {
    case 1:
        e->glyph_advance_x = narrow;
        e->line_advance_y = narrow;
        break;
    case 2:
        e->glyph_advance_x = 12;
        e->line_advance_y = wide;
        break;
    case 3:
        e->glyph_advance_x = wide;
        e->line_advance_y = wide;
        break;
    }
    e->glyph_delay = 1;
    range = (unsigned short *)((unsigned char *)gDuelEffect_awEntryRangeBoundaries + (index << 1));
    e->depth_offset = 0;
    *(u16 *)((u8 *)e + 0x62) = 0;
    e->entry_range_start = range[0];
    e->entry_range_count = range[1] - range[0];
    return e;
}
#else
DuelEffectChannel *DuelEffect_InitEntry(int index, int value, int flags)
{
    int offset = index << 1;
    DuelEffectChannel *e;
#ifdef VERSION_JAPAN
    unsigned char *range;
#else
    unsigned short *range;
#endif
    e = &gText_aChannels[index];
    flags |= DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
    /* The do/while (0) loop notes are a barrier to GCC's first scheduling
       pass, which would otherwise sink the flags OR to its store; retail
       ORs right after the channel address. */
    do {
#ifdef VERSION_JAPAN
        e->line_advance_y = 16;
        e->glyph_advance_x = 16;
#else
        e->glyph_advance_x = 8;
        e->line_advance_y = 12;
#endif
    } while (0);
    e->glyph_delay = 2;
#ifdef VERSION_JAPAN
    /* The 16-bit boundaries are stored modulo 256 in the compact channel. */
    range = (unsigned char *)gDuelEffect_awEntryRangeBoundaries + offset;
#else
    range = (unsigned short *)((unsigned char *)gDuelEffect_awEntryRangeBoundaries + offset);
#endif
    e->channel_index = index;
    e->initial_string_id = value;
    e->color_index_54 = 0;
    e->flags_34 = flags;
    e->pen_x = 0;
    e->pen_y = 0;
    e->depth_offset = 0;
#ifdef VERSION_JAPAN
    e->output_limit = 0;
    e->entry_range_start = range[0];
    e->entry_range_count = range[2] - range[0];
#else
    e->output_limit = 0;
    e->entry_range_start = range[0];
    e->entry_range_count = range[1] - range[0];
#endif
    return (DuelEffectChannel *)e;
}
#endif

void TextBox_Destroy(DuelEffectChannel *record)
{
#ifdef VERSION_EUROPE
    /* The European build leaves an inactive channel alone and no longer
       clears the channel's occupancy value. */
    if (record->flags_34 & DUEL_EFFECT_CHANNEL_FLAG_ACTIVE) {
        DuelEffect_ClearMatchingMarker(record->channel_index);
        record->flags_34 = 0;
        DisplayObject_ReleaseIfPresent(record->prompt_object);
        DisplayObject_ReleaseIfPresent(record->aux_object);
        DisplayObject_ReleaseIfPresent(record->render_object);
        record->prompt_object = 0;
        record->aux_object = 0;
        record->render_object = 0;
    }
#else
    DuelEffect_ClearOccupancyValue(record->channel_index);
    DuelEffect_ClearMatchingMarker(record->channel_index);
    record->flags_34 = 0;
    DisplayObject_ReleaseIfPresent(record->prompt_object);
    DisplayObject_ReleaseIfPresent(record->aux_object);
    DisplayObject_ReleaseIfPresent(record->render_object);
    record->prompt_object = 0;
    record->aux_object = 0;
    record->render_object = 0;
#endif
}

#ifdef VERSION_EUROPE
/* European only: destroys the channel at an index into gText_aChannels. */
void TextBox_DestroyIndex(s32 index)
{
    TextBox_Destroy(&gText_aChannels[index]);
}
#endif

void *TextBox_Create(
    s32 index,
    s32 string_id,
    s32 x,
    s32 y,
    s32 width,
    s32 height
)
{
    TextBox_SetRect(index, x, y, width, height);
    return (u8 *)DuelEffect_InitEntry(index, string_id, 0);
}

void *TextBox_CreateFlagged(
    s32 index,
    s32 string_id,
    s32 x,
    s32 y,
    s32 width,
    s32 height,
    s32 flags
)
{
#ifdef VERSION_EUROPE
    /* The European build hands the flags to DuelEffect_InitEntry. */
    TextBox_SetRect(index, x, y, width, height);
    return DuelEffect_InitEntry(index, string_id, flags);
#else
    DuelEffectChannel *result;

    TextBox_SetRect(index, x, y, width, height);
    result = DuelEffect_InitEntry(index, string_id, 0);
    result->flags_34 |= flags;
    return result;
#endif
}

#include "duel_effect_entry_state.h"

/* Zero-initialized storage this unit owns at 0x800EAF08-0x800EAFF8 in the
 * US build; the generated duel_effect_text_commons subsegment places it.
 * Nothing in the European builds reads it, and their images have no such
 * storage. */
#ifndef VERSION_EUROPE
u8 gDuelEffect_aOccupancyValues[240];
#endif

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gDuelEffect_OccupancyStartRow;
u8 gDuelEffect_OccupancyStartColumn;

#ifndef VERSION_EUROPE
void DuelEffect_ClearOccupancyValue(int value)
{
    int index;
    u8 *entry = gDuelEffect_aOccupancyValues;
    value++;
    for (
        index = 0;
        index < DUEL_EFFECT_OCCUPANCY_COUNT;
        index++, entry++
    ) {
        if (*entry == value) {
            *entry = 0;
        }
    }
}
#endif

/* Clears the gDuelEffect_aOccupancyValues occupancy table (see DuelEffect_ClearOccupancyValue) and resets its
   scan cursor to (0,0). */
#ifndef VERSION_EUROPE
void DuelEffect_ResetOccupancy(void) {
    u8 *v0;
    int v1;
    v0 = gDuelEffect_aOccupancyValues;
    v1 = DUEL_EFFECT_OCCUPANCY_COUNT - 1;
    do {
        *v0 = 0;
        v1 = v1 - 1;
        v0 = v0 + 1;
    } while (v1 >= 0);
    gDuelEffect_OccupancyStartColumn = 0;
    gDuelEffect_OccupancyStartRow = 0;
}
#endif

#ifndef VERSION_EUROPE
s32 DuelEffect_FindFreeOccupancySlot(void)
{
    s32 col = gDuelEffect_OccupancyStartColumn;
    s32 row = gDuelEffect_OccupancyStartRow;
    s32 count = 0x100;
    do {
        s32 index = row * 16 + col;
        if (gDuelEffect_aOccupancyValues[index] == 0)
            return index;
        col++;
        if ((col & 3) == 0) {
            row++;
            if (row >= 0xF)
                row = 0x10;
            if ((row & 3) == 0)
                row -= 4;
            else
                col -= 4;
            if (col >= 0x10) {
                row += 4;
                col = 0;
                if (row >= 0x10)
                    row = 0;
            }
        }
        count--;
    } while (count != 0);
    return -1;
}
#endif

#ifdef VERSION_JAPAN
#define DUEL_EFFECT_MARKER_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_MARKER_ENTRY_COUNT JAPANESE_DUEL_EFFECT_ENTRY_COUNT
#elif defined(VERSION_EUROPE)
#define DUEL_EFFECT_MARKER_ENTRY_TYPE EuropeanDuelEffectEntry
#define DUEL_EFFECT_MARKER_ENTRY_COUNT EUROPEAN_DUEL_EFFECT_ENTRY_COUNT
#else
#define DUEL_EFFECT_MARKER_ENTRY_TYPE DuelEffectEntry
#define DUEL_EFFECT_MARKER_ENTRY_COUNT DUEL_EFFECT_ENTRY_COUNT
#endif

/* Clears the marker on each regional entry whose owner_channel_tag equals a0 + 1. */
void DuelEffect_ClearMatchingMarker(int a0) {
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    DUEL_EFFECT_MARKER_ENTRY_TYPE *v1;
    int a1;
    u8 v0;
    a1 = DUEL_EFFECT_MARKER_ENTRY_COUNT;
    a0 = a0 + 1;
    v1 = (DUEL_EFFECT_MARKER_ENTRY_TYPE *)gDuelEffect_aEntries;
    do {
        v0 = v1->owner_channel_tag;
        if (v0 == a0) {
            v1->entry_flags = 0;
        }
        a1 -= 1;
        v1 += 1;
    } while (a1 != 0);
#else
    DuelEffectEntry *v1;
    int a1;
    u8 v0;
    a1 = DUEL_EFFECT_ENTRY_COUNT;
    a0 = a0 + 1;
    v1 = gDuelEffect_aEntries;
    do {
        v0 = v1->owner_channel_tag;
        if (v0 == a0) {
            v1->entry_flags = 0;
        }
        a1 -= 1;
        v1 += 1;
    } while (a1 != 0);
#endif
}

/* Clears the marker byte in every entry; the North American layout also
   clears layout_selector, the Japanese and European ones do not. */
void DuelEffect_ResetEntryMarkers(void) {
    DUEL_EFFECT_MARKER_ENTRY_TYPE *v0;
    int v1;
    /* Old GCC preserves the regional initialization order in the output. */
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    v0 = (DUEL_EFFECT_MARKER_ENTRY_TYPE *)gDuelEffect_aEntries;
    v1 = DUEL_EFFECT_MARKER_ENTRY_COUNT;
#else
    v1 = DUEL_EFFECT_MARKER_ENTRY_COUNT;
    v0 = gDuelEffect_aEntries;
#endif
    for (; v1 != 0; v1 = v1 - 1) {
        v0->entry_flags = 0;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        v0->layout_selector = 0;
#endif
        v0 = v0 + 1;
    }
}

/*
 * TextBox_DrawEntries, a display-object draw callback (two parameters, the object
 * and its ordering table), installed in a display object's +0x4C slot by
 * TextBox_RefreshDisplayObjects.
 *
 * Levers measured on this body:
 * - each loop walks a single cursor from the start of the glyph record and
 *   reads every field at its offset from there (a typed DuelEffectEntry in
 *   North America). A cursor biased into the middle of the record, or a
 *   second cursor for the glyph code, makes strength reduction add another
 *   induction register;
 * - the tpage attribute flag is read into `w` before the byte at +0x66, and
 *   that byte load is held in a do/while. Together they put the load after
 *   the flag test, which is where retail has it, and they also let the
 *   scheduler split the 0x1F8000A0 constant's lui and ori the way retail
 *   does;
 * - `b` is a u32: its width decides the operand order of the tpage `or`;
 * - the four RotAverageNclip4 corner vectors are stored vector by vector
 *   (x, y, z of each in turn) with literal -8 and 8. Grouping the stores by
 *   value, or naming the 8, materialises the constants ahead of the three
 *   vector addresses, which retail computes first.
 */
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "main.h"

#define GS_SPRITE_COLOR_WORD(sprite) (*(u32 *)&(sprite)->r)

#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
#ifdef VERSION_EUROPE
#define GLYPH_POSITION_OFFSET 0x02
#define GLYPH_SHADE_OFFSET 0x06
#define GLYPH_ROTATION_OFFSET 0x0A
#define GLYPH_INDEX_OFFSET 0x0E
#define GLYPH_FLAGS_OFFSET 0x0F
#define GLYPH_NUDGE_OFFSET 0x12
#define GLYPH_MODE_OFFSET 0x13
#define GLYPH_PALETTE_OFFSET 0x14
#define GLYPH_TINT_OFFSET 0x15
#define GLYPH_RECORD_SIZE 0x16
#define GLYPH_UV_END uvEnd
#define GLYPH_UV_WIDTH ((u8)spr->w)
#define GLYPH_UV_HEIGHT ((u8)spr->h)
#define GLYPH_SECOND_CLUT 0x3FEA
#else
#define GLYPH_SHADE_OFFSET 0x04
#define GLYPH_ROTATION_OFFSET 0x08
#define GLYPH_POSITION_OFFSET 0x0C
#define GLYPH_INDEX_OFFSET 0x10
#define GLYPH_FLAGS_OFFSET 0x11
#define GLYPH_NUDGE_OFFSET 0x14
#define GLYPH_MODE_OFFSET 0x15
#define GLYPH_PALETTE_OFFSET 0x16
#define GLYPH_RECORD_SIZE 0x18
#define GLYPH_UV_END su
#define GLYPH_UV_WIDTH 0x10
#define GLYPH_UV_HEIGHT 0x10
#define GLYPH_SECOND_CLUT 0x3FE2
#endif

/* Europe shares the scratchpad setup and polygon submission with Japan, but
 * walks 0x16-byte records with 8x16, 12x16, 16x16 and special 32x16 glyphs.
 * Europe hoists &vec[2], &vec[3] and &vec[4] out of the glyph loop and keeps
 * them as offsets from vec. The tpage is therefore an if/else whose arms both
 * end in `tpage = t | b`. With a plain `if (w) b |= 0x80;`, cse2 skips the
 * guarded block, still knows vec's scratchpad address in the loop preheader,
 * and turns the three hoisted addresses into separate constants. The later
 * jump pass merges the identical arm tails, which leaves retail's single `or`.
 *
 * The Japanese renderer walks 0x18-byte glyph records and draws each glyph
 * from a single byte: it has neither the Shift-JIS table nor the half-width
 * glyphs, so every glyph is 16 pixels square.
 *
 * Case 1 falls through to case 0 for the plain sprite. With one
 * GsSortFastSprite call, pri has one reference fewer, so gcc spills it and
 * gives $s7 to the (u16)pri it hoists out of the loop for case 2's calls,
 * as retail does; two calls give pri $s7 instead. */
void TextBox_DrawEntries(DisplayObject *obj, GsOT *ot)
{
    s32 x;
    s32 y;
    MATRIX *mat;
    s32 pri;
    POLY_FT4 *ft4;
    SVECTOR *vec;
    long *res;
    GsSPRITE *sprites[3];
    GsSPRITE *spr;
    POLY_GT4 *gt4;
    u8 *p;
    s32 t;
    s32 w;
    s32 su;
    s32 tpage;
    s32 tt;
    u8 f;
    u32 b;
#ifdef VERSION_EUROPE
    u32 halfWidth;
    u32 halfHeight;
    u32 attribute;
#endif

    SetGeomScreen(0x12C);
    x = (s16)obj->xy.h.x;
    y = (s16)obj->xy.h.y;
    pri = (s16)obj->ot_depth;
    if (!(obj->flags & 8)) {
        x = x - gGraphics_sViewportX;
        y = y - gGraphics_sViewportY;
    }
    ft4 = (POLY_FT4 *)0x1F800038;
    vec = (SVECTOR *)0x1F800060;
#ifdef VERSION_EUROPE
    /* This boundary preserves the European object's incoming-argument spill. */
    do {
        mat = (MATRIX *)0x1F800078;
    } while (0);
#else
    mat = (MATRIX *)0x1F800078;
#endif
    res = (long *)0x1F8000A0;
    gt4 = (POLY_GT4 *)0x1F800000;
    sprites[0] = (GsSPRITE *)0x1F8000C0;
    sprites[1] = (GsSPRITE *)0x1F800100;
    sprites[2] = (GsSPRITE *)0x1F800140;
    SetPolyGT4((POLY_GT4 *)0x1F800000);
    SetSemiTrans((void *)0x1F800000, 1);
    *(u32 *)&ft4->r0 = 0x808080;
    setlen(ft4, 9);
    setcode(ft4, 0x2C);
    ft4->clut = (obj->field_40.clut.y << 6) |
                (((u16)obj->field_40.clut.x >> 4) & 0x3F);
    t = (obj->attribute >> 0x17) & 0x60;
    w = obj->attribute & 0x01000000;
    do {
        b = obj->texture_page;
    } while (0);
    if (w) {
        b |= 0x80;
        tpage = t | b;
    } else {
        tpage = t | b;
    }
    ft4->tpage = tpage;
    sprites[0]->attribute = sprites[1]->attribute = sprites[2]->attribute =
        obj->attribute | 0x08000000;
    GS_SPRITE_COLOR_WORD(sprites[0]) = obj->rgb;
    GS_SPRITE_COLOR_WORD(sprites[1]) = obj->rgb;
    GS_SPRITE_COLOR_WORD(sprites[2]) = obj->rgb;
#ifdef VERSION_EUROPE
    *(u32 *)&sprites[0]->w = 0x100008;
    *(u32 *)&sprites[0]->cx = obj->field_40.word;
    sprites[0]->tpage = sprites[2]->tpage = 0xA;
    p = (u8 *)gText_aChannels[obj->field_67].entry_begin;
#else
    *(u32 *)&sprites[0]->w = 0x100010;
    *(u32 *)&sprites[1]->w = 0x100010;
    *(u32 *)&sprites[2]->w = 0x80008;
    *(u32 *)&sprites[0]->cx = obj->field_40.word;
    sprites[0]->tpage = obj->texture_page;
    sprites[1]->tpage = sprites[2]->tpage = 0xB;
    p = (u8 *)gText_aChannels[obj->field_67].entry_begin;
#endif

    while (1) {
        f = p[GLYPH_FLAGS_OFFSET];
        if (!(f & 0x80)) {
            return;
        }
#ifdef VERSION_EUROPE
        if (!(f & 0x43)) {
            spr = sprites[0];
            spr->cx = 0x280;
            spr->cy = p[GLYPH_PALETTE_OFFSET] + 0xC0;
            spr->u = (p[GLYPH_INDEX_OFFSET] & 0x1F) * 8;
            spr->v = (u32)(p[GLYPH_INDEX_OFFSET] & 0xE0) >> 1;
        } else if (f & 0x40) {
            spr = sprites[1];
            *(u32 *)&spr->w = 0x100010;
            spr->tpage = 0xB;
            res[0] = p[GLYPH_INDEX_OFFSET] & 0x7F;
            if (res[0] >= 0x22) {
                s32 iconU = (u8)res[0];
                s32 targetU;
                targetU = 0x50;
                spr->v = 0x80;
                spr->cx = 0x280;
                spr->cy = 0xFC;
                do {
                } while (0);
                iconU *= 0x10;
                iconU -= 0x210;
                spr->u = iconU;
                if ((iconU & 0xFF) == targetU) {
                    spr->cx = 0x290;
                }
            } else {
                s32 paletteY;
                spr->u = (((u8)res[0] & 7) * 0x10) - 0x80;
                spr->v = ((u8)res[0] & 0x38) * 2;
                if (res[0] >= 0x19) {
                    res[0] = 0x18;
                }
                su = (((u16)res[0] & 0xF) * 0x10) + 0x280;
                paletteY = ((u8)res[0] >> 4) + 0xF9;
                spr->cx = su;
                spr->cy = paletteY;
                spr->attribute = obj->attribute | 0x08000000;
                if (p[GLYPH_INDEX_OFFSET] & 0x80) {
                    spr->tpage = 0x1E;
                    switch (res[0]) {
                    case 0x14:
                        spr->u = 0;
                        spr->cy = 0xE2;
                        break;
                    case 0x15:
                        spr->u = 0x40;
                        spr->cy = 0xE3;
                        break;
                    case 0x16:
                        spr->u = 0x60;
                        spr->cy = 0xE4;
                        break;
                    case 0x17:
                        spr->u = 0x20;
                        spr->cy = 0xE1;
                        break;
                    default:
                        spr->u = 0;
                        spr->cy = 0xE1;
                        break;
                    }
                    spr->v = 0x60;
                    *(u32 *)&spr->w = 0x100020;
                    spr->cx = 0x280;
                    spr->attribute |= 0x01000000;
                }
            }
        } else if (f & 2) {
            spr = sprites[2];
            *(u32 *)&spr->w = 0x10000C;
            spr->u = (u8)((u32)p[GLYPH_INDEX_OFFSET] % 21) * 12;
            {
                u32 glyph;
                /* Keep the boundary before the load, not around it: the
                 * palette store must fill the multiply-result latency. */
                do {
                } while (0);
                glyph = p[GLYPH_INDEX_OFFSET];
                tt = glyph / 21;
                spr->cx = 0x280;
                do {
                } while (0);
                spr->v = tt * 16 + 0x50;
            }
            spr->cy = p[GLYPH_PALETTE_OFFSET] + 0xC0;
        } else {
            s32 glyph;
            spr = sprites[2];
            *(u32 *)&spr->w = 0x80008;
            spr->cx = 0x280;
            spr->cy = p[GLYPH_PALETTE_OFFSET] + 0xC0;
            glyph = p[GLYPH_INDEX_OFFSET];
            if (glyph < 0xE0) {
                spr->u = (glyph & 0x1F) * 8;
                spr->v = ((u32)(p[GLYPH_INDEX_OFFSET] & 0xE0) >> 1) + 0x40;
            } else {
                spr->cy = 0xC7;
                *(u16 *)&spr->u = 0x40F8;
                if (glyph == 0xE1) {
                    *(u16 *)&spr->u = 0x48F8;
                }
            }
        }
#else
        if (f & 0x60) {
            if (f & 0x20) {
                res[0] = p[GLYPH_INDEX_OFFSET];
                spr = sprites[1];
                if (res[0] >= 0x22) {
                    tt = (u8)res[0];
                    spr->v = 0x80;
                    spr->cx = 0x200;
                    spr->cy = 0xFC;
                    tt *= 0x10;
                    tt -= 0x210;
                    spr->u = tt;
                    if ((tt & 0xFF) == 0x50) {
                        spr->cx = 0x210;
                    }
                } else {
                    spr->u = (((u8)res[0] & 7) * 0x10) - 0x80;
                    spr->v = ((u8)res[0] & 0x38) * 2;
                    if (res[0] >= 0x19) {
                        res[0] = 0x18;
                    }
                    su = (((u16)res[0] & 0xF) * 0x10) + 0x200;
                    tt = ((u8)res[0] >> 4) + 0xF9;
                    spr->cx = su;
                    spr->cy = tt;
                }
            } else {
                spr = sprites[2];
                spr->cx = (p[GLYPH_PALETTE_OFFSET] * 0x10) + 0x290;
                spr->cy = 0xFA;
                spr->u = ((p[GLYPH_INDEX_OFFSET] & 0xF) * 8) - 0x80;
                spr->v = (u32)(p[GLYPH_INDEX_OFFSET] & 0xF0) >> 1;
            }
        } else {
            spr = sprites[0];
            spr->u = (p[GLYPH_INDEX_OFFSET] & 0xF) * 0x10;
            spr->v = p[GLYPH_INDEX_OFFSET] & 0xF0;
        }
#endif
        spr->x = *(u16 *)(p + GLYPH_POSITION_OFFSET) + x;
        spr->y = *(u16 *)(p + GLYPH_POSITION_OFFSET + 2) + y;
        switch (p[GLYPH_MODE_OFFSET]) {
        case 1:
            if ((p[GLYPH_ROTATION_OFFSET + 2] |
                 (p[GLYPH_ROTATION_OFFSET] | p[GLYPH_ROTATION_OFFSET + 1])) != 0) {
#ifdef VERSION_EUROPE
                s8 bottomV;
                halfWidth = (u16)spr->w >> 1;
                halfHeight = (u16)spr->h >> 1;
                SetGeomOffset((s16)spr->x + halfWidth, (s16)spr->y + halfHeight);
                ft4->u0 = ft4->u2 = spr->u;
                ft4->u1 = ft4->u3 = spr->u + spr->w - 1;
                ft4->v0 = ft4->v1 = spr->v;
                bottomV = spr->v + (u8)spr->h - 1;
#else
                SetGeomOffset((s16)spr->x + 8, (s16)spr->y + 8);
                ft4->u0 = ft4->u2 = spr->u;
                ft4->u1 = ft4->u3 = spr->u + 0xF;
                ft4->v0 = ft4->v1 = spr->v;
                tt = spr->v + 0xF;
#endif
                mat->t[0] = 0;
                mat->t[1] = 0;
                mat->t[2] = 0x12C;
#ifdef VERSION_EUROPE
                ft4->v2 = ft4->v3 = bottomV;
#else
                ft4->v2 = ft4->v3 = tt;
#endif
                vec->vx = p[GLYPH_ROTATION_OFFSET] * 0x10;
                vec->vy = p[GLYPH_ROTATION_OFFSET + 1] * 0x10;
                vec->vz = p[GLYPH_ROTATION_OFFSET + 2] * 0x10;
                RotMatrixZYX_gte(vec, mat);
                GsSetLsMatrix(mat);
#ifdef VERSION_EUROPE
                vec[2].vx = vec[4].vx = halfWidth;
                vec[1].vx = vec[3].vx = -(s16)halfWidth;
                vec[1].vy = vec[2].vy = -(s16)halfHeight;
                vec[3].vy = vec[4].vy = halfHeight;
                vec[1].vz = vec[2].vz = vec[3].vz = vec[4].vz = 0;
#else
                vec[1].vx = -8;
                vec[1].vy = -8;
                vec[1].vz = 0;
                vec[2].vx = 8;
                vec[2].vy = -8;
                vec[2].vz = 0;
                vec[3].vx = -8;
                vec[3].vy = 8;
                vec[3].vz = 0;
                vec[4].vx = 8;
                vec[4].vy = 8;
                vec[4].vz = 0;
#endif
                if (RotAverageNclip4(&vec[1], &vec[2], &vec[3], &vec[4],
                                     (long *)&ft4->x0, (long *)&ft4->x1,
                                     (long *)&ft4->x2, (long *)&ft4->x3,
                                     &res[0], &res[1], &res[2]) > 0) {
                    GsSortPoly(ft4, ot, pri);
                }
                break;
            }
            /* fallthrough */
        case 0:
            GsSortFastSprite(spr, ot, pri);
            break;
        case 2:
#ifdef VERSION_EUROPE
        {
            u8 uvEnd;
#endif
            gt4->r0 = gt4->g0 = gt4->b0 = p[GLYPH_SHADE_OFFSET];
            gt4->r1 = gt4->g1 = gt4->b1 = p[GLYPH_SHADE_OFFSET + 1];
            gt4->r2 = gt4->g2 = gt4->b2 = p[GLYPH_SHADE_OFFSET + 2];
            gt4->r3 = gt4->g3 = gt4->b3 = p[GLYPH_SHADE_OFFSET + 3];
            gt4->x0 = gt4->x2 = spr->x;
#ifdef VERSION_EUROPE
            gt4->x1 = gt4->x3 = spr->x + spr->w + p[GLYPH_NUDGE_OFFSET];
#else
            gt4->x1 = gt4->x3 = spr->x + (p[GLYPH_NUDGE_OFFSET] + 0x10);
#endif
            gt4->y0 = spr->y + ((u8)p[GLYPH_NUDGE_OFFSET] >> 2);
            gt4->y1 = spr->y;
#ifdef VERSION_EUROPE
            gt4->y2 = gt4->y3 = spr->y + spr->h;
#else
            gt4->y2 = gt4->y3 = spr->y + 0x10;
#endif
            gt4->u0 = gt4->u2 = spr->u;
            GLYPH_UV_END = spr->u + GLYPH_UV_WIDTH;
            gt4->u1 = gt4->u3 = GLYPH_UV_END;
            if (!(GLYPH_UV_END & 0xFF)) {
                gt4->u3 = 0xFF;
                gt4->u1 = 0xFF;
            }
            gt4->v0 = gt4->v1 = spr->v;
            GLYPH_UV_END = spr->v + GLYPH_UV_HEIGHT;
            gt4->v2 = gt4->v3 = GLYPH_UV_END;
            if (!(GLYPH_UV_END & 0xFF)) {
                gt4->v3 = 0xFF;
                gt4->v2 = 0xFF;
            }
            gt4->tpage = spr->tpage | 0x20;
            gt4->clut = (spr->cy << 6) | (((u16)spr->cx >> 4) & 0x3F);
            GsSortPoly(gt4, ot, pri);
            gt4->tpage = spr->tpage | 0x40;
            gt4->clut = GLYPH_SECOND_CLUT;
            GsSortPoly(gt4, ot, pri);
            break;
#ifdef VERSION_EUROPE
        }
        case 3:
            attribute = spr->attribute & 0x8FFFFFFF;
            spr->r = spr->g = spr->b = p[GLYPH_TINT_OFFSET];
            spr->attribute = attribute | 0x50000000;
            GsSortFastSprite(spr, ot, pri);
            spr->attribute = attribute | 0x60000000;
            spr->cx = 0x2A0;
            spr->cy = 0xFF;
            GsSortFastSprite(spr, ot, pri);
            break;
#endif
        }
        p += GLYPH_RECORD_SIZE;
    }
}
#else

void TextBox_DrawEntries(DisplayObject *obj, GsOT *ot)
{
    s32 x;
    s32 y;
    MATRIX *mat;
    s32 pri;
    DuelEffectEntry *e;
    POLY_FT4 *ft4;
    SVECTOR *vec;
    long *res;
    GsSPRITE *sprites[3];
    GsSPRITE *spr;
    POLY_GT4 *gt4;
    s32 nudge;
    s32 t;
    s32 i;
    s32 w;
    s32 su;
    s32 cyv;
    s32 c;
    u8 f;
    u32 b;
    s32 v78;
    s32 tt;
    s32 hw;

    SetGeomScreen(0x12C);
    x = (s16)obj->xy.h.x;
    y = (s16)obj->xy.h.y;
    pri = (s16)obj->ot_depth;
    if (!(obj->flags & 8)) {
        x = x - gGraphics_sViewportX;
        y = y - gGraphics_sViewportY;
    }
    ft4 = (POLY_FT4 *)0x1F800038;
    vec = (SVECTOR *)0x1F800060;
    mat = (MATRIX *)0x1F800078;
    res = (long *)0x1F8000A0;
    gt4 = (POLY_GT4 *)0x1F800000;
    sprites[0] = (GsSPRITE *)0x1F8000C0;
    sprites[1] = (GsSPRITE *)0x1F800100;
    sprites[2] = (GsSPRITE *)0x1F800140;
    SetPolyGT4((POLY_GT4 *)0x1F800000);
    SetSemiTrans((void *)0x1F800000, 1);
    *(u32 *)&ft4->r0 = 0x808080;
    setlen(ft4, 9);
    setcode(ft4, 0x2C);
    ft4->clut = (obj->field_40.clut.y << 6) |
                (((u16)obj->field_40.clut.x >> 4) & 0x3F);
    t = (obj->attribute >> 0x17) & 0x60;
    w = obj->attribute & 0x01000000;
    do {
        b = obj->texture_page;
    } while (0);
    if (w) {
        b |= 0x80;
    }
    ft4->tpage = t | b;
    sprites[0]->attribute = sprites[1]->attribute = sprites[2]->attribute =
        obj->attribute | 0x08000000;
    GS_SPRITE_COLOR_WORD(sprites[0]) = obj->rgb;
    GS_SPRITE_COLOR_WORD(sprites[1]) = obj->rgb;
    GS_SPRITE_COLOR_WORD(sprites[2]) = obj->rgb;
    *(u32 *)&sprites[0]->w = 0x100010;
    *(u32 *)&sprites[1]->w = 0x100010;
    *(u32 *)&sprites[2]->w = 0x80008;
    *(u32 *)&sprites[0]->cx = obj->field_40.word;
    sprites[0]->tpage = obj->texture_page;
    sprites[1]->tpage = sprites[2]->tpage = 0xB;
    e = gText_aChannels[obj->field_67].entry_begin;
    v78 = 0x78;

    while (1) {
        f = e->entry_flags;
        c = e->glyph_code;
        nudge = 0;
        if (!(f & 0x80)) {
            return;
        }
        if (f & 0x60) {
            if (f & 0x20) {
                sprites[1]->tpage = 0xB;
                res[0] = e->glyph_index;
                spr = sprites[1];
                if (res[0] >= 0x22) {
                    tt = (u8)res[0];
                    spr->v = 0x80;
                    spr->cx = 0x200;
                    spr->cy = 0xFC;
                    tt *= 0x10;
                    tt -= 0x210;
                    spr->u = tt;
                    if ((tt & 0xFF) == 0x50) {
                        spr->cx = 0x210;
                    }
                } else {
                    spr->u = (((u8)res[0] & 7) * 0x10) - 0x80;
                    spr->v = ((u8)res[0] & 0x38) * 2;
                    if (res[0] >= 0x19) {
                        res[0] = 0x18;
                    }
                    su = (((u16)res[0] & 0xF) * 0x10) + 0x200;
                    tt = ((u8)res[0] >> 4) + 0xF9;
                    spr->cx = su;
                    spr->cy = tt;
                    spr->attribute = obj->attribute | 0x08000000;
                    spr->w = 0x10;
                    spr->h = 0x10;
                    if ((u8)e->icon_variant >= 0x14 && (u8)e->glyph_index < 0x18) {
                        sprites[1]->tpage = 0x1E;
                        switch (e->icon_variant) {
                        case 0x14:
                            cyv = 0xF2;
                            spr->u = 0;
                            break;
                        case 0x15:
                            spr->u = 0x40;
                            cyv = 0xF3;
                            break;
                        case 0x16:
                            spr->u = 0x60;
                            cyv = 0xF4;
                            break;
                        case 0x17:
                            spr->u = 0x20;
                            cyv = 0xF1;
                            break;
                        default:
                            cyv = 0xF1;
                            spr->u = 0;
                            break;
                        }
                        spr->cy = cyv;
                        spr->v = 0x60;
                        spr->w = 0x20;
                        spr->h = 0x10;
                        spr->cx = 0x100;
                        spr->attribute |= 0x01000000;
                    }
                }
            } else {
                spr = sprites[2];
                spr->cx = (e->palette_index * 0x10) + 0x290;
                spr->cy = 0xFA;
                spr->u = ((e->glyph_index & 0xF) * 8) - 0x80;
                spr->v = (u32)(e->glyph_index & 0xF0) >> 1;
            }
            spr->x = (u16)e->x_0C + x;
            spr->y = (u16)e->y_0E + y;
        } else {
            spr = sprites[0];
            switch (c) {
            case 0x8171:
            case 0x8173:
                c = 0x8183;
                break;
            case 0x8172:
            case 0x8174:
                c = 0x8184;
                break;
            }
            if ((u32)(c - 0x824F) < 0x4C) {
                if ((u32)(c - 0x8259) < 7 || (u32)(c - 0x827A) < 7) {
                    if (e->layout_selector == 1) {
                        spr->u = 0;
                        spr->v = v78;
                    } else {
                        spr->u = 0;
                        spr->v = 0x30;
                    }
                } else {
                if (e->layout_selector == 1) {
                    spr->u = (c & 0xF) * 0x10;
                    spr->v = (((s32)(c - 0x8240) >> 4) * 0x10) + 0x48;
                } else {
                    spr->u = (c & 0xF) * 8;
                    spr->v = ((s32)(c - 0x8240) >> 4) * 0xC;
                }
                }
            } else {
                {
                s32 tbl[30] = {
                    0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166, 0x8169,
                    0x816A, 0x8196, 0x817B, 0x8143, 0x817C, 0x8144, 0x815E, 0x8146,
                    0x8147, 0x8183, 0x8181, 0x8184, 0x8148, 0x8140, 0x83BF, 0x83C0,
                    0x81C1, 0x81A9, 0x81A8, 0x81BC, 0x81BD, -1,
                };

                for (i = 0;; i++) {
                    if (c == tbl[i]) {
                        if (e->layout_selector == 1) {
                            if (i < 0xF) {
                                spr->u = i * 0x10;
                                spr->v = 0x48;
                            } else if (i < 0x16) {
                                su = (i * 0x10) - 0x160;
                                spr->u = su;
                                spr->v = 0x58;
                            } else {
                                switch (c) {
                                case 0x83BF:
                                    spr->u = 0xD0;
                                    spr->v = 0x98;
                                    break;
                                case 0x83C0:
                                    spr->u = 0xE0;
                                    spr->v = 0x98;
                                    break;
                                case 0x81C1:
                                    spr->u = 0xF0;
                                    spr->v = 0x98;
                                    break;
                                case 0x81A9:
                                    spr->u = 0xB0;
                                    spr->v = v78;
                                    break;
                                case 0x81A8:
                                    spr->u = 0xC0;
                                    spr->v = v78;
                                    break;
                                case 0x81BC:
                                    nudge = 2;
                                    spr->u = 0xE0;
                                    spr->v = v78;
                                    break;
                                case 0x81BD:
                                    nudge = -2;
                                    spr->u = 0xF0;
                                    spr->v = v78;
                                    break;
                                }
                            }
                        } else {
                            if (i < 0xF) {
                                spr->u = i * 8;
                                spr->v = 0;
                            } else if (i < 0x16) {
                                spr->u = (i * 8) - 0x30;
                                spr->v = 0xC;
                            } else {
                                switch (c) {
                                case 0x83BF:
                                    spr->u = 0x68;
                                    spr->v = 0x3C;
                                    break;
                                case 0x83C0:
                                    spr->u = 0x70;
                                    spr->v = 0x3C;
                                    break;
                                case 0x81C1:
                                    spr->u = v78;
                                    spr->v = 0x3C;
                                    break;
                                case 0x81BD:
                                case 0x81BC:
                                case 0x81A9:
                                case 0x81A8:
                                    spr->u = 0;
                                    spr->v = 0x30;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    if (tbl[i] < 0) {
                        if (e->layout_selector == 1) {
                            spr->u = 0;
                            spr->v = v78;
                        } else {
                            spr->u = 0;
                            spr->v = 0x30;
                        }
                        break;
                    }
                }
                }
            }
            spr->cx = 0x280;
            spr->cy = e->palette_index + 0xE8;
            if (e->layout_selector == 1) {
                hw = 0x10;
                spr->w = hw;
            } else {
                spr->w = 8;
                hw = 0xC;
            }
            spr->h = hw;
            if (e->layout_selector == 1) {
                if (nudge != 0) {
                    spr->x = (u16)e->x_0C + x + nudge;
                    spr->y = (u16)e->y_0E + y;
                } else {
                    spr->x = (u16)e->x_0C + x;
                    spr->y = (u16)e->y_0E + y;
                }
            } else if (e->layout_selector == 2) {
                spr->x = (u16)e->x_0C + x;
                spr->y = (u16)e->y_0E + y - 2;
            } else {
                spr->x = (u16)e->x_0C + x;
                spr->y = (u16)e->y_0E + y + 2;
            }
        }
        switch (e->render_mode) {
        case 0:
            GsSortFastSprite(spr, ot, pri);
            break;
        case 1:
            if ((e->field_08.rotation[2] |
                 (e->field_08.rotation[0] | e->field_08.rotation[1])) != 0) {
                SetGeomOffset((s16)spr->x + 8, (s16)spr->y + 8);
                ft4->u0 = ft4->u2 = spr->u;
                ft4->u1 = ft4->u3 = spr->u + 0xF;
                ft4->v0 = ft4->v1 = spr->v;
                tt = spr->v + 0xF;
                mat->t[0] = 0;
                mat->t[1] = 0;
                mat->t[2] = 0x12C;
                ft4->v2 = ft4->v3 = tt;
                vec->vx = e->field_08.rotation[0] * 0x10;
                vec->vy = e->field_08.rotation[1] * 0x10;
                vec->vz = e->field_08.rotation[2] * 0x10;
                RotMatrixZYX_gte(vec, mat);
                GsSetLsMatrix(mat);
                vec[1].vx = -8;
                vec[1].vy = -8;
                vec[1].vz = 0;
                vec[2].vx = 8;
                vec[2].vy = -8;
                vec[2].vz = 0;
                vec[3].vx = -8;
                vec[3].vy = 8;
                vec[3].vz = 0;
                vec[4].vx = 8;
                vec[4].vy = 8;
                vec[4].vz = 0;
                if (RotAverageNclip4(&vec[1], &vec[2], &vec[3], &vec[4],
                                     (long *)&ft4->x0, (long *)&ft4->x1,
                                     (long *)&ft4->x2, (long *)&ft4->x3,
                                     &res[0], &res[1], &res[2]) > 0) {
                    GsSortPoly(ft4, ot, pri);
                }
            } else {
                GsSortFastSprite(spr, ot, pri);
            }
            break;
        case 2:
            gt4->r0 = gt4->g0 = gt4->b0 = e->field_04.corner_shade[0];
            gt4->r1 = gt4->g1 = gt4->b1 = e->field_04.corner_shade[1];
            gt4->r2 = gt4->g2 = gt4->b2 = e->field_04.corner_shade[2];
            gt4->r3 = gt4->g3 = gt4->b3 = e->field_04.corner_shade[3];
            gt4->x0 = gt4->x2 = spr->x;
            if (e->layout_selector == 1 || (e->entry_flags & 0x20)) {
                gt4->x1 = gt4->x3 = spr->x + (e->field_14 + 0x10);
                gt4->y0 = spr->y + ((u8)e->field_14 >> 2);
                gt4->y1 = spr->y;
                gt4->y2 = gt4->y3 = spr->y + 0x10;
                gt4->u0 = gt4->u2 = spr->u;
                su = spr->u + 0x10;
                gt4->u1 = gt4->u3 = su;
                if (!(su & 0xFF)) {
                    gt4->u3 = 0xFF;
                    gt4->u1 = 0xFF;
                }
                gt4->v0 = gt4->v1 = spr->v;
                su = spr->v + 0x10;
            } else {
                gt4->x1 = gt4->x3 = spr->x + (e->field_14 + 8);
                gt4->y0 = spr->y + ((u8)e->field_14 >> 2);
                gt4->y1 = spr->y;
                gt4->y2 = gt4->y3 = spr->y + 0xC;
                gt4->u0 = gt4->u2 = spr->u;
                su = spr->u + 8;
                gt4->u1 = gt4->u3 = su;
                if (!(su & 0xFF)) {
                    gt4->u3 = 0xFF;
                    gt4->u1 = 0xFF;
                }
                gt4->v0 = gt4->v1 = spr->v;
                su = spr->v + 0xC;
            }
            gt4->v2 = gt4->v3 = su;
            if (!(su & 0xFF)) {
                gt4->v3 = 0xFF;
                gt4->v2 = 0xFF;
            }
            gt4->tpage = spr->tpage | 0x20;
            if (e->entry_flags & 0x20) {
                gt4->clut = (spr->cy << 6) | (((u16)spr->cx >> 4) & 0x3F);
            } else {
                gt4->clut = ((e->palette_index + 0xE8) << 6) | 0x28;
            }
            GsSortPoly(gt4, ot, pri);
            gt4->tpage = spr->tpage | 0x40;
            gt4->clut = 0x3FE2;
            GsSortPoly(gt4, ot, pri);
            break;
        }
        e++;
    }
}
#endif

#include "duel_effect_entry_state.h"
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
#endif
#ifdef VERSION_JAPAN
#include "../psyq/libapi.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
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

    rec = gText_aGlyphRecordBitmaps;
    key = gText_aGlyphRecordKeys;

    for (;;) {
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
    }
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
    entry = (EuropeanDuelEffectEntry *)channel->entry_write_cursor;
    entry->owner_channel_tag = channel->channel_index + 1;
    entry->handler_state = 1;
    entry->render_mode = 0;
    entry->local_x = channel->pen_x;
    entry->local_y = channel->pen_y;
    entry->glyph_code = tagged_value;
    flags = channel->flags_34;
    result = 0;

    if (flags & 0x100) {
        entry->pad_0D[1] = tagged_value;
        entry->entry_flags = 0xC0;
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
    entry->entry_flags = mode | 0x80;

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
    entry->entry_flags = 0;
    channel->entry_write_cursor = (DuelEffectEntry *)entry;
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

    q = p->entry_write_cursor;
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
        q->entry_flags = 0xA0;
#ifndef VERSION_JAPAN
        q->icon_variant = b;
#endif
    } else if (f & 0x100) {
        v = (a >> 20) & 0xFF;
        if (v == 0) {
            return;
        }
        q->glyph_index = v;
        q->entry_flags = 0xC0;
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
            r = (s16 *)(p->output_count * 0x88 + (u8 *)r);
        }
        q->glyph_index = v;
        DuelEffect_RasterizeGlyph(p, a, v, r + 4);
#else
        if (f & 0x200) {
            r = (s16 *)(p->output_count * 0x88 + (u8 *)r);
        }
        q->glyph_index = 0;
        b = p->color_index_54;
#endif
        q->entry_flags = 0x80;
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
    q->entry_flags = 0;
    p->entry_write_cursor = q;
}
#endif

int TextStream_ReadU16LE(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *current = *stream;
    *stream = current + 2;
    return current[0] | (current[1] << 8);
}

u32 TextStream_ReadU32LE(TextStreamOwner *object)
{
    u8 **stream = &object->streams[object->stream_index];
    u8 *current = *stream;

    *stream = current + 4;
    return (current[3] << 24) | (current[2] << 16) |
           (current[1] << 8) | current[0];
}

#include "duel_effect_entry_state.h"
#include "display_object.h"
#include "duel_effect_entry_ranges.h"
#include "dialog_choice_control.h"
#include "dialog_choice.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s8 gDialog_bChoice;
u8 gDialog_bChoiceControl;

#define DUEL_EFFECT_UNSIGNED_HALFWORD(field) (*(u16 *)&(field))

/* Lays out the choice widget's six x/y pairs on the display object the
   channel holds at 0x30. The pairs sit at stride 8 from 0x28; three of them
   take the channel's rect_x as x and three take rect_x + rect_width, and
   the y values come from the entry's layout_selector modulo ten, which selects the
   dialog's row spacing. The record is the canonical DisplayObject:
   Dialog_UpdateChoice stores one there, through the same 0x30. */
#ifndef DUEL_EFFECT_LAYOUT_ROW_STEP
#define DUEL_EFFECT_LAYOUT_ROW_STEP 8
#endif

void DuelEffect_UpdateObjectLayout(DuelEffectChannel *p) {
    DisplayObject *q;
    s32 n;
    s32 m;
    u8 k;
    s32 a;
    s32 t;
    s16 t0;
    s16 t2;
    s32 b;
    s32 c;
    s32 v;
    s32 v0;
    s32 v2;
    s32 w;
    s32 x;
    s32 y;

    q = (DisplayObject *)p->prompt_object;
    n = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_x);
    q->field_48.vertex.x = n;
    q->field_38.h.field_38 = n;
    q->position.h.field_28 = n;
    m = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_x) +
        DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_width);
    q->field_50.h.field_50 = m;
    q->field_40.vertex.x = m;
    q->xy.h.x = m;

#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    /* The Japanese and European builds have one layout for every entry: no
       layout_selector % 10 selection, and one y step from the choice row (8 in the
       Japanese build, 10 in the European one, whose wrapper defines its own)
       rather than the US spacings below. */
    v = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_y) + (gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_LAYOUT_MASK) +
        ((s32)(*(u8 *)&gDialog_bChoice << 24) >> 20);
    q->position.h.field_2A = v;

    do {
        w = v;
    } while (0);
    q->xy.h.y = v;
    v = v + DUEL_EFFECT_LAYOUT_ROW_STEP;
    w = w + 0x10;
    q->field_40.vertex.y = v;
    q->field_38.h.field_3A = v;
    q->field_50.h.field_52 = w;
    q->field_48.vertex.y = w;
#else
    k = gDuelEffect_aEntries[
        gDuelEffect_awEntryRangeBoundaries[p->channel_index]
    ].layout_selector % 10;

    if (k == 1) {
        t = gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_LAYOUT_MASK;
        a = t / 16 * 0x10;
        v = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_y) +
            a + gDialog_bChoice * 0x10;
        q->position.h.field_2A = v;

        do {
            w = v;
        } while (0);
        q->xy.h.y = v;
        v = v + 0x10;
        w = w + 0x10;
        q->field_40.vertex.y = v;
        q->field_38.h.field_3A = v;
        q->field_50.h.field_52 = w;
        q->field_48.vertex.y = w;
        return;
    }

    if (k == 2) {
        t2 = gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_LAYOUT_MASK;
        b = t2 / 16 * 0xC;
        v2 = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_y) +
             b + gDialog_bChoice * 0xC;
        x = v2 - 2;
        y = v2 + 6;
    } else if (k == 0) {
        t0 = gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_LAYOUT_MASK;
        c = t0 / 16 * 0xC;
        v0 = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_y) +
             c + gDialog_bChoice * 0xC;
        x = v0 + 2;
        y = v0 + 0xA;
    } else {
        return;
    }

    q->xy.h.y = x;
    q->position.h.field_2A = x;
    x = x + 0xC;
    q->field_40.vertex.y = y;
    q->field_38.h.field_3A = y;
    q->field_50.h.field_52 = x;
    q->field_48.vertex.y = x;
#endif
}

/* Initialized data this unit owns at 0x80090E58: one start per duel-effect channel followed
   by the common end, then one retail padding halfword. The Japanese build's
   entry pool holds 300 entries, split 55/100/100/45, and the European
   build's 800, split 280/220/220/80. */
u16 gDuelEffect_awEntryRangeBoundaries[] = {
#ifdef VERSION_JAPAN
    0,
    55,
    155,
    255,
    300,
    0,
#elif defined(VERSION_EUROPE)
    0,
    280,
    500,
    720,
    800,
    0,
#else
    0,
    255,
    415,
    575,
    DUEL_EFFECT_ENTRY_COUNT,
    0,
#endif
};

#include "dialog_choice.h"
#include "display_object.h"
#include "input.h"
#include "sound.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s8 gDialog_bChoiceCount;
u8 gDialog_bChoiceEnabled;

void Dialog_HighlightChoice(DuelEffectChannel *record)
{
    s32 value;
    DisplayObject *sub = record->prompt_object;

    if (((gDialog_bChoiceEnabled >> gDialog_bChoice) & 1) == 0) value = 0xC0;
    else value = 0xC0C0;
    sub->rgb = value;
    record->prompt_object->field_3C.word = value;
    record->prompt_object->field_44.word = value;
    value = value / 4;
    record->prompt_object->field_2C.word = value;
    record->prompt_object->field_34.word = value;
    record->prompt_object->field_4C = value;
    *(s32 *)&record->prompt_object->field_54 = value;
    DuelEffect_UpdateObjectLayout(record);
}

/* Up, down or R1 on the repeat pad moves the dialog's choice cursor. R1
   wraps to the top; the directions clamp and report the press as handled
   without re-playing the cursor sound.

   The Spanish build also drops R1 from the previous-held word before the
   tests (DIALOG_READ_CHOICE_CLEARS_HELD_R1).

   gInput_awPadRepeat[0] is volatile: the target reads it once per test rather
   than caching it, and the three reads are what the three branches turn on. */
s32 Dialog_ReadChoiceInput(DuelEffectChannel *record)
{
    if (gInput_awPadRepeat[0] &
        (PAD_DIRECTION_VERTICAL_MASK | PAD_BUTTON_R1)) {
#ifdef DIALOG_READ_CHOICE_CLEARS_HELD_R1
        gInput_dwPreviousHeld &= ~PAD_BUTTON_R1;
#endif
        if (gInput_awPadRepeat[0] & PAD_BUTTON_R1) {
            u8 choice = gDialog_bChoice;
            s32 count = gDialog_bChoiceCount;

            gDialog_bChoice = choice + 1;
            if ((s8)(choice + 1) >= count) {
                gDialog_bChoice = 0;
            }
        } else if (gInput_awPadRepeat[0] & PAD_DIRECTION_DOWN) {
            u8 choice = gDialog_bChoice;
            s32 count = gDialog_bChoiceCount;

            gDialog_bChoice = choice + 1;
            if ((s8)(choice + 1) >= count) {
                gDialog_bChoice = choice;
                return 1;
            }
        } else {
            u8 choice = gDialog_bChoice;

            gDialog_bChoice = choice - 1;
            if ((s8)(choice - 1) < 0) {
                gDialog_bChoice = choice;
                return 1;
            }
        }
        SD_SEPlayFull(6);
        Dialog_HighlightChoice(record);
        return 1;
    }
    return 0;
}

#include "display_object.h"

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->rgb)

#ifndef WIDGET_PULSE_BASE
#define WIDGET_PULSE_BASE 0x80
#endif

void Widget_UpdatePulseColour(DisplayObject *object)
{
    /* A copy, not the parameter: retail moves the object into a3 at entry
       and writes the four dimmed words through that copy. */
    DisplayObject *q = object;
    s32 c = 0;
    s32 t = gMain_dwFrameCounter & 0x7F;
    if (t >= 0x40)
        t = 0x7F - t;
    t = t * 2 + WIDGET_PULSE_BASE;
    if (DISPLAY_OBJECT_COLOR_BYTES(q)[0] != 0)
        c = t;
    if (DISPLAY_OBJECT_COLOR_BYTES(q)[1] != 0)
        c |= t << 8;
    if (DISPLAY_OBJECT_COLOR_BYTES(q)[2] != 0)
        c |= t << 16;
    q->field_3C.word = c;
    q->field_44.word = c;
    c = (c & 0xFCFCFC) / 4;
    q->field_2C.word = c;
    q->field_34.word = c;
    q->field_4C = c;
    *(s32 *)&q->field_54 = c;
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "input.h"
#include "sound.h"
#include "dialog_choice_control.h"
#include "dialog_choice.h"

/* Small global another unit owns, declared here as a common symbol the
 * way the original read it through $gp. */
u8 gDialog_bInputState;

/* The confirm buttons. The Japanese release swaps Cross and Circle, so a
 * regional build names its own. */
#ifndef DIALOG_UPDATE_CHOICE_CONFIRM_MASK
#define DIALOG_UPDATE_CHOICE_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

/* Regional value: the channel flag that makes this update return at once,
   4 in the US build and 8 in the European one (Dialog_UpdateOpenTransition sets and
   clears the same bit, as DIALOG_TRANSITION_CHANNEL_FLAG). */
#ifndef DIALOG_UPDATE_CHOICE_SKIP_FLAG
#define DIALOG_UPDATE_CHOICE_SKIP_FLAG 4
#endif

void Dialog_UpdateChoice(DuelEffectChannel *p) {
    DisplayObject *e;
    s32 f;
    s32 g;
    s32 m;

    if ((p->playback_state & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        p->playback_state = p->playback_state | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        e = DisplayObject_AcquireSlot((s32)DisplayObject_FindFreeSlot(), 4);
        DisplayObject_InitializeGouraudQuad(e, 1);
        DisplayObject_SelectOrderingTable1(e);
        DisplayObject_SetDepthOffset(e,
                      (s8)(p->render_object->depth_offset + 1));
        p->prompt_object = e;
        *(s32 *)&e->field_54 = 0x2000;
        e->field_4C = 0x2000;
        e->field_34.word = 0x2000;
        e->field_2C.word = 0x2000;
        e->field_44.word = 0xC000;
        e->field_3C.word = 0xC000;
        e->update = (DisplayObjectCallback)Widget_UpdatePulseColour;
        e->attribute = e->attribute | (GsALON | GsAONE);
        Dialog_HighlightChoice(p);
    }

    if ((p->flags_34 & DIALOG_UPDATE_CHOICE_SKIP_FLAG) != 0) {
        return;
    }

    f = *(s8 *)&gDialog_bInputState;
    g = gDialog_bInputState;

    if (f != 0) {
        if ((g & DIALOG_CHOICE_INPUT_CONFIRMED) != 0) {
            gDialog_bInputState = g & 0xBF;
            gDialog_bChoice = g & 7;
            Dialog_HighlightChoice(p);
            return;
        }
        if ((f & DIALOG_CHOICE_INPUT_CANCELLED) == 0) {
            return;
        }
        gDialog_bInputState = 0;
    } else {
        if (Dialog_ReadChoiceInput(p) != 0) {
            return;
        }
        if ((gInput_awPadPressed[0] & DIALOG_UPDATE_CHOICE_CONFIRM_MASK) == 0) {
            return;
        }
    }

    m = 1 << gDialog_bChoice;
    if ((gDialog_bChoiceEnabled & m) == 0) {
        SD_SEPlayFull(9);
        return;
    }

    p->playback_state = 0;
    SD_SEPlayFull(7);
    if ((gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_RETAIN_HIGHLIGHT) == 0) {
        DisplayObject_ReleaseIfPresent(p->prompt_object);
        p->prompt_object = 0;
        p->playback_state = 3;
    }
}

#include "duel_effect_entry_state.h"

#ifdef VERSION_JAPAN

#define DUEL_EFFECT_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_ENTRIES ((JapaneseDuelEffectEntry *)gDuelEffect_aEntries)
#define DUEL_EFFECT_RANGE_START(channel) \
    (((JapaneseDuelEffectEntryRange *)(channel))->entry_range_start)
#define DUEL_EFFECT_RANGE_COUNT(channel) \
    (((JapaneseDuelEffectEntryRange *)(channel))->entry_range_count)
#elif defined(VERSION_EUROPE)

/* The European entries are 0x16 bytes, with handler_state and render_mode at 0x11
   and 0x13. */
#define DUEL_EFFECT_ENTRY_TYPE EuropeanDuelEffectEntry
#define DUEL_EFFECT_ENTRIES ((EuropeanDuelEffectEntry *)gDuelEffect_aEntries)
#define DUEL_EFFECT_RANGE_START(channel) ((channel)->entry_range_start)
#define DUEL_EFFECT_RANGE_COUNT(channel) ((channel)->entry_range_count)
#define DUEL_EFFECT_FIELD_13_OFFSET 0x11
#define DUEL_EFFECT_FIELD_15_OFFSET 0x13
#else
#define DUEL_EFFECT_ENTRY_TYPE DuelEffectEntry
#define DUEL_EFFECT_ENTRIES gDuelEffect_aEntries
#define DUEL_EFFECT_RANGE_START(channel) ((channel)->entry_range_start)
#define DUEL_EFFECT_RANGE_COUNT(channel) ((channel)->entry_range_count)
#endif

#ifndef DUEL_EFFECT_FIELD_13_OFFSET
#define DUEL_EFFECT_FIELD_13_OFFSET 0x13
#endif
#ifndef DUEL_EFFECT_FIELD_15_OFFSET
#define DUEL_EFFECT_FIELD_15_OFFSET 0x15
#endif

#define DUEL_EFFECT_ENTRY_FROM_FIELD_13(field) \
    ((DUEL_EFFECT_ENTRY_TYPE *)((field) - DUEL_EFFECT_FIELD_13_OFFSET))
#define DUEL_EFFECT_ENTRY_FROM_FIELD_15(field) \
    ((DUEL_EFFECT_ENTRY_TYPE *)((field) - DUEL_EFFECT_FIELD_15_OFFSET))
#define DUEL_EFFECT_ENTRY_BYTES(entry) ((u8 *)(entry))

/* Starting from the record's entry range, scans its active entries;
   returns 1 on the first entry with DUEL_EFFECT_ENTRY_FLAG_ACTIVE set and
   handler_state nonzero, 0
   if that flag clears, the count runs out, or the range is empty. */
int DuelEffect_HasActiveEntry(DuelEffectChannel *a0) {
    int v0;
    int count;
    DUEL_EFFECT_ENTRY_TYPE *entry;

    v0 = DUEL_EFFECT_RANGE_START(a0);
    count = DUEL_EFFECT_RANGE_COUNT(a0);
    entry = &DUEL_EFFECT_ENTRIES[v0];
    for (; count != 0; count--, entry++) {
        if ((entry->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) == 0) {
            return 0;
        }
        if (entry->handler_state != 0) {
            return 1;
        }
    }
    return 0;
}

/* Starting from the record's entry range, walks its active entries;
   for each active entry, writes a1 to handler_state and a2 to render_mode,
   stopping at the first entry with that flag clear or when the count runs
   out. */
void DuelEffect_SetEntryControlBytes(DuelEffectChannel *a0, u8 a1, u8 a2) {
    int start;
    int count;
    DUEL_EFFECT_ENTRY_TYPE *entry;

    start = DUEL_EFFECT_RANGE_START(a0);
    /* Empty statement where a release-stripped debug hook plausibly stood; it
       keeps the count load after the start load, so count reuses a0. */
    do { } while (0);
    count = DUEL_EFFECT_RANGE_COUNT(a0);
    entry = &DUEL_EFFECT_ENTRIES[start];
    for (; count != 0; count--, entry++) {
        if ((entry->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) == 0) {
            return;
        }
        entry->handler_state = a1;
        entry->render_mode = a2;
    }
}

void Text_CompletePageAdvance(DuelEffectChannel *object)
{
    u8 state = object->playback_state;
    DUEL_EFFECT_ENTRY_TYPE *entry;

    if ((state & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = state | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        DuelEffect_SetEntryControlBytes(object, 2, 0);
        return;
    }

    entry = &DUEL_EFFECT_ENTRIES[DUEL_EFFECT_RANGE_START(object)];
    if (entry->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
        return;
    }

    object->explicit_newline_count = 0;
    object->pen_x = 0;
    object->pen_y = 0;
    object->playback_state = 0;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    object->field_62 = 0;
#endif
}

#include "../psyq/rand.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "input.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "script_state.h"
#include "sound.h"
#include "sound_effect_request.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s16 gText_CdSoundPollBudget;
u8 gText_CdSoundWaitPhase;
u16 gText_ShakeViewportOrigin[2];
s16 gText_WaitCounter;
MenuRecord *gText_DisplayEffectRecord;
u8 gText_ScriptDispatchGate;
s8 gText_PageAdvanceHoldCountdown;

void Text_BeginPageClear(DuelEffectChannel *object)
{
    u8 flags = object->playback_state;

    if ((flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        DuelEffect_SetEntryControlBytes(object, 3, 0);
        object->playback_state = 0x82;
    }
}

DisplayObject *Dialog_OpenChoice(DuelEffectChannel *record)
{
    DisplayObject *cursor = DisplayObject_AcquireSlot(
        (s32)DisplayObject_FindFreeSlot(), 2
    );

    DisplayObject_ConfigureSpriteAtPosition(
        cursor,
        record->rect_x + record->rect_width - 0x10,
        record->rect_y + record->rect_height - 0x10,
        3,
        0,
        0,
        11,
        0x20C
    );
    cursor->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_SelectOrderingTable1(cursor);
    DisplayObject_SetDepthOffset(cursor, (s8)(record->depth_offset + 1));
    return cursor;
}

#ifndef DUEL_EFFECT_CHOICE_CONFIRM_MASK
#define DUEL_EFFECT_CHOICE_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void Text_WaitForPageAdvance(DuelEffectChannel*o){unsigned char f=o->playback_state;if((f&DUEL_EFFECT_STATE_FLAG_INITIALIZED)==0){o->playback_state=f|DUEL_EFFECT_STATE_FLAG_INITIALIZED;gText_PageAdvanceHoldCountdown=10;o->prompt_object=Dialog_OpenChoice(o);}else{if(gInput_awPadHeld[0]&PAD_BUTTON_SQUARE){gText_PageAdvanceHoldCountdown--;if(gText_PageAdvanceHoldCountdown<0)gText_PageAdvanceHoldCountdown=0;}else gText_PageAdvanceHoldCountdown=10;if(gText_PageAdvanceHoldCountdown!=0&&!(gInput_awPadPressed[0]&DUEL_EFFECT_CHOICE_CONFIRM_MASK))return;SD_SEPlayFull(11);o->playback_state=2;DisplayObject_ReleaseIfPresent(o->prompt_object);o->prompt_object=0;}}

void Text_StageScriptImage(DuelEffectChannel *state)
{
    s32 result;

    gScript_ViewportTargetY = 0;
    gScript_ViewportTargetX = 0;
    result = TextStream_ReadU16LE(state);
    gScript_ImageOperand = result;

    if (result & SCRIPT_IMAGE_OPERAND_HAS_VIEWPORT) {
        u8 **slot =
            &((TextStreamOwner *)state)->streams[state->stream_index];
        u8 *script = *slot;
        s32 value = *script;

        *slot = script + 1;
        gScript_ViewportTargetY = value;
        gScript_ViewportTargetX = TextStream_ReadU16LE(state);
    }

    gText_ScriptDispatchGate = 5;
    gScript_wCommandWord = 5;
    state->playback_state = 10;
}

void Text_StartViewportTween(DuelEffectChannel *object)
{
    s32 signed_value;
    s32 raw_value;

    object->playback_state = 0;
    *(s16 *)&gScript_ViewportTargetX = TextStream_ReadU16LE(object);
    *(s16 *)&gScript_ViewportTargetY = TextStream_ReadU16LE(object);
    gScript_SharedOperand = TextStream_ReadU16LE(object);

    signed_value = *(s16 *)&gScript_ViewportTargetY;
    raw_value = (u16)*(s16 *)&gScript_ViewportTargetY;
    if (signed_value >= 0x1000) {
        *(s16 *)&gScript_ViewportTargetY = raw_value - 0x1000;
        object->playback_state = 10;
    }

    gText_ScriptDispatchGate = 7;
    *(s16 *)&gScript_wCommandWord = 7;
}

void TextBox_WaitForScriptCompletion(DuelEffectChannel *object)
{
    if (gText_ScriptDispatchGate == 0) {
        object->playback_state = 0;
    }
}

void Text_ContinueDisplayEffect(DuelEffectChannel *channel) {
    u8 v = channel->playback_state;
    MenuRecord *p;

    if (!(v & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        channel->playback_state = v | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
    }

    p = gText_DisplayEffectRecord;

    if (p->display_effect_step != 0) {
        return;
    }

    {
        u8 w = channel->playback_state;

        if ((w & 0x40) || p->effect_id >= 0x41) {
            channel->playback_state = 0;

            return;
        }

        channel->playback_state = w | 0x40;
    }

    {
        MenuRecord *q = gText_DisplayEffectRecord;

        q->display_effect_step = *(u8 *)&q->field_40;
    }

    {
        MenuRecord *r = gText_DisplayEffectRecord;

        *(u16 *)&r->field_40 = 0x68;

        if (r->slot_index != 0) {
            *(u16 *)&r->field_40 = 0xD8;
        }
    }

    {
        MenuRecord *s = gText_DisplayEffectRecord;

        *(u16 *)&s->tween_target_y = 0xB2;
        s->field_44 = -0x10;
    }
}

/* Same playback_state/bit80 gating as Text_WaitForDisplayEffect, but additionally calls
   DisplayEffect_ReleaseRecord(gText_DisplayEffectRecord) before clearing playback_state when the display-effect
   step is zero. */

void Text_WaitForDisplayEffectAndRelease(DuelEffectChannel *object)
{
    u8 flags;
    MenuRecord *record;

    flags = object->playback_state;
    if (!(flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
    }
    record = gText_DisplayEffectRecord;
    if (record->display_effect_step == 0) {
        DisplayEffect_ReleaseRecord(record);
        object->playback_state = 0;
    }
}

void Text_WaitForDisplayEffect(DuelEffectChannel *object)
{
    u8 flags = object->playback_state;

    if ((flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
    }
    if (gText_DisplayEffectRecord->display_effect_step == 0) {
        object->playback_state = 0;
    }
}

void Text_RequestDisplayEffectFadeOut(DuelEffectChannel *object)
{
    u8 flags = gText_DisplayEffectRecord->variant_update_flags;

    if ((flags & 3) == 0) {
        gText_DisplayEffectRecord->variant_update_flags = flags | 0x10;
        gText_DisplayEffectRecord->display_effect_step = 6;
        object->playback_state = 8;
    }
}

void Text_RequestDisplayEffectCrossfade(DuelEffectChannel *object)
{
    u8 flags = gText_DisplayEffectRecord->variant_update_flags;

    if ((flags & 3) == 0) {
        gText_DisplayEffectRecord->variant_update_flags = flags | 0x10;
        gText_DisplayEffectRecord->display_effect_step = 4;
        object->playback_state = 8;
    }
}

void DuelEffect_WaitForFileTransfer(DuelEffectChannel *object)
{
    if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         gFile_dwCancelRequestState) == 0) {
        object->playback_state = 0;
    }
}

void Text_WaitForBgmFade(DuelEffectChannel *object)
{
    if (SD_IsBgmFadeActive() != 1) {
        object->playback_state = 0;
    }
}

void Text_WaitForDelay(DuelEffectChannel *object)
{
    u8 flags = object->playback_state;

    if ((flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gText_WaitCounter = TextStream_ReadU16LE(object);
    }
    gText_WaitCounter--;
    if (gText_WaitCounter == 0) {
        object->playback_state = 0;
    }
}

void Text_WaitWithViewportShake(DuelEffectChannel *object)
{
    u8 flags = object->playback_state;

    if ((flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gText_WaitCounter = TextStream_ReadU16LE(object);
        gText_ShakeViewportOrigin[0] = *(u16 *)&gGraphics_sViewportX;
        gText_ShakeViewportOrigin[1] = *(u16 *)&gGraphics_sViewportY;
    }
    if (gGraphics_dwSyncedFrameCount & 1) {
        *(u16 *)&gGraphics_sViewportX = gText_ShakeViewportOrigin[0] + ((rand() & 7) - 4);
        *(u16 *)&gGraphics_sViewportY = gText_ShakeViewportOrigin[1] + ((rand() & 3) - 2);
    }
    gText_WaitCounter--;
    if (gText_WaitCounter == 0) {
        *(u16 *)&gGraphics_sViewportX = gText_ShakeViewportOrigin[0];
        *(u16 *)&gGraphics_sViewportY = gText_ShakeViewportOrigin[1];
        object->playback_state = 0;
    }
}

/* The CD-sound wait polls three shared phases with a byte watchdog.
   Phase transitions re-arm 0xFF and deliberately fall through. The optional
   signed phase-two budget counts qualifying polls, not restarted transfers. */
void Text_WaitForCdSoundTransfer(DuelEffectChannel *object)
{
    DuelEffectChannel *p = object;

    if ((p->playback_state & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        p->playback_state |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        p->delay_countdown = 0xFF;
        gText_CdSoundWaitPhase = 0;
        if (gText_CdSoundPollBudget != 0) {
            p->playback_state |= 0x40;
        }
    }

    p->delay_countdown--;

    if (p->delay_countdown != 0) {
        switch (gText_CdSoundWaitPhase) {
        case 0:
            if ((gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE) == 0) {
                return;
            }
            p->delay_countdown = 0xFF;
            gText_CdSoundWaitPhase = 1;
        case 1:
            if ((gFile_wTransferStepFlags & 0x4000) == 0) {
                return;
            }
            p->delay_countdown = 0xFF;
            gText_CdSoundWaitPhase = 2;
        case 2:
            if ((gFile_wTransferStepFlags & 0x4000) == 0) {
                break;
            }
            if ((p->playback_state & 0x40) == 0) {
                return;
            }
            gText_CdSoundPollBudget--;
            if (gText_CdSoundPollBudget > 0) {
                return;
            }
            break;
        }
    }

    p->playback_state = 0;
    p->delay_countdown = 1;
}

#include "dialog_choice_control.h"
#include "dialog_choice.h"
#include "text_stream_commands.h"

/* Small global another unit owns, declared here as a common symbol the
 * way the original read it through $gp. */
void (*gText_ChoiceLayoutCallback)(volatile DuelEffectChannel *object);

#define DUEL_EFFECT_SIGNED_HALFWORD(field) (*(s16 *)&(field))

s32 TextBox_WrapLineIfNeeded(DuelEffectChannel *object) {
    if (DUEL_EFFECT_SIGNED_HALFWORD(object->pen_x) >= object->rect_width) {
        object->pen_x = 0;
        object->pen_y += object->line_advance_y;
    }
    if (object->rect_height <
        DUEL_EFFECT_SIGNED_HALFWORD(object->pen_y) + object->line_advance_y) {
        object->pen_y -= object->line_advance_y;
        return 1;
    }
    return 0;
}

void Text_TryCompleteChoiceLayout(volatile DuelEffectChannel *object)
{
    u32 flags;
    u8 control;

    if (object->explicit_newline_count >= gDialog_bChoiceCount) {
        object->playback_state = 1;
        flags = object->flags_34;
        object->explicit_newline_count = 0;
        control = gDialog_bChoiceControl;
        gText_ChoiceLayoutCallback = 0;
        flags &= 0xEFFF;
        control &= DIALOG_CHOICE_CONTROL_LAYOUT_MASK;
        ((DuelEffectChannel *)object)->flags_34 = flags;
        if (control != 0)
            gDialog_bChoiceCount = 2;
    }
}

#include "text_constants.h"
#include "text_stream_commands.h"

/* Small global another unit owns, declared here as a common symbol the
 * way the original read it through $gp. */
volatile s32 gText_CommandDisposition;

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what makes the
   opcode store in TextBox_BuildStep a single %gp_rel instruction whose load-delay slot needs
   the retail nop. The address comes from c_symbols.ld, which overrides this
   common symbol, so no storage is allocated here.  volatile is what keeps the
   read-back after the store, which retail issues at every use. */
volatile u16 gText_CurrentStreamCode;

void Text_ExtendGlyphCode(TextStreamOwner *owner)
{
    volatile u16 *code = &gText_CurrentStreamCode;

    /* Combine the F0-F5 prefix with the next stream byte and publish the
     * extended code before signaling completion. */
    *code = ((*code - TEXT_SINGLE_BYTE_GLYPH_LIMIT) << 8)
        | *owner->streams[owner->stream_index]++;
    gText_CommandDisposition = -1;
}

#include "text_stream_commands.h"
#include "dialog_choice.h"

void Text_SetStateFromStream(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *current = *stream;
    u8 value = current[0];

    *stream = current + 1;
    object->playback_state = value;
    gText_CommandDisposition = 1;
}

#include "card_constants.h"
#include "duel_card.h"
#include "text_constants.h"
#include "text_string_bank_views.h"
#include "european_text_bank_views.h"
#include "text_encode_decimal_digits.h"

/* Zero-initialized storage this unit owns at 0x800EAFF8-0x800EB010 in the
 * US build. */
u16 gText_DecimalDigitGlyphMap[12];

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u16 gText_StagedStringId;
u8 gText_GuardianGlyphCode;
s16 gDuel_wSelectedCardID;
u8 gText_SelectedCardColorIndex;

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))

/* Entries 0 through 12 of the secondary text-command table gText_SecondaryCommandHandlers,
   the handlers the F8 escape reaches, together with Text_EmitSymbolGlyph, the
   helper two of them share -- which is not itself a table entry, so this is
   fourteen definitions rather than thirteen entries. Each entry takes the
   text channel and reads its operands from the channel's live stream. Entry
   13, Text_StartCampaignDuel, is next in both the table and the image. */

#ifdef VERSION_JAPAN
/* The Japanese card-text command: no field_62 (the Japanese channel record
   ends at 0x60), a +0x100 rather than +0xD100 id for op bit 6, the stars
   without the US type and 0x17 tests, an unconditional glyph for the plain
   case, and the two-bank string lookup of the Japanese Text_LookupString. */
void Text_ExpandSelectedCardInfo(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    s32 n;
    u8 *text;
    u8 *current;

    text = (u8 *)(s32)object->stream_index;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    if (op & 0x10) {
        object->color_index_54 = gText_SelectedCardColorIndex;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0x100;
    } else {
        id = 0;
        switch (op & 0xF) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_1_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_index++;
    n = id;
    if (id & TEXT_GLOBAL_STRING_ID_BASE) {
        text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                      gText_GlobalStringOffsets[id & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
    } else {
        if (id >= 0x500) {
            n = id - 0x100;
        }
        text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                      gText_PrimaryStringOffsets[n]);
    }
    TEXT_STREAM_OWNER(object)->streams[object->stream_index] = text;
    return;
plain:
    object->flags_34 |= 0x80;
    DuelEffect_AppendEntry(object, id);
    object->flags_34 &= 0xFF7F;
    object->pen_x += 0x10;
}
#elif defined(VERSION_EUROPE)
/* The European card-text command: the Japanese shape (no field_62 reset,
   +0x100 for op bit 6, the stars without the US type and 0x17 tests), a
   fourth selector (the card type with bit 7 set), the European string banks
   and the plain glyph flagged 0x100 rather than 0x80. */
void Text_ExpandSelectedCardInfo(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    u8 *text;
    u8 *current;

    text = (u8 *)(s32)object->stream_index;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    if (op & 0x10) {
        object->color_index_54 = gText_SelectedCardColorIndex;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0x100;
    } else {
        id = 0;
        switch (op & 0xF) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_1_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 3:
            id = ((gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                   CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) | 0x80;
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_index++;
    if (id <= 0x7FFF) {
        if (id >= 0x500) {
            s32 n = id - 0x100;

            text = (u8 *)(((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) +
                          gText_PrimaryStringOffsets[n]);
        } else {
            text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                          gText_PrimaryStringOffsets[id]);
        }
    } else {
        text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                      gText_GlobalStringOffsets[id - 0x8000]);
    }
    TEXT_STREAM_OWNER(object)->streams[object->stream_index] = text;
    return;
plain:
    object->flags_34 |= 0x100;
    DuelEffect_AppendEntry(object, id);
    object->flags_34 &= 0xFEFF;
    object->pen_x += 0x10;
}
#else
void Text_ExpandSelectedCardInfo(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    s32 n;
    s32 kind;
    s32 stats;
    s32 type;
    u8 *text;
    u8 *current;
    u8 **slot;

    text = (u8 *)(s32)object->stream_index;
    object->field_62 = 0;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    n = 0;
    if (op & 0x10) {
        object->color_index_54 = gText_SelectedCardColorIndex;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0xD100;
    } else {
        kind = op & 0xF;
        id = 0;
        switch (kind) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            stats = gDuel_adwCardStats[gDuel_wSelectedCardID - 1];
            id = (stats >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                 CARD_STAT_GUARDIAN_STAR_MASK;
            type = (stats >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            id += 0x17;
            if ((u32)(type - CARD_TYPE_MAGIC) < CARD_NON_MONSTER_TYPE_COUNT) {
                object->field_62 = type;
            }
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                 CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            if (id == 0x17) {
                n = 1;
            }
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_index++;
    n = id;
    if (id > 0xCFFF) {
        text = (u8 *)((u32)gText_PrimaryStringOffsets & 0xFFFF0000) +
               gText_PrimaryStringOffsets[id - 0xD000];
    } else if (id > 0x7FFF) {
        text = (u8 *)((u32)gText_GlobalStringOffsets & 0xFFFF0000) +
               gText_GlobalStringOffsets[id - 0x8000];
    } else {
        if (id >= 0x500) {
            n = id - 0x100;
        }
        text = (u8 *)((u32)gText_PrimaryStringAddressAnchor & 0xFFFF0000) + gText_PrimaryStringOffsets[n];
    }
store:
    slot = &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    *slot = text;
    return;
plain:
    object->flags_34 |= 0x80;
    if ((u8)n == 0) {
        DuelEffect_AppendEntry(object, id);
    }
    object->flags_34 &= 0xFF7F;
    object->pen_x += 0x10;
}
#endif

/* The channel flag bit Text_EmitSymbolGlyph holds across its call; 0x100 in the
 * European build, whose wrapper defines it. */
#ifndef DUEL_EFFECT_SELECTOR_FLAG
#define DUEL_EFFECT_SELECTOR_FLAG 0x80
#endif

void Text_EmitSymbolGlyph(DuelEffectChannel *object, s32 value)
{
    *(u8 *)&object->flags_34 = *(u8 *)&object->flags_34;
    object->flags_34 |= DUEL_EFFECT_SELECTOR_FLAG;
    DuelEffect_AppendEntry(object, value);
    object->flags_34 &= 0xFFFF ^ DUEL_EFFECT_SELECTOR_FLAG;
    object->pen_x += 0x10;
}

void Text_EmitGuardianStarGlyph(DuelEffectChannel *object)
{
    Text_EmitSymbolGlyph(object, gText_GuardianGlyphCode);
}

void Text_EmitNextSymbolGlyph(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];

    Text_EmitSymbolGlyph(object, *(*stream)++);
}

void Text_ResetPenXAndAdvanceY(DuelEffectChannel *object)
{
    register u8 **stream;
    register u8 *current;
    register u32 value;

    object->pen_x = 0;
    stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    current = *stream;
    value = current[0];
    current++;
    *stream = current;
    object->pen_y += (s8)value;
}

void Text_AdvancePenX(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    register u8 **slot = stream;
    register u8 *current = *slot;
    register u32 value = current[0];

    current++;
    *slot = current;
    object->pen_x += value;
}

void Text_PushFormattedDecimal(DuelEffectChannel *object)
{
    u8 buf[8];
    u8 *e;
    u8 *bp;
    s32 r;
    s32 c;
    s32 t;
    s32 k;
    s32 i;
    s32 h;
    s32 w;

    r = TextStream_ReadU32LE(TEXT_STREAM_OWNER(object));
    t = *TEXT_STREAM_OWNER(object)->streams[object->stream_index]++;
    c = t;
    Text_EncodeDecimalDigits(*(s32 *)r, c & 0xF, buf);

    h = 0;

    if ((c & 0x80) != 0) {
        if ((c & 0x40) == 0) {
            goto skip;
        }
        h = *(u16 *)&gText_DecimalDigitGlyphMap[0];
        e = object->decimal_buffer;
        goto write;
    }

    if (c < 2) {
        e = object->decimal_buffer;
        goto write;
    }

    bp = buf;
    k = c - 1;
    while (1) {
        if (bp[k] < TEXT_DECIMAL_RADIX) {
            break;
        }
        c = k;
        if (k < 2) {
            break;
        }
        k = c - 1;
    }

skip:
    e = object->decimal_buffer;

write:
    i = (c & 0xF) - 1;
    do {
        w = h;
        if (buf[i] < TEXT_DECIMAL_RADIX) {
            w = *(u16 *)&gText_DecimalDigitGlyphMap[buf[i]];
        }
        if (w >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
            *e = (w >> 8) - 0x10;
            e[1] = w;
            e += 2;
        } else {
            *e = w;
            e += 1;
        }
        i--;
    } while (i >= 0);

    *e = TEXT_STRING_TERMINATOR;
    object->stream_index++;
    TEXT_STREAM_OWNER(object)->streams[object->stream_index] = object->decimal_buffer;
}

/* Inlining keeps the stream value and channel in independent live ranges. */
static __inline__ u32 read_operand(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    u8 *cursor = *stream;
    u32 value = *cursor++;

    *stream = cursor;
    return value;
}

void Text_SetFontMode(DuelEffectChannel *object)
{
#ifdef VERSION_EUROPE
    u8 value;
#else
    u32 value;
#endif

#ifdef VERSION_EUROPE
    object->flags_34 &= 0xFFFC;
#else
    object->flags_34 &= 0xFEFF;
#endif
    value = read_operand(object);
#ifdef VERSION_JAPAN
    object->line_advance_y = value << 3;
    object->glyph_advance_x = value << 3;
#elif defined(VERSION_EUROPE)
    /* The European release has four text sizes and keeps the size in the
       channel's two low flag bits. */
    switch (value) {
    case 0:
        object->glyph_advance_x = 8;
        object->line_advance_y = 16;
        break;
    case 1:
        object->glyph_advance_x = 8;
        object->line_advance_y = 8;
        object->flags_34 |= 1;
        break;
    case 2:
        object->glyph_advance_x = 12;
        object->line_advance_y = 16;
        object->flags_34 |= 2;
        break;
    case 3:
        object->glyph_advance_x = 16;
        object->line_advance_y = 16;
        object->flags_34 |= 3;
        break;
    }
#else
    switch (value) {
    case 1:
        object->glyph_advance_x = 8;
        object->line_advance_y = 8;
        break;
    case 2:
        object->glyph_advance_x = 8;
        object->line_advance_y = 12;
        break;
    }
#endif
#ifndef VERSION_EUROPE
    if (value == 1)
        object->flags_34 |= 0x100;
#endif
}

void Text_SetGlyphCellSize(DuelEffectChannel *object)
{
    /* Separate lifetimes preserve allocation across the two stream reads. */
    {
        u8 **stream =
            &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->glyph_advance_x = value;
    }
    {
        u8 **stream =
            &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->line_advance_y = value;
    }
}

void Text_SetPenX(DuelEffectChannel *object)
{
    object->pen_x = TextStream_ReadU16LE(object);
}

#ifdef VERSION_EUROPE
/* A European-only command, next to Text_SetPenX in the image: the same
   read into field_3A. */
void Text_SetPenY(DuelEffectChannel *object)
{
    object->pen_y = TextStream_ReadU16LE(object);
}
#endif

void Text_SetOutputLimit(DuelEffectChannel *object)
{
#ifdef VERSION_EUROPE
    /* The European build clears the pixel counter and sets the limit
       (halfwords at 0x60 and 0x62) from one operand byte, times eight. */
    *(u16 *)&object->output_count = 0;
    *(u16 *)&object->field_62 =
        read_operand(object) << 3;
#else
    object->output_count = 0;
    object->output_limit = TextStream_ReadU16LE(object);
#endif
}

u8 **Text_PushStagedString(DuelEffectChannel *a0) {
    DuelEffectChannel *a3 = a0;
#ifdef VERSION_JAPAN
    s32 a2 = (s16)gText_StagedStringId;
#else
    s32 a2 = gText_StagedStringId;
#endif
    u32 v1;
    u8 counter;
    u8 **slot;

#ifdef VERSION_JAPAN
    if (a2 & TEXT_GLOBAL_STRING_ID_BASE) {
        v1 = ((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
             gText_GlobalStringOffsets[a2 & (TEXT_GLOBAL_STRING_ID_BASE - 1)];
    } else {
        if (a2 >= 0x500) {
            a2 -= 0x100;
        }
        v1 = ((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) | gText_PrimaryStringOffsets[a2];
    }
#elif defined(VERSION_EUROPE)
    /* The European banks, as in TextBox_BuildStep. */
    if (a2 <= 0x7FFF) {
        if (a2 >= 0x500) {
            s32 n = a2 - 0x100;

            v1 = ((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) + gText_PrimaryStringOffsets[n];
        } else {
            v1 = ((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) + gText_PrimaryStringOffsets[a2];
        }
    } else {
        v1 = ((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
             gText_GlobalStringOffsets[a2 - 0x8000];
    }
#else
    if (a2 > 0xCFFF) {
        v1 = ((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
             gText_PrimaryStringOffsets[a2 - 0xD000];
    } else if (a2 > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
        v1 = ((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
             gText_GlobalStringOffsets[a2 - TEXT_GLOBAL_STRING_ID_BASE];
    } else {
        if (a2 >= 0x500) {
            a2 -= 0x100;
        }
        v1 = ((u32)gText_PrimaryStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) + gText_PrimaryStringOffsets[a2];
    }
#endif

    counter = *(u8 *)&a3->stream_index + 1;
    *(u8 *)&a3->stream_index = counter;
    slot = &TEXT_STREAM_OWNER(a3)->streams[(s8)counter];
    *slot = (u8 *)v1;
    return slot;
}

void Text_SetColorIndex(DuelEffectChannel *object)
{
    u8 **slot =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    u8 *q = *slot;
    s32 v = *q;
    s32 w;

    *slot = q + 1;
    w = v;
    if (v & 0x80) {
        w = gText_abColorSlots[v & 0xF];
    }
    object->color_index_54 = w;
}

void Text_SetFastPrintMode(DuelEffectChannel *object)
{
    register DuelEffectChannel *obj;
    register u8 **stream;
    register u8 *current;
    register u32 value;

    obj = object;
    obj->flags_34 &= 0xEFFF;
    stream = &TEXT_STREAM_OWNER(obj)->streams[obj->stream_index];
    current = *stream;
    value = *current;
    current++;
    *stream = current;
    if (value)
        obj->flags_34 |= 0x1000;
}

#include "duel_side_state.h"
#include "ai_script.h"
#include "duel_session_controls.h"

#include "ai_opponent_data.h"
#include "duel_terrain_boost.h"
#include "main_mode_state.h"

#define TEXT_STREAM_OWNER_VIEW(object) ((TextStreamOwner *)(object))

/*
 * Each byte read owns its slot pointer, advancing cursor and unsigned value.
 * Those separate lifetimes recover all 352 bytes under uniform G0 without
 * register bindings. Sharing the three locals instead changes the index
 * register in all four reads (twelve words).
 *
 * The opponent is reloaded unsigned after the opaque word reader, then
 * explicitly narrowed for the two signed-byte equality tests.
 */
void Text_StartCampaignDuel(DuelEffectChannel *o)
{
    s32 x;
    gDuel_bSide0ControllerSelector[0] = -1;
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_bOpponentID = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_awCampaignSceneByOutcome[0] = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_awCampaignSceneByOutcome[1] = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_bTerrain = v;
    }
    gDuel_wBgmId = TextStream_ReadU16LE(o);
    gDuel_wAnimatedBattleBgmCommand = 0x72A0;
    x = (u8)gDuel_bOpponentID;
    if ((u32)(x - 9) < 8) gDuel_wAnimatedBattleBgmCommand = 0x7280;
    if ((s8)x == 0x11) gDuel_wAnimatedBattleBgmCommand = 0x7290;
    if ((s8)x == 0x26) gDuel_wAnimatedBattleBgmCommand = 0x72B0;
    gDuel_bPostDuelMainMode = 2;
    gDuel_bResumeFromAnimatedBattle = 0;
    gMain_bActiveModeState = 3;
}

/* Seven contiguous duel-effect command handlers from DuelEffect_PlayBgmCommand
 * (0x80038690) through Text_OpenCampaignMap. The four sound handlers read their
 * operands from the channel's selected stream and drive SD_BGMPlay,
 * SD_SEPlayFull or the BGM fade; then come the two empty table entries and
 * the handler that reads one byte into gCampaignMap_bEntryLocation and moves the scene mode
 * to 5. */

#include "sound.h"
#include "main_mode_state.h"
#include "sound_bgm_commands.h"

void DuelEffect_PlayBgmCommand(DuelEffectChannel *object)
{
    SD_BGMPlay((u16)TextStream_ReadU16LE(object));
}

void DuelEffect_ProcessBgmCommand(DuelEffectChannel *object)
{
    u8 **slot =
        &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *stream = *slot;
    s32 command = *stream;
    s32 op;

    *slot = stream + 1;
    op = command;
    if (op & 0x3F) {
        SD_BGMPlay(TextStream_ReadU16LE(object) & 0xFFFF);
    } else {
        if (op & 1) {
            SD_BGMPlay(gSD_dwSavedBgmCommand);
        }
        if (op & 2) {
            gSD_dwSavedBgmCommand = TextStream_ReadU16LE(object) & 0xFFFF;
        }
        if (op & 4) {
            gSD_dwSavedBgmCommand = gSD_dwCurrentBgmCommand;
        }
    }
    if (op & 0x80) {
        object->playback_state = 0xC;
        gText_CommandDisposition = 1;
    }
}

void DuelEffect_PlaySoundCommand(DuelEffectChannel *object) {
    u32 value = TextStream_ReadU16LE(object);

    value &= 0xFFFF;
    if (value & 0x8000) {
        SD_CdSoundPlay((u16)value);
        gText_CdSoundPollBudget = TextStream_ReadU16LE(object);
        object->playback_state = 0x11;
        gText_CommandDisposition = 1;
    } else {
        SD_SEPlayFull((u16)value);
    }
}

void DuelEffect_FadeOutBgmCommand(DuelEffectCommand *command)
{
    u8 **slot;
    u8 *cursor;
    u32 raw;
    u32 saved_raw;
    u32 opcode;
    u32 high_bit;

    slot = &((TextStreamOwner *)command)->streams[command->cursor_slot];
    cursor = *slot;
    raw = *cursor++;
    saved_raw = raw;
    opcode = saved_raw & 0x7F;
    *slot = cursor;

    if (opcode == 0) {
        SD_BGMFadeOut();
        goto shared_test;
    } else {
        SD_BGMFadeOutWithStep(opcode);
    }

shared_test:
    high_bit = saved_raw & 0x80;
    if (high_bit != 0) {
        command->command_state = 13;
        gText_CommandDisposition = 1;
    }
}

void Text_NoOpSecondary12(void)
{
}

void Text_NoOpSecondary13(void)
{
}

void Text_OpenCampaignMap(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 value = *(*stream)++;
    gMain_bActiveModeState = 5;
    gCampaignMap_bEntryLocation = value;
}

#include "fade.h"
#include "fade_overlay_controls.h"
#include "display_object_depth_bases.h"

void Text_ApplyFadeCommand(DuelEffectChannel *object)
{
    u8 **cursor = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *stream = *cursor;
    s32 command = *stream;
    s32 opcode;

    *cursor = stream + 1;
    opcode = command;
    if (opcode & 0x40) {
        gFade_bKeepOverlayDepth = *(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] + 9;
    }
    if (opcode & 0x20) {
        gFade_bKeepOverlayDepth = 4;
    }
    if (opcode & 0x10) {
        if (opcode & 1) {
            Fade_InitOutColor(0xFFFFFF);
        } else {
            Fade_InitInColor(0xFFFFFF);
        }
        gFade_State.step = 4;
    } else if (opcode & 1) {
        Fade_StartOutKeepOverlay();
    } else {
        Fade_StartInKeepOverlay();
    }
    if (opcode & 0x80) {
        Fade_Wait();
    }
}

#include "card_constants.h"
#include "campaign_flags.h"
/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gText_BranchSelectorB;
u8 gText_BranchSelectorA;

#ifdef VERSION_EUROPE
#include "mem_card.h"
#include "player_name_glyphs.h"

#endif

/* Regional value: the channel flag Text_DisableCompletionWait clears (8 -> 0x10). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_EFFECT_CLEARED_CHANNEL_FLAG
#define DUEL_EFFECT_CLEARED_CHANNEL_FLAG 8
#endif

void Text_DisableCompletionWait(DuelEffectChannel *value)
{
    value->flags_34 &= (u16)~DUEL_EFFECT_CLEARED_CHANNEL_FLAG;
}

#ifndef DUEL_EFFECT_STREAM_HIGH_MASK
#define DUEL_EFFECT_STREAM_HIGH_MASK 0xFFFF0000
#endif

void Text_JumpBySelectorA(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_index] += gText_BranchSelectorA * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_index] =
        (u8 *)(((u32)owner->streams[object->stream_index] &
                DUEL_EFFECT_STREAM_HIGH_MASK) |
               (value & 0xFFFF));
}

void Text_JumpBySelectorB(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_index] += gText_BranchSelectorB * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_index] =
        (u8 *)(((u32)owner->streams[object->stream_index] &
                DUEL_EFFECT_STREAM_HIGH_MASK) |
               (value & 0xFFFF));
}

void Text_UnlockDuelist(DuelEffectChannel *object)
{
    s16 duelist_id;
    u32 value;
    u8 **stream;
    u8 *cursor;

    stream = &((TextStreamOwner *)object)->streams[object->stream_index];
    cursor = *stream;
    value = *cursor++;
    duelist_id = value;
    *stream = cursor;
    if (duelist_id > 0) {
        Library_UpdateCardUsedFlag(
            duelist_id + CAMPAIGN_FLAG_DUELIST_DEFEATED_BASE);
        Library_UpdateCardUsedFlag(duelist_id + FREE_DUEL_UNLOCK_FLAG_BASE);
    }
}

void Text_CloseChoice(DuelEffectChannel *object)
{
    DisplayObject_ReleaseIfPresent(object->prompt_object);
    object->prompt_object = 0;
    object->playback_state = 2;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    object->field_62 = 0;
#endif
    gText_CommandDisposition = 1;
}

#ifdef VERSION_EUROPE
/* European only: pushes the player name's glyph codes, gText_aCurrentPlayerNameGlyphs, as the
   next text stream. */
void Text_PushPlayerNameStream(DuelEffectChannel *object)
{
    ((TextStreamOwner *)object)->streams[object->stream_index + 1] = gText_aCurrentPlayerNameGlyphs;
    object->stream_index++;
}
#endif

/* The text stream's primary control-byte handlers and the two text-box
   layout helpers after them, contiguous from Text_DispatchSecondaryCommand
   (0x80038B4C) through TextBox_RefreshDisplayObjects.

   The nine handlers are the ones TextBox_BuildStep reaches through
   gText_apfnPrimaryCommands for bytes F6 and F8 through FF: the F8 escape into the
   secondary table, the cursor set, the choice command, the page wait and
   campaign-flag command, the stream push, the new line and end of stream,
   and the display-effect command. See notes/text-control-bytecode.md. The
   other two primary handlers are Text_ExtendGlyphCode and
   Text_SetStateFromStream, which precede them in this file.

   The layout helpers are TextBox_UpdateOutline, which TextBox_SetPos calls to lay
   the box's second object out as an outline around the first, and
   TextBox_RefreshDisplayObjects, which TextBox_BuildStep calls to create or refresh the
   box's frame object. */

#include "duel_effect_command_table.h"
#include "text_stream_commands.h"
#include "dialog_choice.h"
#include "campaign_flags.h"
#include "display_effect_lifecycle.h"

#include "menu_record.h"
#include "display_menu_records.h"
#include "script_scene.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "color_constants.h"
#include "dialog_choice_control.h"

/* Zero-initialized storage this unit owns at 0x800EB010-0x800EB0F8 in the
 * US build. */
MenuRecord gDisplayEffect_aMenuRecords[3];

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
void (*gText_ChoiceLayoutCallback)(volatile DuelEffectChannel *object);

#ifndef TEXT_STREAM_CURSOR_HIGH_MASK
#define TEXT_STREAM_CURSOR_HIGH_MASK 0xFFFF0000
#endif

void Text_DispatchSecondaryCommand(DuelEffectChannel *object)
{
    u8 **pp = &TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_index];
    u8 *p = *pp;
    s32 op = *p;

    *pp = p + 1;
    gText_SecondaryCommandHandlers[op](object);
}

  void Text_SetCursorOffset(DuelEffectChannel *o){int v; u8 **p;v=TextStream_ReadU16LE(o);p=&TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];*p=(u8 *)(((u32)*p&TEXT_STREAM_CURSOR_HIGH_MASK)|(v&0xFFFF));}

void Text_HandleChoiceCommand(DuelEffectChannel *object)
{
    s32 t;
    s32 u;
    s32 w;
    s32 v;
    s32 c;
    s32 d;

    gText_CommandDisposition = 1;
    t = *TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_index]++;
    c = t;
    d = TEXT_CHOICE_OPTIONS_ENABLED_MASK;
    if (c & TEXT_CHOICE_COMMAND_HAS_OPTIONS) {
        u = *TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_index]++;
        t = u;
        d = t;
    }
    if (c & TEXT_CHOICE_COMMAND_BRANCH) {
        TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_index] +=
            gDialog_bChoice * 2;
        Text_SetCursorOffset(object);
    } else {
        gDialog_bChoiceCount = TEXT_CHOICE_COMMAND_COUNT_MASK;
        gDialog_bChoiceCount = c & gDialog_bChoiceCount;
        gDialog_bChoiceControl = c & DIALOG_CHOICE_CONTROL_MASK;
        gDialog_bChoiceEnabled = d & TEXT_CHOICE_OPTIONS_ENABLED_MASK;
        w = d & TEXT_CHOICE_OPTIONS_INPUT_FLAG;
        gDialog_bChoice = 0;
        gDialog_bInputState = 0;
        if (w != 0) {
            gDialog_bInputState = 1;
        }
#ifndef VERSION_EUROPE
        /* The European build no longer clears the channel's occupancy value
           here, as in TextBox_Destroy. */
        DuelEffect_ClearOccupancyValue(object->channel_index);
#endif
        DuelEffect_ClearMatchingMarker(object->channel_index);
        v = object->flags_34;
        object->explicit_newline_count = 0;
        gText_ChoiceLayoutCallback = Text_TryCompleteChoiceLayout;
        object->flags_34 = v | 0x1000;
    }
}

void Text_StartPageWait(DuelEffectChannel *value)
{
    value->playback_state = 4;
    gText_CommandDisposition = 1;
}

void Text_HandleCampaignFlagCommand(DuelEffectChannel *object)
{
    s32 flag = TextStream_ReadU16LE(object);

    flag &= CAMPAIGN_FLAG_COMMAND_WORD_MASK;
    if (flag & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(flag & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
        return;
    }

    {
        s32 target = TextStream_ReadU16LE(object);

        target &= 0xFFFF;
        if (Campaign_TestStoryFlag(flag) != 0) {
            s32 *cursor = (s32 *)(
                (u32)object + (u32)&((u8 **)0)[object->stream_index]);

            *cursor = (*cursor & TEXT_STREAM_CURSOR_HIGH_MASK) | target;
        }
    }
}

void Text_PushStreamOffset(DuelEffectChannel *arg0)
{
    TextStreamOwner *owner = TEXT_STREAM_OWNER_VIEW(arg0);
    s32 c;
    s32 v;

    v = TextStream_ReadU16LE(arg0);
    c = arg0->stream_index;
    owner->streams[c + 1] =
        (u8 *)(((u32)owner->streams[c] & TEXT_STREAM_CURSOR_HIGH_MASK) |
               (v & 0xFFFF));
    arg0->stream_index++;
}

void Text_NewLine(DuelEffectChannel *record)
{
    record->explicit_newline_count++;
    record->pen_x = 0x1000;
#ifdef VERSION_EUROPE
    /* The European build moves to state 4 only from the fourth line on. */
    if (TextBox_WrapLineIfNeeded(record) && record->explicit_newline_count >= 4) {
#else
    if (TextBox_WrapLineIfNeeded(record)) {
#endif
        record->playback_state = 4;
    }
    gText_CommandDisposition = 1;
    if (gText_ChoiceLayoutCallback) {
        gText_ChoiceLayoutCallback(record);
    }
}

void Text_EndStream(DuelEffectChannel *record)
{
    u16 flags;

    record->stream_index--;
    if (record->stream_index < 0) {
        flags = record->flags_34;
        gText_CommandDisposition = 1;
        flags |= 0x2000;
        record->flags_34 = flags;
    }
}

/* Effect-script command handler: reads a command id and a flag byte from the
   object's current script stream, finds the display effect record for the id
   in gDisplayEffect_aMenuRecords (ids below 0x41 live in the first two records, others in
   the third), and depending on the flags either stops it (bit 7), adjusts a
   running one (bits 5 and 6) or starts it in the slot given by bit 0, with
   the object's state byte set to the matching wait state. */
void Text_HandleDisplayEffectCommand(EffectObject *o) {
    MenuRecord *e;
    s32 id;
    s32 flags;
    s32 slot;
    s32 a;
    s32 b;

    gText_CommandDisposition = 1;
    a = *o->streams[o->depth]++;
    b = *o->streams[o->depth]++;
    id = a;
    flags = b;

    e = gDisplayEffect_aMenuRecords;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        e += 2;
    } else if (e->effect_id != id) {
        e++;
        if (e->effect_id != id) {
            e = 0;
        }
    }

    if (flags & 0x80) {
        if (e == 0) {
            return;
        }
        gText_DisplayEffectRecord = e;
        if (flags & 2) {
            DisplayEffect_ReleaseRecord(e);
            return;
        }
        if (flags & 1) {
            o->state = 0xE;
            return;
        }
        if (e->slot_index != 0) {
            e->field_40 = 0x178;
        } else {
            e->field_40 = -0x38;
        }
        e->tween_target_y = 0xB2;
        e->field_44 = 0x10;
        o->state = 7;
        e->display_effect_step = 3;
        if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
            e->display_effect_step = 5;
            e->field_40 = 1;
        }
        return;
    }
    if (flags & 0x60) {
        if (e == 0) {
            return;
        }
        if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
            return;
        }
        gText_DisplayEffectRecord = e;
        if (flags & 0x40) {
            e->resource_entry_index = flags & 3;
            o->state = 9;
            return;
        } else if (flags & 0x20) {
            e->variant_update_flags &= 0xEF;
            if (flags & 1) {
                e->variant_update_flags |= 0x10;
            }
            return;
        }
    }

    slot = flags & 1;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        slot = 2;
    }
    e = &gDisplayEffect_aMenuRecords[slot];
    DisplayEffect_ResetRecordState((DisplayEffectState *)e);
    e->effect_id = id;
    e->slot_index = slot;
    if (slot != 0) {
        *(s16 *)&e->x = 0x178;
    } else {
        *(s16 *)&e->x = -0x38;
    }
    o->state = 6;
    gText_DisplayEffectRecord = e;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        e->display_effect_step = 5;
        e->slot_index = 2;
        *(s16 *)&e->x = 0xF0;
        e->field_40 = 0;
        *(s16 *)&e->y = 0x60;
        return;
    }
    e->display_effect_step = 2;
    e->field_40 = 3;
    if (flags & 8) {
        *(s16 *)&e->x = 0x400;
        e->field_40 = 7;
    }
    if (flags & 0x10) {
        e->resource_entry_index = (flags >> 1) & 3;
    }
}

/* Lays the text box's second object out as a six-point outline around the
 * first, eight pixels outside its top, sides, and bottom. */
void TextBox_UpdateOutline(struct DuelEffectChannel *record) {
    DisplayObject *a = record->render_object, *b = record->aux_object;
    u16 v;
    b->field_18.center.x = a->field_18.center.x;
    b->field_18.center.y = a->field_18.center.y;
    v = a->xy.h.x;
    b->field_48.vertex.x = v;
    b->position.h.field_28 = v;
    b->field_38.h.field_38 = v - 8;
    v = a->xy.h.x + a->field_3C.size.w;
    b->field_50.h.field_50 = v;
    b->xy.h.x = v;
    b->field_40.vertex.x = v + 8;
    v = a->xy.h.y - 8;
    b->xy.h.y = v;
    b->position.h.field_2A = v;
    v = a->xy.h.y + a->field_48.pivot.y;
    b->field_40.vertex.y = v;
    b->field_38.h.field_3A = v;
    v = a->xy.h.y + a->field_3C.size.h + 8;
    b->field_50.h.field_52 = v;
    b->field_48.vertex.y = v;
}

/* Regional value: the flags_34 bit under which the box rebuilds its gouraud
 * quad (0x20 -> 0x40). The European build (src/game/european/) defines its
 * own. */
#ifndef TEXT_BOX_LAYOUT_QUAD_FLAG
#define TEXT_BOX_LAYOUT_QUAD_FLAG 0x20
#endif

#ifndef TEXT_BOX_GLYPH_CLUT_X
#define TEXT_BOX_GLYPH_CLUT_X 0x280
#endif

#ifndef TEXT_BOX_GLYPH_CLUT_Y
#define TEXT_BOX_GLYPH_CLUT_Y 0xE8
#endif

void TextBox_RefreshDisplayObjects(DuelEffectChannel *p) {
    DisplayObject *e;
    s32 v;
    s32 b;
    s32 f;

    e = p->render_object;
    if (e == (DisplayObject *)0) {
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeSlot(), 6);
        b = p->channel_index;
        e->field_40.clut.x = TEXT_BOX_GLYPH_CLUT_X;
        e->field_40.clut.y = TEXT_BOX_GLYPH_CLUT_Y;
        e->texture_page = 0xA;
        f = e->flags;
        e->field_4C = (s32)TextBox_DrawEntries;
        e->field_67 = b;
        e->flags = f | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        p->render_object = e;
    }

    DisplayObject_SelectOrderingTable1(e);
    DisplayObject_SetDepthOffset(e, (s8)p->depth_offset);

    e->xy.h.x = (u16)p->rect_x;
    e->xy.h.y = (u16)p->rect_y;
    e->field_3C.size.w = (u16)p->rect_width;
    e->field_3C.size.h = (u16)p->rect_height;
    v = p->rect_width / 2;
    e->field_18.center.x = v;
    e->field_48.pivot.x = v;
    v = p->rect_height / 2;
    e->field_18.center.y = v;
    e->field_48.pivot.y = v;

    if ((p->flags_34 & TEXT_BOX_LAYOUT_QUAD_FLAG) != 0) {
        if (p->aux_object != 0) {
            DisplayObject_ReleaseIfPresent(p->aux_object);
        }
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
        DisplayObject_InitializeGouraudQuad(e, 1);
        p->aux_object = e;
        e->attribute = e->attribute | (GsALON | GsATWO);
        DisplayObject_SelectOrderingTable1(e);
        DisplayObject_SetDepthOffset(e, (s8)(p->depth_offset - 1));
        *(s32 *)&e->field_54 = 0xA0A0A0;
        e->field_4C = 0xA0A0A0;
        e->field_34.word = 0xA0A0A0;
        e->field_2C.word = 0xA0A0A0;
        e->field_44.word = COLOR_RGB24_NEUTRAL_GREY;
        e->field_3C.word = COLOR_RGB24_NEUTRAL_GREY;
        TextBox_UpdateOutline(p);
    }
}

#include "input.h"
#include "duel_effect_entry_state.h"
#include "text_constants.h"
#include "text_string_bank_views.h"
#include "european_text_bank_views.h"
#include "script_command_table.h"
#include "script_state.h"
#include "text_box_state_callbacks.h"
#include "text_stream_commands.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s16 gText_PumpedGlyphIndex;

/* Initialized data this unit owns at 0x80090E64: the text-box state callback table.
 *
 * TextBox_BuildStep indexes this table with the low five bits of state byte
 * 0x51. The image contains exactly eighteen entries; the masked index can
 * exceed that bound, so the public declaration remains unsized.
 */

TextBoxStateCallback gTextBox_apfnStateCallbacks[] = {
    Dialog_UpdateChoice,
    Dialog_UpdateChoice,
    Text_CompletePageAdvance,
    Text_BeginPageClear,
    Text_WaitForPageAdvance,
    Text_StageScriptImage,
    Text_ContinueDisplayEffect,
    Text_WaitForDisplayEffectAndRelease,
    Text_WaitForDisplayEffect,
    Text_RequestDisplayEffectCrossfade,
    TextBox_WaitForScriptCompletion,
    Text_StartViewportTween,
    DuelEffect_WaitForFileTransfer,
    Text_WaitForBgmFade,
    Text_RequestDisplayEffectFadeOut,
    Text_WaitForDelay,
    Text_WaitWithViewportShake,
    Text_WaitForCdSoundTransfer,
};

/* The glyph counter and its limit, and the buttons that skip ahead. PAL
   views both as halfwords and the Japanese release confirms with Circle
   rather than Cross, so a regional build supplies its own. */
#ifndef TEXT_BOX_GLYPH_COUNT
#define TEXT_BOX_GLYPH_COUNT(o) ((o)->output_count)
#define TEXT_BOX_GLYPH_LIMIT(o) ((o)->output_limit)
#endif
#ifndef TEXT_BOX_ENTRY_TYPE
#define TEXT_BOX_ENTRY_TYPE DuelEffectEntry
#endif
#ifndef TEXT_BOX_ADVANCE_MASK
#define TEXT_BOX_ADVANCE_MASK PAD_BUTTON_CONFIRM_MASK
#endif

/* The flag that keeps the box's previous contents: 2 in the US build, 4 in
   the European one, whose wrapper defines its own. */
#ifndef TEXT_BOX_BUILD_KEEP_FLAG
#define TEXT_BOX_BUILD_KEEP_FLAG 2
#endif
/* The flag that keeps the channel's entries: 0x40, 0x80 in the European
   build. */
#ifndef TEXT_BOX_BUILD_OWN_ENTRIES_FLAG
#define TEXT_BOX_BUILD_OWN_ENTRIES_FLAG 0x40
#endif

void TextBox_BuildStep(DuelEffectChannel *object)
{
    u16 flags;
    s32 id;
    u8 *text;
    u8 **slot;
    u8 *script;
    DuelEffectEntry *entry;
    s32 op;
    void (**handlers)(u8 *);

    flags = object->flags_34;
    if ((flags & TEXT_BOX_FLAG_BUILD_ACTIVE) == 0) {
        flags |= TEXT_BOX_FLAG_BUILD_ACTIVE;
        object->flags_34 = flags;
        if ((flags & TEXT_BOX_BUILD_KEEP_FLAG) == 0) {
            DisplayEffect_ResetMenuRecords();
        }
#ifndef VERSION_EUROPE
        /* The European build makes neither field store. */
        if ((object->flags_34 & 0x100) != 0) {
            object->line_advance_y = 8;
            object->glyph_advance_x = 8;
        }
#endif
        id = object->initial_string_id;
        gText_ScriptDispatchGate = 0;
        gText_ChoiceLayoutCallback = 0;
        object->delay_countdown = 1;
        TEXT_BOX_GLYPH_COUNT(object) = 0;
        object->stream_index = 0;
#ifdef VERSION_JAPAN
        /* The Japanese two-bank layout, as in its Text_LookupString. */
        if (id & TEXT_GLOBAL_STRING_ID_BASE) {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                gText_GlobalStringOffsets[id & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                gText_PrimaryStringOffsets[id]);
        }
#elif defined(VERSION_EUROPE)
        /* The European banks: global strings from 0x8000, ids below 0x500
           in the first bank and the rest in the second, all through the
           tables four bytes into each bank. */
        if (id <= 0x7FFF) {
            if (id >= 0x500) {
                s32 n = id - 0x100;

                text = (u8 *)(((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) +
                    gText_PrimaryStringOffsets[n]);
            } else {
                text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                    gText_PrimaryStringOffsets[id]);
            }
        } else {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_GlobalStringOffsets[id - 0x8000]);
        }
#else
        if (id > 0xCFFF) {
            text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_PrimaryStringOffsets[id - 0xD000]);
        } else if (id > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_GlobalStringOffsets[id - TEXT_GLOBAL_STRING_ID_BASE]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)gText_PrimaryStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) +
                gText_PrimaryStringOffsets[id]);
        }
#endif
        object->root_stream = text;
        object->explicit_newline_count = 0;
        object->playback_state = 0;
        DisplayObject_ReleaseIfPresent(object->prompt_object);
        DisplayObject_ReleaseIfPresent(object->aux_object);
        object->prompt_object = (void *)0;
        object->aux_object = 0;
        TextBox_RefreshDisplayObjects(object);
        if ((object->flags_34 & TEXT_BOX_BUILD_OWN_ENTRIES_FLAG) == 0) {
            entry = (DuelEffectEntry *)&((TEXT_BOX_ENTRY_TYPE *)gDuelEffect_aEntries)[
                object->entry_range_start];
            object->entry_begin = entry;
            object->entry_write_cursor = entry;
#ifndef VERSION_EUROPE
            /* The European build no longer clears the channel's occupancy
               value, as in TextBox_Destroy. */
            DuelEffect_ClearOccupancyValue(object->channel_index);
#endif
            DuelEffect_ClearMatchingMarker(object->channel_index);
        }
        return;
    }

    if (gText_ScriptDispatchGate != 0) {
        gScript_apfnCommandHandlers[*(u8 *)&gScript_wCommandWord]();
        if (gScript_wCommandWord == 0) {
            gText_ScriptDispatchGate = 0;
        }
    }
    if (object->playback_state != 0) {
        gTextBox_apfnStateCallbacks[object->playback_state & DUEL_EFFECT_STATE_INDEX_MASK](object);
        object->flags_34 = object->flags_34 & 0xFBFF;
        return;
    }
    if ((object->flags_34 & 0x1C00) == 0) {
        if ((gInput_awPadHeld[0] & PAD_BUTTON_SQUARE) ||
            (gInput_awPadPressed[0] & TEXT_BOX_ADVANCE_MASK)) {
            DuelEffect_SetEntryControlBytes(object, 0, 0);
            object->delay_countdown = 1;
            object->flags_34 = object->flags_34 | 0x400;
        }
        object->delay_countdown = object->delay_countdown - 1;
        if (object->delay_countdown != 0) {
            return;
        }
    }
    handlers = gText_apfnPrimaryCommands;
    object->delay_countdown = object->glyph_delay;
next_opcode:
    slot = &((TextStreamOwner *)object)->streams[object->stream_index];
    script = *slot;
    gText_CurrentStreamCode = script[0];
    op = (s16)gText_CurrentStreamCode;
    *slot = script + 1;
    if (op >= 0xF0) {
        gText_CommandDisposition = 0;
        handlers[(s16)gText_CurrentStreamCode - 0xF0]((u8 *)object);
        if (gText_CommandDisposition >= 0) {
            if (gText_CommandDisposition == 1) {
                return;
            }
            goto next_opcode;
        }
    }
    if (TextBox_WrapLineIfNeeded(object) != 0) {
        object->playback_state = 4;
        return;
    }
    gText_PumpedGlyphIndex = gText_CurrentStreamCode;
#ifdef VERSION_EUROPE
    /* The European glyph call returns a signed adjustment to the nominal
       horizontal advance. The box counts pixels (the wrapper makes the
       counter and its limit halfwords at 0x60 and 0x62) rather than
       glyphs. */
    {
        s32 w;
        s32 n;
        s32 limit;

        w = object->glyph_advance_x +
            DuelEffect_AppendEntry(object, gText_aGlyphLookup[(s16)gText_CurrentStreamCode].packed & 0x8FF0FFFF);
        limit = (s16)TEXT_BOX_GLYPH_LIMIT(object);
        object->pen_x = object->pen_x + w;
        n = TEXT_BOX_GLYPH_COUNT(object) + w;
        TEXT_BOX_GLYPH_COUNT(object) = n;
        if (limit != 0) {
            w = (s16)n + object->glyph_advance_x;
            if (limit < w) {
                object->flags_34 = object->flags_34 | TEXT_BOX_FLAG_DONE;
            }
        }
    }
#else
    DuelEffect_AppendEntry(object, gText_aGlyphLookup[(s16)gText_CurrentStreamCode].packed & 0x8FF0FFFF);
    TEXT_BOX_GLYPH_COUNT(object) = TEXT_BOX_GLYPH_COUNT(object) + 1;
    if (TEXT_BOX_GLYPH_LIMIT(object) != 0 &&
        TEXT_BOX_GLYPH_COUNT(object) >= TEXT_BOX_GLYPH_LIMIT(object)) {
        object->flags_34 = object->flags_34 | TEXT_BOX_FLAG_DONE;
    }
    object->pen_x = object->pen_x + object->glyph_advance_x;
#endif
}

#include "input.h"
#include "display_effect_process_menu_records.h"

#include "duel_effect_process_entries.h"
#include "sound.h"
#include "display_object.h"
#include "text_constants.h"

/* Zero-initialized storage this unit owns (0x800EB0F8-0x800EB288 in North
 * America, 0x800EAFD8-0x800EB158 in Japan). */
DuelEffectChannel gText_aChannels[DUEL_EFFECT_CHANNEL_COUNT];

/* Small global another unit owns, declared here as a common symbol the
 * way the original read it through $gp. */
u8 gText_PreviousGlyphKind;

/* Regional value: the choice.flags bit that opens and closes the choice (0x8
 * -> 0x10). The European build (src/game/european/) defines its own. */
#ifndef FUNC_80039794_CHOICE_FLAG
#define FUNC_80039794_CHOICE_FLAG 8
#endif

#ifndef DISPLAY_EFFECT_MENU_CONFIRM_MASK
#define DISPLAY_EFFECT_MENU_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

/* One record pointer walks the four channels beside a down-counter. Under
 * strength reduction GCC folds the record's prompt_object and flags_34
 * accesses into a second induction register parked at offset 0x30, which is
 * the retail shape. */
void TextBox_UpdateChannels(void)
{
    DuelEffectChannel *p;
    s32 reset_value;
    TextGlyphLookupEntry *table;
    s32 n;
    s32 cnt;
    s32 idx;
    s32 kind;
    s32 arg;
    u16 f;

    p = gText_aChannels;
    n = 4;
    reset_value = -1;
    table = gText_aGlyphLookup;
    do {
        if (p->flags_34 & 0x8000) {
            gText_PumpedGlyphIndex = 0;
            if ((p->flags_34 & 0x2000) == 0) {
                gText_PumpedGlyphIndex = reset_value;
                cnt = -1;
                for (;;) {
                    TextBox_BuildStep(p);
                    cnt++;
                    f = p->flags_34;
                    if (f & 0x2000) {
                        if (f & FUNC_80039794_CHOICE_FLAG) {
                            p->prompt_object = Dialog_OpenChoice(p);
                        }
                        break;
                    }
                    if ((f & 0x1C00) == 0) {
                        if (cnt >= gGraphics_bExtraFrameStep) {
                            break;
                        }
                    } else {
                        gText_PumpedGlyphIndex = reset_value;
                    }
                }
            } else {
                if (p->flags_34 & FUNC_80039794_CHOICE_FLAG) {
                    if (gInput_awPadPressed[0] &
                        DISPLAY_EFFECT_MENU_CONFIRM_MASK) {
                        p->flags_34 &= 0xFFFF ^ FUNC_80039794_CHOICE_FLAG;
                        DisplayObject_ReleaseIfPresent(p->prompt_object);
                        p->prompt_object = 0;
                        SD_SEPlayFull(0xB);
                    }
                }
            }
            arg = -1;
            idx = gText_PumpedGlyphIndex;
            if (idx >= 0) {
                kind = table[idx].halves.upper & 7;
                if (kind == 4) {
                    arg = gText_PreviousGlyphKind;
                } else {
                    gText_PreviousGlyphKind = kind;
                    arg = kind;
                }
            }
            DisplayEffect_ProcessMenuRecords(arg);
            DuelEffect_ProcessEntries(p);
        }
        p++;
        n--;
    } while (n != 0);
}

/* These three reach the record through the parameter directly. What used to
 * stand at each use was a cast of the pointer to its own type, left from when
 * the parameter was a byte pointer; removing them is byte-identical, and
 * measured.
 *
 * What does change the code is a typed local. Hoisting the record into one
 * changes the prologue register saves under GCC 2.8.1, which is the
 * constraint the old note here was about. */
void TextBox_SetPos(DuelEffectChannel *record, s32 x, s32 y)
{
    DisplayObject *object;

    object = record->render_object;
    record->rect_x = x;
    record->rect_y = y;
    if (object != 0) {
        object->xy.h.x = x;
        object->xy.h.y = y;
    }
    object = record->aux_object;
    if (object != 0) {
        if (object->list_key == 4)
            TextBox_UpdateOutline(record);
        else {
            object->xy.h.x = x;
            object->xy.h.y = y;
        }
    }
    object = record->prompt_object;
    if (object != 0) {
        if (object->list_key == 4)
            DuelEffect_UpdateObjectLayout(record);
        else {
            object->xy.h.x = record->rect_width + x - 16;
            object->xy.h.y = record->rect_height + y - 16;
        }
    }
}

void TextBox_BuildBlocking(DuelEffectChannel *object)
{
    object->flags_34 |= TEXT_BOX_FLAG_BUILD_REQUESTED;
    do {
        TextBox_BuildStep(object);
    } while (!(object->flags_34 & TEXT_BOX_FLAG_DONE));
}

void TextBox_BuildBlockingWithGlyphScratch(DuelEffectChannel *object)
{
    object->flags_34 |= 0xA00;
    do {
        TextBox_BuildStep(object);
    } while (!(object->flags_34 & TEXT_BOX_FLAG_DONE));
}

#include "duel_effect_entry_state.h"
/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s32 gDuelEffect_nCompactionRequested;

#ifdef VERSION_EUROPE

#endif

/* The two helpers the display-object fade callbacks below share: the first-frame latch on the
   channel's byte 0x13, and the release that frees the channel's entry slot
   and requests an entry-list rebuild. */

#ifdef VERSION_EUROPE
/* The European channel is laid out as a EuropeanDuelEffectEntry: the latch
   byte sits at 0x11 and the occupied byte at 0xF. */
#define FADE_LATCH(object) (((EuropeanDuelEffectEntry *)(object))->handler_state)
#define FADE_OCCUPIED(object) (((EuropeanDuelEffectEntry *)(object))->entry_flags)
#else
#define FADE_LATCH(object) ((object)->handler_state)
#define FADE_OCCUPIED(object) ((object)->entry_flags)
#endif

s32 DisplayObjectFade_MarkInitialized(DuelEffectChannel *object)
{
    u8 flags = FADE_LATCH(object);
    if ((flags & DISPLAY_OBJECT_FADE_FLAG_INITIALIZED) == 0) {
        FADE_LATCH(object) = flags | DISPLAY_OBJECT_FADE_FLAG_INITIALIZED;
        return 0;
    }
    return 1;
}

void DisplayObjectFade_ReleaseChannel(DuelEffectChannel *object)
{
#ifndef VERSION_EUROPE
    /* The European build does not clear the occupancy value, as in
       TextBox_Destroy. */
    gDuelEffect_aOccupancyValues[object->glyph_index] = 0;
#endif
    FADE_OCCUPIED(object) = 0;
    gDuelEffect_nCompactionRequested = 1;
}

#include "../psyq/rand.h"
#include "../psyq/libgte.h"
#include "graphics_frame_controls.h"
#ifdef VERSION_EUROPE
#endif

#define DUEL_EFFECT_FIELD_04_WORD(type, object) \
    (*(type *)&(object)->field_04)

/* The European build's entries are 0x16 bytes (EuropeanDuelEffectEntry):
   its callbacks reach their fields through this view. */
#define EU_ENTRY(record) ((EuropeanDuelEffectEntry *)(record))
#define EU_ENTRY_FADE_VALUE(record) (((u8 *)(record))[0x15])

#ifdef VERSION_EUROPE
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record)
{
    /* The European entry (0x16 bytes, two-byte aligned) clears field_04 as
       two halfwords, and the fade runs at half the US steps. */
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY(record)->render_mode = 2;
        *(u16 *)&EU_ENTRY(record)->field_04 = 0;
        *(u16 *)&EU_ENTRY(record)->field_06 = 0;
        EU_ENTRY(record)->field_14 = 16;
    }
    EU_ENTRY(record)->field_14 =
        EU_ENTRY(record)->field_14 - *(u8 *)&gGraphics_nBaseFrameStep;
    if ((EU_ENTRY(record)->handler_state &
         DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE) == 0) {
        EU_ENTRY(record)->field_04 =
            EU_ENTRY(record)->field_04 + gGraphics_nBaseFrameStep * 16;
        if ((s8)EU_ENTRY(record)->field_04 < 0) {
            EU_ENTRY(record)->field_04 = 128;
            EU_ENTRY(record)->field_14 = 8;
            EU_ENTRY(record)->handler_state |=
                DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
        EU_ENTRY(record)->field_06 = EU_ENTRY(record)->field_04;
    } else {
        EU_ENTRY(record)->field_05 =
            EU_ENTRY(record)->field_05 + gGraphics_nBaseFrameStep * 16;
        if ((s8)EU_ENTRY(record)->field_05 < 0) {
            EU_ENTRY(record)->field_14 = 0;
            EU_ENTRY(record)->render_mode = 0;
            EU_ENTRY(record)->handler_state = 0;
        }
        EU_ENTRY(record)->field_07 = EU_ENTRY(record)->field_05;
    }
}
#else
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record)
{
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        record->render_mode = 2;
        DUEL_EFFECT_FIELD_04_WORD(s32, record) = 0;
        record->field_14 = 32;
    }
    record->field_14 = record->field_14 - gGraphics_nBaseFrameStep * 2;
    if ((record->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE) == 0) {
        record->field_04 = record->field_04 + gGraphics_nBaseFrameStep * 16;
        if ((s8)record->field_04 < 0) {
            record->field_04 = 128;
            record->field_14 = 16;
            record->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
        record->field_06 = record->field_04;
    } else {
        record->field_05 = record->field_05 + gGraphics_nBaseFrameStep * 16;
        if ((s8)record->field_05 < 0) {
            record->field_14 = 0;
            record->render_mode = 0;
            record->handler_state = 0;
        }
        record->field_07 = record->field_05;
    }
}
#endif

#ifdef VERSION_EUROPE
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *p)
{
    s32 v;

    /* The European entry sets field_04 as two halfwords, and the fade
       steps by gGraphics_nBaseFrameStep * 24 where the US build steps by * 16. */
    if (!DisplayObjectFade_MarkInitialized(p)) {
        EU_ENTRY(p)->render_mode = 2;
        *(u16 *)&EU_ENTRY(p)->field_04 = 0x8080;
        *(u16 *)&EU_ENTRY(p)->field_06 = 0x8080;
        EU_ENTRY(p)->field_14 = 0;
    }
    if (!(EU_ENTRY(p)->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        v = EU_ENTRY(p)->field_04;
        v -= gGraphics_nBaseFrameStep * 24;
        if (v <= 0) {
            EU_ENTRY(p)->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
            v = 0;
        }
        EU_ENTRY(p)->field_04 = v;
        EU_ENTRY(p)->field_05 = v;
    } else {
        v = EU_ENTRY(p)->field_06;
        v -= gGraphics_nBaseFrameStep * 24;
        if (v <= 0) {
            DisplayObjectFade_ReleaseChannel(p);
            v = 0;
        }
        EU_ENTRY(p)->field_06 = v;
        EU_ENTRY(p)->field_07 = v;
    }
}
#else
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *p)
{
    s32 v;

    if (!DisplayObjectFade_MarkInitialized(p)) {
        p->render_mode = 2;
        DUEL_EFFECT_FIELD_04_WORD(u32, p) = 0x80808080;
        p->field_14 = 0;
    }
    if (!(p->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        v = p->field_04 - (gGraphics_nBaseFrameStep << 4);
        if (v <= 0) {
            p->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
            v = 0;
        }
        p->field_04 = v;
        p->field_05 = v;
    } else {
        v = p->field_06 - (gGraphics_nBaseFrameStep << 4);
        if (v <= 0) {
            DisplayObjectFade_ReleaseChannel(p);
            v = 0;
        }
        p->field_06 = v;
        p->field_07 = v;
    }
}
#endif

#ifdef VERSION_EUROPE
void DuelEffect_UpdateDelayedGlyphRotation(DuelEffectChannel *record) {
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        s32 a;
        s32 b;

        EU_ENTRY(record)->render_mode = 1;
        a = EU_ENTRY(record)->local_x;
        b = EU_ENTRY(record)->local_y;
        EU_ENTRY(record)->field_08 = 0;
        EU_ENTRY(record)->field_09 = 0;
        EU_ENTRY(record)->field_0A = 0;
        EU_ENTRY(record)->field_04 = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(EU_ENTRY(record)->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        s32 v = EU_ENTRY(record)->field_04 - 1;

        EU_ENTRY(record)->field_04 = v;

        if ((u8)v == 0) {
            EU_ENTRY(record)->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
    } else {
        s32 v = EU_ENTRY(record)->field_08 + 4;

        EU_ENTRY(record)->field_0A = v;
        EU_ENTRY(record)->field_09 = v;
        EU_ENTRY(record)->field_08 = v;

        if (v >= 0x40) {
            DisplayObjectFade_ReleaseChannel(record);
        }
    }
}
#else
void DuelEffect_UpdateDelayedGlyphRotation(DuelEffectChannel *record) {
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        s32 a;
        s32 b;

        record->render_mode = 1;
        a = record->local_x;
        b = record->local_y;
        record->field_08 = 0;
        record->field_09 = 0;
        record->field_0A = 0;
        record->field_04 = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(record->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        s32 v = record->field_04 - 1;

        record->field_04 = v;

        if ((u8)v == 0) {
            record->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
    } else {
        s32 v = record->field_08 + 4;

        record->field_0A = v;
        record->field_09 = v;
        record->field_08 = v;

        if (v >= 0x40) {
            DisplayObjectFade_ReleaseChannel(record);
        }
    }
}
#endif

#ifdef VERSION_EUROPE
void DuelEffect_UpdateGlyphSpiralFade(DuelEffectChannel *record, void *context)
{
    s32 radius;
    s32 value;
    s32 next_angle;
    u32 flags;
    u16 *origin_x;
    u16 *origin_y;

    origin_x = (u16 *)&EU_ENTRY(record)->field_04;
    origin_y = (u16 *)&EU_ENTRY(record)->field_06;

    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY_FADE_VALUE(record) = 0;
        EU_ENTRY(record)->render_mode = 3;
        if ((rand() & 0xFF) >= 0x80) {
            EU_ENTRY(record)->field_14 |= 0x40;
        }
        *origin_x = EU_ENTRY(record)->local_x;
        *origin_y = EU_ENTRY(record)->local_y;
        *(u16 *)&EU_ENTRY(record)->field_08 = rand() & 0xFFF;
        *(u16 *)&EU_ENTRY(record)->field_0A = 0x100;
        if (EU_ENTRY(record)->field_14 & 0x80) {
            EU_ENTRY_FADE_VALUE(record) = 0x80;
            *(u16 *)&EU_ENTRY(record)->field_0A = 0;
        }
    }

    flags = EU_ENTRY(record)->field_14;
    if (flags & 0x80) {
        radius = *(u16 *)&EU_ENTRY(record)->field_0A + 10;
        if (radius < 0x140) {
            flags &= 0x7F;
            goto assign_value;
        }
        DisplayObjectFade_ReleaseChannel(record);
        return;
    }

    radius = *(u16 *)&EU_ENTRY(record)->field_0A - 10;
    value = radius;
    if (radius > 0) {
        goto clamp_value;
    }
    EU_ENTRY(record)->local_x = *origin_x;
    EU_ENTRY(record)->local_y = *origin_y;
    EU_ENTRY(record)->render_mode = 0;
    EU_ENTRY(record)->handler_state = 0;
    return;

assign_value:
    value = radius;

clamp_value:
    *(u16 *)&EU_ENTRY(record)->field_0A = radius;
    if (radius >= 0xA1) {
        value = 0xA0;
    }
    EU_ENTRY_FADE_VALUE(record) = (0xA0 - value) * 0x80 / 0xA0;

    if (flags & 0x40) {
        flags &= 0x3F;
        next_angle = *(u16 *)&EU_ENTRY(record)->field_08 - flags;
    } else {
        next_angle = *(u16 *)&EU_ENTRY(record)->field_08 + flags;
    }
    value = next_angle & 0xFFF;
    *(u16 *)&EU_ENTRY(record)->field_08 = value;
    EU_ENTRY(record)->local_x =
        *origin_x + rcos(value) * radius / 0x1000;
    EU_ENTRY(record)->local_y =
        *origin_y + rsin(value) * radius / 0x1000;
}

void DuelEffect_StartGlyphSpiralReveal(DuelEffectChannel *record, void *context)
{
    EU_ENTRY(record)->field_14 = (rand() & 0x1F) + 8;
    EU_ENTRY(record)->field_14 = (rand() & 0x1F) + 32;
    EU_ENTRY(record)->handler_state = 4;
    DuelEffect_UpdateGlyphSpiralFade(record, context);
}

void DuelEffect_StartGlyphSpiralConceal(DuelEffectChannel *record, void *context)
{
    EU_ENTRY(record)->field_14 = ((rand() & 0x1F) + 8) | -0x80;
    EU_ENTRY(record)->field_14 = ((rand() & 0x1F) + 32) | -0x80;
    EU_ENTRY(record)->handler_state = 4;
    DuelEffect_UpdateGlyphSpiralFade(record, context);
}

void DuelEffect_UpdateGlyphLinearFade(DuelEffectChannel *record)
{
    s32 value;

    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY(record)->render_mode = 3;
        value = EU_ENTRY(record)->field_14;
        EU_ENTRY_FADE_VALUE(record) = 0;
        if (value != 0) {
            EU_ENTRY_FADE_VALUE(record) = 0x80;
        }
    }

    value = EU_ENTRY_FADE_VALUE(record);
    if (EU_ENTRY(record)->field_14 != 0) {
        value -= 5;
        if (value <= 0) {
            DisplayObjectFade_ReleaseChannel(record);
            return;
        }
    } else {
        value += 5;
        if (value >= 0x80) {
            EU_ENTRY(record)->render_mode = 0;
            EU_ENTRY(record)->handler_state = 0;
            return;
        }
    }
    EU_ENTRY_FADE_VALUE(record) = value;
}

void DuelEffect_UpdateGlyphFadeIn(DuelEffectChannel *record)
{
    EU_ENTRY(record)->field_14 = 0;
    DuelEffect_UpdateGlyphLinearFade(record);
}

void DuelEffect_UpdateGlyphFadeOut(DuelEffectChannel *record)
{
    EU_ENTRY(record)->field_14 = 0x80;
    DuelEffect_UpdateGlyphLinearFade(record);
}
#endif

#include "duel_effect_entry_state.h"
#include "main_modes.h"
#include "duel_effect_process_entries.h"

/* Zero-initialized storage this unit owns at 0x800EB288-0x800EF668 in the
 * US build; the European builds place the battle models right after
 * gText_aChannels instead. A regional wrapper selects its own entry record
 * and count through DUEL_EFFECT_ENTRY_STORAGE_TYPE/COUNT. */
DUEL_EFFECT_ENTRY_STORAGE_TYPE gDuelEffect_aEntries[DUEL_EFFECT_ENTRY_STORAGE_COUNT];
AnimatedBattleModelProperties gMain_aAnimatedBattleModels[2];

#ifndef DUEL_EFFECT_PROCESS_ENTRY_TYPE
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE DuelEffectEntry
#endif

void DuelEffect_ProcessEntries(DuelEffectChannel *channel)
{
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *p;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *q;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *s;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *e;

    p = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)channel->entry_begin;
    gDuelEffect_nCompactionRequested = 0;

    while (p->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
        if (p->handler_state != 0) {
            gDuelEffect_EntryHandlers[p->handler_state & DUEL_EFFECT_ENTRY_HANDLER_INDEX_MASK](
                (DuelEffectEntry *)p, channel);
        }
        p++;
    }

    if (gDuelEffect_nCompactionRequested != 0) {
        q = &((DUEL_EFFECT_PROCESS_ENTRY_TYPE *)gDuelEffect_aEntries)[
            channel->entry_range_start];
        e = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)channel->entry_write_cursor;
        s = q;
        while (s != e) {
            if (s->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
                *q = *s;
                q++;
            }
            s++;
        }
        channel->entry_write_cursor = (DuelEffectEntry *)q;
        q->entry_flags = 0;
    }
}

#include "display_effect_resource_setup.h"
#include "menu_record.h"
#include "display_menu_records.h"
#ifdef VERSION_JAPAN
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#endif

s8 *DisplayEffect_ResetMenuRecords(void)
{
    s32 i;
    s32 column;
#ifdef VERSION_JAPAN
    s32 has_more;
#else
    s32 empty;
    s8 *entry;
#endif

    for (i = MENU_RECORD_COUNT - 1; i >= 0; i--) {
        MenuRecord *record = &gDisplayEffect_aMenuRecords[i];

        record->effect_id = -1;
        record->variant_update_flags = 0;
        record->requested_variant = 0;
        record->variant_update_counter = 0;
        for (column = 2; column >= 0; column--) {
            record->grid[0][column] = 0;
            record->grid[1][column] = 0;
            record->grid[2][column] = 0;
            record->grid[3][column] = 0;
        }
    }

#ifdef VERSION_JAPAN
    i = 0;
    do {
        ((s8 *)gRuntime_pSharedWorkBase)[0x7FFF0 + i] = -1;
        i++;
        has_more = i < DISPLAY_EFFECT_VRAM_SLOT_COUNT;
    } while (has_more);
    /* The final false loop predicate is the Japanese null return. */
    return (s8 *)has_more;
#else
    empty = -1;
    i = DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1;
    entry = gDisplayEffect_CachedResourceIds + i;
    do {
        *entry = empty;
        i--;
        entry--;
    } while (i >= 0);
    return entry;
#endif
}
