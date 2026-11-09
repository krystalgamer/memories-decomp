/* Duel effect text: the text-box lifecycle, the occupancy table and entry
 * markers, the glyph draw callback, entry appending, the little-endian
 * stream readers, the choice widget layout, cursor and update, the pulse
 * colour, entry control, the text state callbacks and line wrapping. In
 * every executable these are one contiguous run of text
 * (0x80035B7C-0x80037D2C in the US build). */
#include "../types.h"

#include "duel_effect_init_entry.h"
#include "duel_effect_entry_occupancy.h"
#include "text_box_lifecycle.h"
#include "display_object_core.h"
#include "text_box_set_rect.h"
#ifdef VERSION_EUROPE
#include "duel_effect.h"
#include "text_channel_state.h"
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

#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_entry_occupancy.h"
#include "../unmatched.h"

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
#include "duel_effect.h"
#include "text_channel_state.h"
#include "graphics_frame.h"
#include "text_box_draw_entries.h"

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

#include "duel_effect.h"
#include "text_stream_read_le.h"

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

#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "display_object.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_update_object_layout.h"
#include "../unmatched.h"
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

#include "dialog_highlight_choice.h"
#include "duel_effect_update_object_layout.h"
#include "dialog_choice.h"
#include "display_object.h"
#include "input.h"
#include "sound.h"
#include "dialog_read_choice_input.h"
#include "duel_effect.h"

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

#include "graphics_frame.h"
#include "display_object.h"
#include "widget_update_pulse_colour.h"

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
#include "dialog_highlight_choice.h"
#include "display_object.h"
#include "display_object_core.h"
#include "input.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "../unmatched.h"
#include "dialog_choice_control.h"
#include "dialog_choice.h"
#include "dialog_read_choice_input.h"
#include "dialog_choice_state.h"
#include "duel_effect.h"
#include "widget_update_pulse_colour.h"

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

#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_entry_control.h"

#ifdef VERSION_JAPAN
#include "duel_effect_entry_occupancy.h"

#define DUEL_EFFECT_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_ENTRIES ((JapaneseDuelEffectEntry *)gDuelEffect_aEntries)
#define DUEL_EFFECT_RANGE_START(channel) \
    (((JapaneseDuelEffectEntryRange *)(channel))->entry_range_start)
#define DUEL_EFFECT_RANGE_COUNT(channel) \
    (((JapaneseDuelEffectEntryRange *)(channel))->entry_range_count)
#elif defined(VERSION_EUROPE)
#include "duel_effect_entry_occupancy.h"

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
#ifdef VERSION_EUROPE
    int count;
    u32 address;
    u8 *v1;
    v0 = DUEL_EFFECT_RANGE_START(a0);
    count = DUEL_EFFECT_RANGE_COUNT(a0);
    address = (u32)&DUEL_EFFECT_ENTRIES[v0];
    if (count == 0) {
        goto ret_zero_a;
    }
    v1 = (u8 *)(address + DUEL_EFFECT_FIELD_13_OFFSET);
loop:
    if ((DUEL_EFFECT_ENTRY_FROM_FIELD_13(v1)->entry_flags &
         DUEL_EFFECT_ENTRY_FLAG_ACTIVE) == 0) {
        return 0;
    }
    if (DUEL_EFFECT_ENTRY_FROM_FIELD_13(v1)->handler_state != 0) {
        return 1;
    }
    count--;
    v1 = v1 + sizeof(DUEL_EFFECT_ENTRY_TYPE);
    if (count != 0) {
        goto loop;
    }
#else
    int count;
    u8 *v1;
    v0 = DUEL_EFFECT_RANGE_START(a0);
    count = DUEL_EFFECT_RANGE_COUNT(a0);
    v1 = DUEL_EFFECT_ENTRY_BYTES(&DUEL_EFFECT_ENTRIES[v0]);
    if (count == 0) {
        goto ret_zero_a;
    }
    v1 = v1 + DUEL_EFFECT_FIELD_13_OFFSET;
loop:
    v0 = DUEL_EFFECT_ENTRY_FROM_FIELD_13(v1)->entry_flags &
         DUEL_EFFECT_ENTRY_FLAG_ACTIVE;
    if (v0 == 0) {
        return v0;
    }
    if (DUEL_EFFECT_ENTRY_FROM_FIELD_13(v1)->handler_state != 0) {
        return 1;
    }
    count = count - 1;
    v1 = v1 + sizeof(DUEL_EFFECT_ENTRY_TYPE);
    if (count != 0) {
        goto loop;
    }
#endif
ret_zero_a:
    return 0;
}

/* Starting from the record's entry range, walks its active entries;
   for each active entry, writes a1 to handler_state and a2 to render_mode,
   stopping at the first entry with that flag clear or when the count runs
   out. */
#ifdef VERSION_EUROPE
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
#else
void DuelEffect_SetEntryControlBytes(DuelEffectChannel *a0, u8 a1, u8 a2) {
    int v0;
    int count;
    u8 *v1;

    v0 = DUEL_EFFECT_RANGE_START(a0);
    count = DUEL_EFFECT_RANGE_COUNT(a0);
    v1 = DUEL_EFFECT_ENTRY_BYTES(&DUEL_EFFECT_ENTRIES[v0]);
    if (count == 0) {
        return;
    }
    v1 = v1 + DUEL_EFFECT_FIELD_15_OFFSET;
loop:
    if ((DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->entry_flags &
         DUEL_EFFECT_ENTRY_FLAG_ACTIVE) == 0) {
        return;
    }
    count = count - 1;
    DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->handler_state = a1;
    DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->render_mode = a2;
    v1 = v1 + sizeof(DUEL_EFFECT_ENTRY_TYPE);
    if (count != 0) {
        goto loop;
    }
}
#endif

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

#include "text_stream_read_le.h"
#include "../psyq/rand.h"
#include "dialog_choice_state.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "duel_effect_entry_control.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "input.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "duel_effect.h"
#include "duel_effect_state_callbacks.h"
#include "script_state.h"
#include "sound.h"
#include "sound_sequence_state.h"
#include "../unmatched.h"

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

#include "text_box_wrap_line.h"
#include "../unmatched.h"
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
