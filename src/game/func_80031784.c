#include "../types.h"

extern void GsSortFastSprite();

void func_80031784(u8 *record, s32 arg1, u8 *data, s32 selected)
{
    s32 i;
    u8 *cursor;

    *(s16 *)(record + 18) = 251;
    i = 0;
    cursor = data + 1;
    do {
        *(s32 *)(record + 20) = 0x202020;
        if ((cursor[0] & 15) == selected)
            *(s32 *)(record + 20) = 0x808080;
        i++;
        record[14] = ((data[0] & 15) << 3) - 128;
        record[15] = data[0] & 240;
        *(s16 *)(record + 16) = (cursor[0] & 240) | 512;
        cursor += 2;
        GsSortFastSprite(record, arg1, 0);
        *(u16 *)(record + 4) += 18;
        data += 2;
    } while (i < 7);
}
