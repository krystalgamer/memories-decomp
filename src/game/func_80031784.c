#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "color_constants.h"
#include "func_80031784.h"

#define GS_SPRITE_COLOR_WORD(sprite) (*(s32 *)&(sprite)->r)

#ifdef VERSION_EUROPE
/* European only, right before BuildDeck_DrawSortIcons: draws count digit
   cells from the right (the last byte first), skipping bytes from 10 up,
   and steps the sprite 8 pixels right after each. */
void func_800317F0(GsSPRITE *record, GsOT *ordering_table, u8 *digits, s32 count)
{
    s32 i;
    s32 digit;

    for (i = count - 1; i >= 0; i--) {
        digit = digits[i];
        if ((u32)digit < 10) {
            record->u = digit * 8 - 128;
            GsSortFastSprite(record, ordering_table, 1);
        }
        record->x += 8;
    }
}
#endif

void BuildDeck_DrawSortIcons(
    GsSPRITE *record, GsOT *ordering_table, u8 *data, s32 selected)
{
    s32 i;
    u8 *cursor;

    record->cy = 251;
    i = 0;
    cursor = data + 1;
    do {
        GS_SPRITE_COLOR_WORD(record) = 0x202020;
        if ((cursor[0] & 15) == selected)
            GS_SPRITE_COLOR_WORD(record) = COLOR_RGB24_NEUTRAL_GREY;
        i++;
        record->u = ((data[0] & 15) << 3) - 128;
        record->v = data[0] & 240;
#ifdef VERSION_EUROPE
        record->cx = (cursor[0] & 240) + 0x280;
#else
        record->cx = (cursor[0] & 240) | 512;
#endif
        cursor += 2;
        GsSortFastSprite(record, ordering_table, 0);
        record->x += 18;
        data += 2;
    } while (i < 7);
}
