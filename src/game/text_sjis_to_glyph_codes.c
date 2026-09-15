#include "../types.h"
#include "text_constants.h"
#include "text_sjis_to_glyph_codes.h"

void Text_SjisToGlyphCodes(u8 *dst, const u8 *src, s32 count)
{
    const u16 *input = (const u16 *)src;
    s32 i;

    for (i = count - 1; i >= 0; i--) {
        if (input[i] != 0) {
            count = i + 1;
            break;
        }
    }
    for (i = 0; i < count; i++, input++) {
        s32 value = *input;
        s32 index = value != 0;
        u32 *entry = &D_801D9000[index];
        u32 word = *entry;

        goto test;
large:
        dst[0] = (index >> 8) | -16;
        dst[1] = index;
        dst += 2;
        goto next;
body:
        if (*(u16 *)entry == value) {
            if (index < TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
                *dst = index;
                goto bump;
            }
            goto large;
        }
        entry++;
        word = *entry;
        index++;
test:
        if (word != 0)
            goto body;
        *dst = 0;
bump:
        dst++;
next:
        ;
    }
    *dst = TEXT_STRING_TERMINATOR;
}
