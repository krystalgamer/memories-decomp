/* The credits overlay's runtime: presentation setup, the per-frame line
 * update, glyph building, line drawing and line-group start, in image order. */
#include "../../types.h"
#include "../../game/graphics_frame.h"
#include "../../psyq/libapi.h"
#include "../../psyq/strings.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/ordering_tables.h"
#include "credits.h"

#ifdef VERSION_JAPAN
/* The Japanese package draws its rows from glyph bitmaps in VRAM, so it
 * uploads one palette instead of the three colour ramps. */
void Credits_InitPresentation(void)
{
    RECT rect;
    u16 clut[16];
    s32 i;

    rect = gCredits_ClutRect0;
    clut[0] = 0;
    clut[1] = 0xFFFF;
    clut[2] = 0xC21F;
    clut[3] = 0xC3F0;
    clut[4] = 0xA108;
    clut[5] = 0x9088;
    clut[6] = 0x9104;
    for (i = 7; i < 16; i++) {
        clut[i] = 0;
    }
    while (IsIdleGPU(3) != 0) {
    }
    while (LoadImage2(&rect, (u32 *)clut) != 0) {
    }
    while (IsIdleGPU(3) != 0) {
    }
    for (i = 1; i >= 0; i--) {
        gCredits_aLines[i].entries = 0;
    }
    gCredits_bFinished = 0;
}
#else
void Credits_InitPresentation(void)
{
    RECT rect0;
    RECT rect1;
    RECT rect2;
    u16 clut0[16];
    u16 clut1[16];
    u16 clut2[16];
    s32 i;

    rect0 = gCredits_ClutRect0;
    rect1 = gCredits_ClutRect1;
    rect2 = gCredits_ClutRect2;
    clut0[0] = 0;
    clut0[1] = 0x8421;
    clut0[2] = 0x9084;
    clut0[3] = 0x9CE7;
    clut0[4] = 0xA529;
    clut0[5] = 0xAD6B;
    clut0[6] = 0xB5AD;
    clut0[7] = 0xBDEF;
    clut0[8] = 0xC631;
    clut0[9] = 0xCE73;
    clut0[10] = 0xD6B5;
    clut0[11] = 0xDEF7;
    clut0[12] = 0xE739;
    clut0[13] = 0xEF7B;
    clut0[14] = 0xF7BD;
    clut0[15] = 0xFFFF;
    clut1[0] = 0;
    clut1[1] = 0x8421;
    clut1[2] = 0x8844;
    clut1[3] = 0x8C67;
    clut1[4] = 0x9089;
    clut1[5] = 0x94AB;
    clut1[6] = 0x98CD;
    clut1[7] = 0x9CEF;
    clut1[8] = 0xA111;
    clut1[9] = 0xA533;
    clut1[10] = 0xA955;
    clut1[11] = 0xAD77;
    clut1[12] = 0xB199;
    clut1[13] = 0xB5BB;
    clut1[14] = 0xB9DD;
    clut1[15] = 0xBDFF;
    clut2[0] = 0;
    clut2[1] = 0x8421;
    clut2[2] = 0x8882;
    clut2[3] = 0x8CE3;
    clut2[4] = 0x9124;
    clut2[5] = 0x9565;
    clut2[6] = 0x99A6;
    clut2[7] = 0x9DE7;
    clut2[8] = 0xA228;
    clut2[9] = 0xA669;
    clut2[10] = 0xAAAA;
    clut2[11] = 0xAEEB;
    clut2[12] = 0xB32C;
    clut2[13] = 0xB76D;
    clut2[14] = 0xBBAE;
    clut2[15] = 0xBFEF;
    while (IsIdleGPU(3) != 0) {
    }
    while (LoadImage2(&rect0, (u32 *)clut0) != 0) {
    }
    while (LoadImage2(&rect1, (u32 *)clut1) != 0) {
    }
    while (LoadImage2(&rect2, (u32 *)clut2) != 0) {
    }
    while (IsIdleGPU(3) != 0) {
    }
    for (i = 1; i >= 0; i--) {
        gCredits_aLines[i].entries = 0;
    }
    gCredits_bFinished = 0;
}
#endif

