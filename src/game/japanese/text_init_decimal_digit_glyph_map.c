#define D_800EAFF8 gJapanese_DecimalDigitGlyphMap
#define D_801D9004 gJapanese_GlyphLookupTableEntries
#define D_80010330 gJapanese_DecimalDigitSjisKeys
#include "../../types.h"
#include "../../ygo_types.h"
#include "../text_init_decimal_digit_glyph_map.h"
#include "../text_constants.h"

void Text_InitDecimalDigitGlyphMap(void)
{
    TextDecimalDigitKeyBlock buf;
    u16 *out;
    u16 *q;
    const u32 *e;
    s32 p;
    s32 end;
    s32 i;
    s32 n;
    s32 key;

    out = gJapanese_DecimalDigitGlyphMap;
    i = 1;
    p = (s32)buf.bytes;
    end = (s32)buf.bytes + 2 * TEXT_DECIMAL_RADIX;
    buf = gJapanese_DecimalDigitSjisKeys;

    do {
        e = gJapanese_GlyphLookupTableEntries;
        n = 1;
        key = (*(u8 *)p << 8) | buf.bytes[i];
        if (gJapanese_GlyphLookupTableEntries[0] != 0) {
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
