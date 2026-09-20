#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "color_constants.h"
#include "func_80031784.h"

#define GS_SPRITE_COLOR_WORD(sprite) (*(s32 *)&(sprite)->r)

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
        record->cx = (cursor[0] & 240) | 512;
        cursor += 2;
        GsSortFastSprite(record, ordering_table, 0);
        record->x += 18;
        data += 2;
    } while (i < 7);
}
