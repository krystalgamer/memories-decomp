#define D_800EAFF8 gJapanese_DecimalDigitGlyphMap
#include "../../types.h"
#include "../../ygo_types.h"
#include "../text_init_decimal_digit_glyph_map.h"
#include "../text_constants.h"

void Text_InitDecimalDigitGlyphMap(void)
{
    TextDecimalDigitKeyBlock buf;
    u16 *out;
    u16 *q;
    const TextGlyphLookupEntry *e;
    u8 *p;
    u8 *end;
    s32 i;
    s32 n;
    s32 key;

    out = gJapanese_DecimalDigitGlyphMap;
    i = 1;
    p = buf.bytes;
    end = buf.bytes + 2 * TEXT_DECIMAL_RADIX;
    buf = tent_DecimalDigitSjisKeys;

    /* Pointer ordering emits sltu; retain the retail signed address view. */
    do {
        e = tent_GlyphLookupTableEntries;
        n = 1;
        key = (*p << 8) | buf.bytes[i];
        if (tent_GlyphLookupTableEntries[0].packed != 0) {
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
