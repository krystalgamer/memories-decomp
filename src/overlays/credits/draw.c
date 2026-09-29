#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/strings.h"
#include "../../game/ordering_tables.h"
#include "credits.h"

/* Draws one credits line: every row of its current page, one Shift-JIS
 * glyph pair per character as a textured quad plus its shadow quad, with the
 * four corner brightnesses c0..c3 scaled against 0x80. */
void func_8018173C(s32 index, s32 c0, s32 c1, s32 c2, s32 c3)
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
    line = &D_8018220C[index];
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
                    GsSortPoly(&glyph, D_800E9D90[2], 0);
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