/* The brightness ramp of a fading line: 0 below 0, 0x80 above 0x80. */
#define CLAMP(x) ((x) < 0 ? 0 : (x) > 0x80 ? 0x80 : (x))

/* Advances both credits lines by one frame. State 0 fades a line in, and
 * state 1 holds it; a line whose first row is group 0x63 sets gCredits_bFinished
 * instead. State 2 fades the line out. State 3 keeps it blank, then lays out
 * its next page, or moves to state 4 when no page is left. State 4 starts the
 * following group in the first free line and releases this one. Returns
 * gCredits_bFinished. */
s32 Credits_UpdateLines(void)
{
    CreditsLine *other;
    s32 i;
    s32 j;
    s32 t;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 n;
    s32 k;
    s32 t2;
    s32 a2;
    s32 b2;
    s32 c2;
    s32 d2;

    for (i = 0; i < 2; i++) {
        if (gCredits_aLines[i].entries == 0) {
            continue;
        }
        switch (gCredits_aLines[i].state) {
        case 0:
            t = (0x80 - gCredits_aLines[i].timer) << 7;
            a = t / 32;
            b = t / 64;
            c = t / 96;
            d = t / 128;
            gCredits_aLines[i].timer -= D_8009B0D8 * 2;
            if (gCredits_aLines[i].timer <= 0) {
                gCredits_aLines[i].timer = 0x98;
                gCredits_aLines[i].state++;
            }
            a = CLAMP(a);
            b = CLAMP(b);
            c = CLAMP(c);
            d = CLAMP(d);
            Credits_DrawLine(i, a, b, c, d);
            break;
        case 1:
            if (gCredits_aLines[i].entries->group == 0x63) {
                gCredits_bFinished = 1;
            } else {
                gCredits_aLines[i].timer -= D_8009B0D8;
                if (gCredits_aLines[i].timer <= 0) {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state++;
                    if (gCredits_aLines[i].field_13 != 0) {
                        if (gCredits_aLines[i].page == 1) {
                            gCredits_aLines[i].field_13 = 0;
                        }
                    } else {
                        gCredits_aLines[i].field_13++;
                    }
                }
            }
            Credits_DrawLine(i, 0x80, 0x80, 0x80, 0x80);
            break;
        case 2:
            k = 0x80;
            t2 = (k - gCredits_aLines[i].timer) << 7;
            a2 = t2 / 32;
            b2 = t2 / 64;
            c2 = t2 / 96;
            d2 = t2 / 128;
            gCredits_aLines[i].timer -= D_8009B0D8 * 2;
            if (gCredits_aLines[i].timer <= 0) {
                if (gCredits_aLines[i].page >= 2) {
                    gCredits_aLines[i].timer = 0x3C;
                    gCredits_aLines[i].state++;
                } else {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state = 4;
                }
            }
            d2 = CLAMP(d2);
            c2 = CLAMP(c2);
            b2 = CLAMP(b2);
            a2 = CLAMP(a2);
            Credits_DrawLine(i, k - d2, k - c2, k - b2, k - a2);
            break;
        case 3:
            gCredits_aLines[i].timer -= D_8009B0D8 * 2;
            if (gCredits_aLines[i].timer <= 0) {
                gCredits_aLines[i].timer = 0x98;
                gCredits_aLines[i].state = 0;
                gCredits_aLines[i].page--;
                if (gCredits_aLines[i].page != 0) {
                    Credits_BuildLineGlyphs(i, gCredits_aLines[i].entries->group);
                } else {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state = 4;
                }
            }
            Credits_DrawLine(i, 0, 0, 0, 0);
            break;
        case 4:
            gCredits_aLines[i].timer -= D_8009B0D8 * 2;
            if (gCredits_aLines[i].timer <= 0) {
                n = gCredits_aLines[i].end->group;
                if (n >= 0) {
                    j = 0;
                    do {
                        other = gCredits_aLines;
                    } while (0);
                next:
                    if (other->entries == 0) {
                            other->height = 0;
                            other->width = 0;
                            other->page = 0;
                            if (Credits_BuildLineGlyphs(j, n) != 0) {
                                other->x = 40;
                                other->y = 40;
                                if (other->width > 160 || other->height > 120) {
                                    other->x = (320 - other->width) / 2;
                                    other->y = (240 - other->height) / 2 - 10;
                                }
                                other->state = 0;
                                other->timer = 152;
                                other->field_13 = 0;
                            }
                            goto done;
                    }
                    j++;
                    other++;
                    if (j < 2) {
                        goto next;
                    }
                }
            done:
                gCredits_aLines[i].entries = 0;
            }
            break;
        }
    }
    return gCredits_bFinished;
}

