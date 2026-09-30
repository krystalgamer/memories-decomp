#include "../../types.h"
#include "../../psyq/libapi.h"
#include "../../psyq/strings.h"
#include "credits.h"

/* Collects the credits rows of `group` for line `index`: builds each row's
 * glyph bitmaps from the ROM font into a local buffer and records the line's
 * width, height and page. */
s32 func_80180F58(s32 index, s32 group)
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
    e = D_80181D38;
    count = 0;
    cur = 0;
    h = 0;
    rect.x = index * 192 + 512;
    rect.y = 256;
    rect.w = 4;
    rect.h = 16;
    line = &D_8018220C[index];
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
                            font = D_80011820;
                        }
                        if (code == 0x9B92) {
                            font = D_8001183E;
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
