#include "../types.h"
#include "../ygo_types.h"
#include "text_init_decimal_digit_glyph_map.h"
#include "text_constants.h"

/* Copies the ten two-byte Shift-JIS digit keys from tent_DecimalDigitSjisKeys
   into a local buffer and walks them as big-endian u16 keys. Each key is
   looked up in the 4-byte-stride table at tent_GlyphLookupTableEntries (key in the low halfword,
   a zero word terminating) and the 1-based match index is written to
   D_800EAFF8. The lookup is skipped entirely when the table is empty.

   tent_GlyphLookupTableEntries is const so that the guard read hoists out of the loop: GCC treats
   a load from unchanging memory as loop-invariant regardless of the stores in
   the body, which is what leaves one shared %hi in the preheader feeding both
   the guard load and the per-iteration table address. */
#ifdef VERSION_EUROPE
/* European only: the 1-based index of key in the glyph lookup table (the
   entries from four bytes into tent_GlyphLookupTable, code in the low
   halfword), or 0 when the zero terminator comes first. */
s32 func_8003B758(s32 key)
{
    s32 n = 0;
    u16 *e = (u16 *)tent_GlyphLookupTable;
    s32 code;

    do {
        e += 2;
        code = *e;
        n++;
        if (code == 0) {
            return 0;
        }
    } while (code != key);
    return n;
}

/* The European map stores the ten digit codes themselves, read from the
   byte string at D_801BF870: a byte from 0xF0 up starts a two-byte code
   (its low nibble is the high byte), and 0xFF ends the string early. */
void Text_InitDecimalDigitGlyphMap(void)
{
    u8 *p = D_801BF870;
    s32 i = 0;
    s32 c = *p;

    if (c != 0xFF) {
        do {
            if (c >= 0xF0) {
                c &= 0xF;
                p++;
                c <<= 8;
                c |= *p;
            }
            p++;
            D_800EAFF8[i] = c;
            i++;
            if (i >= 10) {
                break;
            }
            c = *p;
        } while (c != 0xFF);
    }
}
#else
void Text_InitDecimalDigitGlyphMap(void) {
    TextDecimalDigitKeyBlock buf;
    u16 *out;
    u16 *q;
    const u32 *e;
    s32 p;
    s32 end;
    s32 i;
    s32 n;
    s32 key;

    out = D_800EAFF8;
    i = 1;
    p = (s32)buf.bytes;
    end = (s32)buf.bytes + 2 * TEXT_DECIMAL_RADIX;
    buf = tent_DecimalDigitSjisKeys;

    do {
        e = tent_GlyphLookupTableEntries;
        n = 1;
        key = (*(u8 *G32)p << 8) | buf.bytes[i];
        if (tent_GlyphLookupTableEntries[0] != 0) {
            q = out;
        search:
            if (key == *(u16 *)e) {
                *q = n;
                goto found;
            }
            e++;
            n++;
            if (*e != 0) {
                goto search;
            }
        found:
            ;
        }
        out++;
        p += 2;
        i += 2;
    } while (p < end);
}
#endif