/* Collects the credits rows of `group` for line `index`: builds each row's
 * glyph bitmaps from the ROM font into a local buffer and records the line's
 * width, height and page. */
s32 Credits_BuildLineGlyphs(s32 index, s32 group)
{
    RECT rect;
    u16 buf[64];
    CreditsLine *line;
    CreditsEntry *e;
    s32 count;
    u32 cur;
    s32 h;
    s32 m;
    u32 flags;
    u8 *text;
    s32 fg;
    s32 sh;
    u32 g;
    s32 w;
    u16 code;
    u8 *font;
    u16 *p;
    s32 k;
    s32 v;
    u8 b;
    s32 t; /* one name for every p[k] read-modify-write: its single pseudo
            * outranks `b` in global-alloc, so the loads take $a0 and `b` $a1 */

top:  /* allocator steering: see the note at the end */
    e = gCredits_aEntries;
    count = 0;
    cur = 0;
    h = 0;
    rect.x = index * 192 + 512;
    rect.y = 256;
    rect.w = 4;
    rect.h = 16;
    line = &gCredits_aLines[index];
    while (e->group >= 0) {
        if (group < e->group) {
            break;
        }
        if (e->group == group) {
            flags = e->flags;
            text = e->text;
            fg = (flags & 0xF) + 1;
            sh = (flags & 0xF) + 4;
            m = 15;
            g = flags >> 4;
            if (line->entries == 0) {
                line->entries = e;
                cur = g;
            }
            if (cur == g) {
                h += 16;
            } else {
                if (line->height < h) {
                    line->height = h;
                }
                cur = g;
                h = 0;
            }
            if (text != 0) {
                w = strlen(text) / 2 * 16;
                if (line->width < w) {
                    line->width = w;
                }
                if (line->page == 0 || line->page == g) {
                    rect.x = index * 192 + 512 + count / 15 * 96;
                    rect.y = count % 15 * 16 + 256;
                    while (*text != 0) {
                        code = text[1] | (text[0] << 8);
                        font = (u8 *)Krom2RawAdd2(code);
                        p = buf;
                        k = 63;
                    clear:
                        *p++ = 0;
                        if (--k >= 0) {
                            goto clear;
                        }
                        p = buf;
                        if (code == 0xE056) {
                            font = gCredits_abGlyphE056;
                        }
                        if (code == 0x9B92) {
                            font = gCredits_abGlyph9B92;
                        }
                        for (k = 14; k != -1; k--) {
                            b = font[0];
                            v = 0;
                            if (b & 0x80) v = sh << 4;
                            if (b & 0x40) v |= sh << 8;
                            t = p[4];
                            p[4] = t | ((b & 0x20) ? v | sh << 12 : v);
                            b = font[0];
                            v = 0;
                            if (b & 0x10) v = sh;
                            if (b & 0x08) v |= sh << 4;
                            if (b & 0x04) v |= sh << 8;
                            t = p[5];
                            p[5] = t | ((b & 0x02) ? v | sh << 12 : v);
                            v = 0;
                            if (font[0] & 0x01) v = sh;
                            b = font[1];
                            if (b & 0x80) v |= sh << 4;
                            if (b & 0x40) v |= sh << 8;
                            t = p[6];
                            p[6] = t | ((b & 0x20) ? v | sh << 12 : v);
                            b = font[1];
                            v = 0;
                            if (b & 0x10) v = sh;
                            if (b & 0x08) v |= sh << 4;
                            if (b & 0x04) v |= sh << 8;
                            t = p[7];
                            p[7] = t | ((b & 0x02) ? v | sh << 12 : v);

                            b = font[0];
                            v = 0;
                            if (!(b & 0x80)) v = m;
                            if (!(b & 0x40)) v |= m << 4;
                            if (!(b & 0x20)) v |= m << 8;
                            t = p[0];
                            p[0] = t & (!(b & 0x10) ? v | m << 12 : v);
                            b = font[0];
                            v = 0;
                            if (!(b & 0x08)) v = m;
                            if (!(b & 0x04)) v |= m << 4;
                            if (!(b & 0x02)) v |= m << 8;
                            t = p[1];
                            p[1] = t & (!(b & 0x01) ? v | m << 12 : v);
                            b = font[1];
                            v = 0;
                            if (!(b & 0x80)) v = m;
                            if (!(b & 0x40)) v |= m << 4;
                            if (!(b & 0x20)) v |= m << 8;
                            t = p[2];
                            p[2] = t & (!(b & 0x10) ? v | m << 12 : v);
                            b = font[1];
                            v = 0;
                            if (!(b & 0x08)) v = m;
                            if (!(b & 0x04)) v |= m << 4;
                            if (!(b & 0x02)) v |= m << 8;
                            t = p[3];
                            p[3] = t & (!(b & 0x01) ? v | m << 12 : v);

                            b = font[0];
                            v = 0;
                            if (b & 0x80) v = fg;
                            if (b & 0x40) v |= fg << 4;
                            if (b & 0x20) v |= fg << 8;
                            t = p[0];
                            p[0] = t | ((b & 0x10) ? v | fg << 12 : v);
                            b = font[0];
                            v = 0;
                            if (b & 0x08) v = fg;
                            if (b & 0x04) v |= fg << 4;
                            if (b & 0x02) v |= fg << 8;
                            t = p[1];
                            p[1] = t | ((b & 0x01) ? v | fg << 12 : v);
                            b = font[1];
                            v = 0;
                            if (b & 0x80) v = fg;
                            if (b & 0x40) v |= fg << 4;
                            if (b & 0x20) v |= fg << 8;
                            t = p[2];
                            p[2] = t | ((b & 0x10) ? v | fg << 12 : v);
                            b = font[1];
                            v = 0;
                            if (b & 0x08) v = fg;
                            if (b & 0x04) v |= fg << 4;
                            if (b & 0x02) v |= fg << 8;
                            t = p[3];
                            p[3] = t | ((b & 0x01) ? v | fg << 12 : v);
                            font += 2;
                            p += 4;
                        }
                        while (IsIdleGPU(3) != 0) {
                        }
#ifdef VERSION_JAPAN
                        /* Japan uploads each cell for Credits_DrawLine. */
                        while (LoadImage2(&rect, (u32 *)buf) != 0) {
                        }
#endif
                        while (IsIdleGPU(3) != 0) {
                        }
                        rect.x += 4;
                        text += 2;
                    }
                }
                if (line->page == 0) {
                    line->page = e[1].flags >> 4;
                }
                count++;
                if (count >= 30) {
                    break;
                }
            }
        }
        e++;
    }
    line->end = e;
    if (line->height < h) {
        line->height = h;
    }
    if (line->page != 0) {
        line->height += 16;
    }
    return count;

    /* Allocator steering, not behaviour: this loop is unreachable. It keeps
     * `top` referenced until flow deletes the unreachable block, so the label
     * still splits the entry block when reload runs and jump2 only removes it
     * afterwards. With the label there, reload cannot reuse $a0 for `index`
     * and reloads it from its stack slot ($s5), which is what retail does. */
    while (1) {
        if (count) {
            goto top;
        }
    }
}

