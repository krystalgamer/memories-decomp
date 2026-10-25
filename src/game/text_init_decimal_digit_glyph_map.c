#include "../types.h"
#include "../ygo_types.h"
#include "text_init_decimal_digit_glyph_map.h"
#include "text_decimal_key_views.h"
#include "text_constants.h"
#include "text_glyph_lookup.h"
#include "european_text_bank_views.h"

/* Copies the ten two-byte Shift-JIS digit keys from gText_DecimalDigitSjisKeys
   into a local buffer and walks them as big-endian u16 keys. Each key is
   looked up in the 4-byte-stride table at gText_aGlyphSearchEntries (key in the low halfword,
   a zero word terminating) and the 1-based match index is written to
   gText_DecimalDigitGlyphMap. The lookup is skipped entirely when the table is empty.

   gText_aGlyphSearchEntries is const so that the guard read hoists out of the loop: GCC treats
   a load from unchanging memory as loop-invariant regardless of the stores in
   the body, which is what leaves one shared %hi in the preheader feeding both
   the guard load and the per-iteration table address. */
#ifdef VERSION_EUROPE
/* European only: the 1-based index of key in the glyph lookup table (the
   entries from four bytes into gText_aGlyphLookup, code in the low
   halfword), or 0 when the zero terminator comes first. */
s32 func_8003B758(s32 key)
{
    s32 n = 0;
    TextGlyphLookupEntry *e = gText_aGlyphLookup;
    s32 code;

    do {
        e += 1;
        code = e->halves.key;
        n++;
        if (code == 0) {
            return 0;
        }
    } while (code != key);
    return n;
}

/* The European map stores the ten digit codes themselves, read from the
   byte string at gText_DecimalDigitGlyphString: a byte from 0xF0 up starts a two-byte code
   (its low nibble is the high byte), and 0xFF ends the string early. */
void Text_InitDecimalDigitGlyphMap(void)
{
    u8 *p = gText_DecimalDigitGlyphString;
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
            gText_DecimalDigitGlyphMap[i] = c;
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
    const TextGlyphLookupEntry *e;
    u8 *p;
    u8 *end;
    s32 i;
    s32 n;
    s32 key;

    out = gText_DecimalDigitGlyphMap;
    i = 1;
    p = buf.bytes;
    end = buf.bytes + 2 * TEXT_DECIMAL_RADIX;
    buf = gText_DecimalDigitSjisKeys;

    /* Pointer ordering emits sltu; retain the retail signed address view. */
    do {
        e = gText_aGlyphSearchEntries;
        n = 1;
        key = (*p << 8) | buf.bytes[i];
        if (gText_aGlyphSearchEntries[0].packed != 0) {
            q = out;
        search:
            if (key == e->halves.key) {
                *q = n;
                goto found;
            }
            e++;
            n++;
            if (e->packed != 0) {
                goto search;
            }
        found:
            ;
        }
        out++;
        p += 2;
        i += 2;
    } while ((s32)p < (s32)end);
}
#endif
