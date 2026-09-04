#include "../../types.h"

extern void *gFreeDuel_apSparklePool[];
extern void func_8004036C(void *);

void FreeDuel_UpdateSparkle(void)
{
    s32 i;

    for (i = 15; i >= 0; i--) {
        u8 *object = gFreeDuel_apSparklePool[i];
        u8 flags;
        u8 level;
        s16 timer;

        if (object == 0) {
            continue;
        }
        flags = object[0x6C];
        if ((flags & 0xF) != 1) {
            continue;
        }
        if ((flags & 0x80) == 0) {
            object[0x6C] = flags | 0x80;
            *(s16 *)(object + 0x60) = 0x10;
            *(s32 *)(object + 0xC) = 0x404040;
            *(s32 *)(object + 4) = *(s32 *)(object + 4) | 0x50000000;
        }
        level = object[0xC] - 4;
        timer = *(u16 *)(object + 0x60) - 1;
        *(u16 *)(object + 0x60) = timer;
        object[0xE] = level;
        object[0xD] = level;
        object[0xC] = level;
        if (timer == 0) {
            func_8004036C(object);
            gFreeDuel_apSparklePool[i] = 0;
        }
    }
}