#ifdef VERSION_JAPAN
/* Draws one credits line from the glyph bitmaps Credits_BuildLineGlyphs
 * uploaded to VRAM: row `drawn` of the line sits in column drawn / 15, slot
 * drawn % 15 of the line's area, and each two-byte character is one 16x16
 * POLY_GT4 cell. Unlike the US package, both packets are submitted: `glyph`
 * with blend mode 1 and `shade` with blend mode 2. */
void Credits_DrawLine(s32 index, s32 c0, s32 c1, s32 c2, s32 c3)
{
    POLY_GT4 glyph;
    POLY_GT4 shade;
    CreditsLine *line;
    CreditsEntry *entry;
    s32 drawn;
    s32 d;
    s32 x;
    s32 y;
    s32 i;
    s32 r0;
    s32 r1;
    s32 r2;
    s32 r3;
    u32 group;
    s32 u;
    s32 v;
    s32 first;
    s32 ox;

    drawn = 0;
    line = &gCredits_aLines[index];
    entry = line->entries;
    x = line->x;
    y = line->y;
    d = 0x80;
    if (entry == 0) {
        return;
    }
    setPolyGT4(&glyph);
    setPolyGT4(&shade);
    setSemiTrans(&glyph, 1);
    setSemiTrans(&shade, 1);
    glyph.clut = 0x3DA0;
    shade.clut = 0x3C20;
    first = entry->group;
    do {
        group = entry->flags >> 4;
        if (drawn == 0 || line->page == group) {
            if (entry->text != 0) {
                r0 = c0;
                r1 = c1;
                r2 = c2;
                r3 = c3;
                if (line->field_13 != 0 && line->page != 0 && drawn == 0) {
                    r0 = r1 = r2 = r3 = d;
                }
                setRGB0(&glyph, r0 * 192 / d, r0 * 192 / d, r0 * 192 / d);
                setRGB1(&glyph, r1 * 64 / d, r1 * 64 / d, r1 * 64 / d);
                setRGB2(&glyph, r2 * 64 / d, r2 * 64 / d, r2 * 64 / d);
                setRGB3(&glyph, r3 * 192 / d, r3 * 192 / d, r3 * 192 / d);
                setRGB0(&shade, r0 * 192 / d, r0 * 192 / d, r0 * 192 / d);
                setRGB1(&shade, r1 * 192 / d, r1 * 192 / d, r1 * 192 / d);
                setRGB2(&shade, r2 * 192 / d, r2 * 192 / d, r2 * 192 / d);
                setRGB3(&shade, r3 * 192 / d, r3 * 192 / d, r3 * 192 / d);
                for (i = 0; i < strlen(entry->text) / 2; i++) {
                    ox = i * 4;
                    u = (drawn / 15 * 96 + ox) % 64 * 4;
                    v = drawn % 15 * 16 % 256;
                    setXY4(&glyph, x, y, x + 15, y, x, y + 15, x + 15, y + 15);
                    setUV4(&glyph, u, v, u + 15, v, u, v + 15, u + 15, v + 15);
                    setXY4(&shade, x, y, x + 15, y, x, y + 15, x + 15, y + 15);
                    setUV4(&shade, u, v, u + 15, v, u, v + 15, u + 15, v + 15);
                    glyph.tpage = getTPage(0, 1,
                                           index * 192 + 512 + drawn / 15 * 96 + ox,
                                           drawn % 15 * 16 + 256);
                    GsSortPoly(&glyph, gGraphics_apOrderingTables[2], 0);
                    shade.tpage = getTPage(0, 2,
                                           index * 192 + 512 + drawn / 15 * 96 + ox,
                                           drawn % 15 * 16 + 256);
                    GsSortPoly(&shade, gGraphics_apOrderingTables[2], 0);
                    x += 16;
                }
                x = line->x;
                drawn++;
            }
            y += 16;
        } else if (entry->text != 0) {
            drawn++;
        }
        entry++;
    } while (entry->group == first);
}
#else
/* Draws one credits line: walks its group and draws the first text-bearing
 * row plus any row whose paragraph matches the line's page. Each byte of the
 * row text is mapped into the 8x12 credits glyph atlas and submitted as one
 * semi-transparent POLY_GT4 with corner brightnesses c0..c3 scaled against
 * 0x80. A second `shade` packet is configured alongside but never submitted. */
