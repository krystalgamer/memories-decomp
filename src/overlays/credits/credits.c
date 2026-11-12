/* The credits overlay's runtime: presentation setup, the per-frame line
 * update, glyph building, line drawing and line-group start, in image order. */
#include "../../types.h"
#include "../../game/main.h"
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
    s32 fade_in;
    s32 level0;
    s32 level1;
    s32 level2;
    s32 level3;
    s32 n;
    s32 full_level;
    s32 fade_out;
    s32 dim3;
    s32 dim2;
    s32 dim1;
    s32 dim0;

    for (i = 0; i < 2; i++) {
        if (gCredits_aLines[i].entries == 0) {
            continue;
        }
        switch (gCredits_aLines[i].state) {
        case 0:
            fade_in = (0x80 - gCredits_aLines[i].timer) << 7;
            level0 = fade_in / 32;
            level1 = fade_in / 64;
            level2 = fade_in / 96;
            level3 = fade_in / 128;
            gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep * 2;
            if (gCredits_aLines[i].timer <= 0) {
                gCredits_aLines[i].timer = 0x98;
                gCredits_aLines[i].state++;
            }
            level0 = CLAMP(level0);
            level1 = CLAMP(level1);
            level2 = CLAMP(level2);
            level3 = CLAMP(level3);
            Credits_DrawLine(i, level0, level1, level2, level3);
            break;
        case 1:
            if (gCredits_aLines[i].entries->group == 0x63) {
                gCredits_bFinished = 1;
            } else {
                gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep;
                if (gCredits_aLines[i].timer <= 0) {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state++;
                    if (gCredits_aLines[i].title_held != 0) {
                        if (gCredits_aLines[i].page == 1) {
                            gCredits_aLines[i].title_held = 0;
                        }
                    } else {
                        gCredits_aLines[i].title_held++;
                    }
                }
            }
            Credits_DrawLine(i, 0x80, 0x80, 0x80, 0x80);
            break;
        case 2:
            full_level = 0x80;
            fade_out = (full_level - gCredits_aLines[i].timer) << 7;
            dim3 = fade_out / 32;
            dim2 = fade_out / 64;
            dim1 = fade_out / 96;
            dim0 = fade_out / 128;
            gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep * 2;
            if (gCredits_aLines[i].timer <= 0) {
                if (gCredits_aLines[i].page >= 2) {
                    gCredits_aLines[i].timer = 0x3C;
                    gCredits_aLines[i].state++;
                } else {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state = 4;
                }
            }
            dim0 = CLAMP(dim0);
            dim1 = CLAMP(dim1);
            dim2 = CLAMP(dim2);
            dim3 = CLAMP(dim3);
            Credits_DrawLine(i, full_level - dim0, full_level - dim1, full_level - dim2, full_level - dim3);
            break;
        case 3:
            gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep * 2;
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
            gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep * 2;
            if (gCredits_aLines[i].timer <= 0) {
                n = gCredits_aLines[i].end->group;
                /* A negative group enters the slot search at its exit; the
                   search hands the group to the first free line. */
                switch (n < 0) {
                case 0:
                    j = 0;
                    do {
                        other = gCredits_aLines;
                    } while (0);
                    for (;;) {
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
                                other->title_held = 0;
                            }
                        } else {
                            j++;
                            other++;
                            if (j < 2) {
                                continue;
                            }
                        }
                default:
                        gCredits_aLines[i].entries = 0;
                        break;
                    }
                }
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
    CreditsEntry *entry;
    s32 count;
    u32 cur;
    s32 page_height;
    s32 keep_mask;
    u32 flags;
    u8 *text;
    s32 fg;
    s32 sh;
    u32 page;
    s32 width;
    u16 code;
    u8 *font;
    u16 *pixel_row;
    s32 k;
    s32 nibbles;
    u8 bits;
    s32 word; /* one name for every pixel_row[k] read-modify-write: its
               * single pseudo outranks `bits` in global-alloc, so the loads
               * take $a0 and `bits` $a1 */

top:  /* allocator steering: see the note at the end */
    entry = gCredits_aEntries;
    count = 0;
    cur = 0;
    page_height = 0;
    rect.x = index * 192 + 512;
    rect.y = 256;
    rect.w = 4;
    rect.h = 16;
    line = &gCredits_aLines[index];
    while (entry->group >= 0) {
        if (group < entry->group) {
            break;
        }
        if (entry->group == group) {
            flags = entry->flags;
            text = entry->text;
            fg = (flags & 0xF) + 1;
            sh = (flags & 0xF) + 4;
            keep_mask = 15;
            page = flags >> 4;
            if (line->entries == 0) {
                line->entries = entry;
                cur = page;
            }
            if (cur == page) {
                page_height += 16;
            } else {
                if (line->height < page_height) {
                    line->height = page_height;
                }
                cur = page;
                page_height = 0;
            }
            if (text != 0) {
                width = strlen(text) / 2 * 16;
                if (line->width < width) {
                    line->width = width;
                }
                if (line->page == 0 || line->page == page) {
                    rect.x = index * 192 + 512 + count / 15 * 96;
                    rect.y = count % 15 * 16 + 256;
                    while (*text != 0) {
                        code = text[1] | (text[0] << 8);
                        font = (u8 *)Krom2RawAdd2(code);
                        pixel_row = buf;
                        k = 63;
                    clear:
                        *pixel_row++ = 0;
                        if (--k >= 0) {
                            goto clear;
                        }
                        pixel_row = buf;
                        if (code == 0xE056) {
                            font = gCredits_abGlyphE056;
                        }
                        if (code == 0x9B92) {
                            font = gCredits_abGlyph9B92;
                        }
                        for (k = 14; k != -1; k--) {
                            bits = font[0];
                            nibbles = 0;
                            if (bits & 0x80) nibbles = sh << 4;
                            if (bits & 0x40) nibbles |= sh << 8;
                            word = pixel_row[4];
                            pixel_row[4] = word | ((bits & 0x20) ? nibbles | sh << 12 : nibbles);
                            bits = font[0];
                            nibbles = 0;
                            if (bits & 0x10) nibbles = sh;
                            if (bits & 0x08) nibbles |= sh << 4;
                            if (bits & 0x04) nibbles |= sh << 8;
                            word = pixel_row[5];
                            pixel_row[5] = word | ((bits & 0x02) ? nibbles | sh << 12 : nibbles);
                            nibbles = 0;
                            if (font[0] & 0x01) nibbles = sh;
                            bits = font[1];
                            if (bits & 0x80) nibbles |= sh << 4;
                            if (bits & 0x40) nibbles |= sh << 8;
                            word = pixel_row[6];
                            pixel_row[6] = word | ((bits & 0x20) ? nibbles | sh << 12 : nibbles);
                            bits = font[1];
                            nibbles = 0;
                            if (bits & 0x10) nibbles = sh;
                            if (bits & 0x08) nibbles |= sh << 4;
                            if (bits & 0x04) nibbles |= sh << 8;
                            word = pixel_row[7];
                            pixel_row[7] = word | ((bits & 0x02) ? nibbles | sh << 12 : nibbles);

                            bits = font[0];
                            nibbles = 0;
                            if (!(bits & 0x80)) nibbles = keep_mask;
                            if (!(bits & 0x40)) nibbles |= keep_mask << 4;
                            if (!(bits & 0x20)) nibbles |= keep_mask << 8;
                            word = pixel_row[0];
                            pixel_row[0] = word & (!(bits & 0x10) ? nibbles | keep_mask << 12 : nibbles);
                            bits = font[0];
                            nibbles = 0;
                            if (!(bits & 0x08)) nibbles = keep_mask;
                            if (!(bits & 0x04)) nibbles |= keep_mask << 4;
                            if (!(bits & 0x02)) nibbles |= keep_mask << 8;
                            word = pixel_row[1];
                            pixel_row[1] = word & (!(bits & 0x01) ? nibbles | keep_mask << 12 : nibbles);
                            bits = font[1];
                            nibbles = 0;
                            if (!(bits & 0x80)) nibbles = keep_mask;
                            if (!(bits & 0x40)) nibbles |= keep_mask << 4;
                            if (!(bits & 0x20)) nibbles |= keep_mask << 8;
                            word = pixel_row[2];
                            pixel_row[2] = word & (!(bits & 0x10) ? nibbles | keep_mask << 12 : nibbles);
                            bits = font[1];
                            nibbles = 0;
                            if (!(bits & 0x08)) nibbles = keep_mask;
                            if (!(bits & 0x04)) nibbles |= keep_mask << 4;
                            if (!(bits & 0x02)) nibbles |= keep_mask << 8;
                            word = pixel_row[3];
                            pixel_row[3] = word & (!(bits & 0x01) ? nibbles | keep_mask << 12 : nibbles);

                            bits = font[0];
                            nibbles = 0;
                            if (bits & 0x80) nibbles = fg;
                            if (bits & 0x40) nibbles |= fg << 4;
                            if (bits & 0x20) nibbles |= fg << 8;
                            word = pixel_row[0];
                            pixel_row[0] = word | ((bits & 0x10) ? nibbles | fg << 12 : nibbles);
                            bits = font[0];
                            nibbles = 0;
                            if (bits & 0x08) nibbles = fg;
                            if (bits & 0x04) nibbles |= fg << 4;
                            if (bits & 0x02) nibbles |= fg << 8;
                            word = pixel_row[1];
                            pixel_row[1] = word | ((bits & 0x01) ? nibbles | fg << 12 : nibbles);
                            bits = font[1];
                            nibbles = 0;
                            if (bits & 0x80) nibbles = fg;
                            if (bits & 0x40) nibbles |= fg << 4;
                            if (bits & 0x20) nibbles |= fg << 8;
                            word = pixel_row[2];
                            pixel_row[2] = word | ((bits & 0x10) ? nibbles | fg << 12 : nibbles);
                            bits = font[1];
                            nibbles = 0;
                            if (bits & 0x08) nibbles = fg;
                            if (bits & 0x04) nibbles |= fg << 4;
                            if (bits & 0x02) nibbles |= fg << 8;
                            word = pixel_row[3];
                            pixel_row[3] = word | ((bits & 0x01) ? nibbles | fg << 12 : nibbles);
                            font += 2;
                            pixel_row += 4;
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
                    line->page = entry[1].flags >> 4;
                }
                count++;
                if (count >= 30) {
                    break;
                }
            }
        }
        entry++;
    }
    line->end = entry;
    if (line->height < page_height) {
        line->height = page_height;
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
    s32 full_level;
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
    full_level = 0x80;
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
                if (line->title_held != 0 && line->page != 0 && drawn == 0) {
                    r0 = r1 = r2 = r3 = full_level;
                }
                setRGB0(&glyph, r0 * 192 / full_level, r0 * 192 / full_level, r0 * 192 / full_level);
                setRGB1(&glyph, r1 * 64 / full_level, r1 * 64 / full_level, r1 * 64 / full_level);
                setRGB2(&glyph, r2 * 64 / full_level, r2 * 64 / full_level, r2 * 64 / full_level);
                setRGB3(&glyph, r3 * 192 / full_level, r3 * 192 / full_level, r3 * 192 / full_level);
                setRGB0(&shade, r0 * 192 / full_level, r0 * 192 / full_level, r0 * 192 / full_level);
                setRGB1(&shade, r1 * 192 / full_level, r1 * 192 / full_level, r1 * 192 / full_level);
                setRGB2(&shade, r2 * 192 / full_level, r2 * 192 / full_level, r2 * 192 / full_level);
                setRGB3(&shade, r3 * 192 / full_level, r3 * 192 / full_level, r3 * 192 / full_level);
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
    s32 full_level;
    s32 x;
    s32 nx;
    s32 y;
    s32 i;
    u32 code;
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
    full_level = 0x80;
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
                if (line->title_held != 0 && line->page != 0 && drawn == 0) {
                    r0 = r1 = r2 = r3 = full_level;
                }
                setRGB0(&glyph, r0 * 128 / full_level, r0 * 128 / full_level, r0 * 128 / full_level);
                setRGB1(&glyph, r1 * 128 / full_level, r1 * 128 / full_level, r1 * 128 / full_level);
                setRGB2(&glyph, r2 * 128 / full_level, r2 * 128 / full_level, r2 * 128 / full_level);
                setRGB3(&glyph, r3 * 128 / full_level, r3 * 128 / full_level, r3 * 128 / full_level);
                setRGB0(&shade, r0 * 192 / full_level, r0 * 192 / full_level, r0 * 192 / full_level);
                setRGB1(&shade, r1 * 192 / full_level, r1 * 192 / full_level, r1 * 192 / full_level);
                setRGB2(&shade, r2 * 192 / full_level, r2 * 192 / full_level, r2 * 192 / full_level);
                setRGB3(&shade, r3 * 192 / full_level, r3 * 192 / full_level, r3 * 192 / full_level);
                for (i = 0; i < strlen(entry->text); i++) {
                    code = entry->text[i];
                    if (code >= 0x61) {
                        code += 0x20;
                    } else {
                        code += 0x1F;
                    }
                    if ((*(u32 *)entry & 0xFFF) == 0x63) {
                        glyph.clut = (code & 0xFF) == 0x69 ? 0x3DE0 : 0x3DA0;
                    }
                    u = (code & 0xF) * 8;
                    v = (((code & 0xF0) >> 4) - 4) * 12;
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
                line->title_held = 0;
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
