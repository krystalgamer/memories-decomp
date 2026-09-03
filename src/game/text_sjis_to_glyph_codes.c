#include "../types.h"

extern u32 D_801D9000[];

void Text_SjisToGlyphCodes(u8 *dst, u8 *src, s32 count)
{
    s32 i;

    for (i = count - 1; i >= 0; i--) {
        if (*(u16 *)(src + i * 2) != 0) {
            count = i + 1;
            break;
        }
    }
    for (i = 0; i < count; i++, src += 2) {
        s32 value = *(u16 *)src;
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
            if (index < 240) {
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
    *dst = 255;
}