void Credits_DrawLine(s32 index, s32 c0, s32 c1, s32 c2, s32 c3)
{
    POLY_GT4 glyph;
    POLY_GT4 shade;
    CreditsLine *line;
    CreditsEntry *entry;
    s32 drawn;
    s32 d;
    s32 x;
    s32 nx;
    s32 y;
    s32 i;
    u32 c;
    s32 u;
    s32 r0;
    s32 r1;
    s32 r2;
    s32 r3;
    u32 flags;
    u32 group;
    u32 kind;
    s32 clut;
    s32 v;
    s32 first;

    drawn = 0;
    line = &gCredits_aLines[index];
    entry = line->entries;
    x = line->x;
    y = line->y;
    d = 0x80;
    if (entry == 0) {
        return;
    }
    setPolyGT4(&glyph);
    setPolyGT4(&shade);
    setSemiTrans(&glyph, 1);
    setSemiTrans(&shade, 1);
    shade.clut = 0x3C20;
    first = entry->group;
    do {
        flags = entry->flags;
        group = flags >> 4;
        kind = flags & 0xF;
        if (kind == 2) {
            glyph.clut = 0x3E20;
        } else if (kind == 1) {
            glyph.clut = 0x3DE0;
        } else {
            glyph.clut = 0x3DA0;
        }
        if (drawn == 0 || line->page == group) {
            if (entry->text != 0) {
                r0 = c0;
                r1 = c1;
                r2 = c2;
                r3 = c3;
                if (line->field_13 != 0 && line->page != 0 && drawn == 0) {
                    r0 = r1 = r2 = r3 = d;
                }
                setRGB0(&glyph, r0 * 128 / d, r0 * 128 / d, r0 * 128 / d);
                setRGB1(&glyph, r1 * 128 / d, r1 * 128 / d, r1 * 128 / d);
                setRGB2(&glyph, r2 * 128 / d, r2 * 128 / d, r2 * 128 / d);
                setRGB3(&glyph, r3 * 128 / d, r3 * 128 / d, r3 * 128 / d);
                setRGB0(&shade, r0 * 192 / d, r0 * 192 / d, r0 * 192 / d);
                setRGB1(&shade, r1 * 192 / d, r1 * 192 / d, r1 * 192 / d);
                setRGB2(&shade, r2 * 192 / d, r2 * 192 / d, r2 * 192 / d);
                setRGB3(&shade, r3 * 192 / d, r3 * 192 / d, r3 * 192 / d);
                for (i = 0; i < strlen(entry->text); i++) {
                    c = entry->text[i];
                    if (c >= 0x61) {
                        c += 0x20;
                    } else {
                        c += 0x1F;
                    }
                    if ((*(u32 *)entry & 0xFFF) == 0x63) {
                        glyph.clut = (c & 0xFF) == 0x69 ? 0x3DE0 : 0x3DA0;
                    }
                    u = (c & 0xF) * 8;
                    v = (((c & 0xF0) >> 4) - 4) * 12;
                    nx = x + 8;
                    setXY4(&glyph, x, y, nx, y, x, y + 12, nx, y + 12);
                    setUV4(&glyph, u, v, u + 8, v, u, v + 12, u + 8, v + 12);
                    setXY4(&shade, x, y, nx, y, x, y + 12, nx, y + 12);
                    glyph.tpage = 10;
                    setUV4(&shade, u, v, u + 8, v, u, v + 12, u + 8, v + 12);
                    GsSortPoly(&glyph, gGraphics_apOrderingTables[2], 0);
                    x = nx;
                    shade.tpage = 10;
                }
                x = line->x;
                drawn++;
            }
            y += 16;
        } else if (entry->text != 0) {
            drawn++;
        }
        entry++;
    } while (entry->group == first);
}
#endif

void Credits_StartLineGroup(s32 group)
{
    CreditsLine *line;
    s32 i;

    i = 0;
    line = gCredits_aLines;
next:
    if (line->entries == 0) {
        line->height = 0;
        line->width = 0;
        do {
            line->page = 0;
            if (Credits_BuildLineGlyphs(i, group) != 0) {
                line->x = 40;
                line->y = 40;
                if (line->width > 160 || line->height > 120) {
                    line->x = (320 - line->width) / 2;
                    line->y = (240 - line->height) / 2 - 10;
                }
                line->state = 0;
                line->timer = 152;
                line->field_13 = 0;
            }
            return;
        } while (0);
    }
    i++;
    line++;
    if (i < 2) {
        goto next;
    }
}

u8 Credits_IsFinished(void)
{
    return gCredits_bFinished;
}
