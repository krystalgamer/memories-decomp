/*
 * TextBox_DrawEntries, a display-object draw callback (two parameters, the object
 * and its ordering table), installed in a display object's +0x4C slot by
 * TextBox_RefreshDisplayObjects. All three regional bodies build with plain
 * gcc_2_8_1_g8_split, like the neighbouring functions.
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
#include "../types.h"
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
