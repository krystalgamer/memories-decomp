#include "../types.h"
#include "card_constants.h"

extern u16 gDuel_awEquipTable[];
extern u16 gDuel_aFusionTable[];

s32 Duel_CheckEquip(s32 arg0, s32 arg1)
{
    u16 *p = gDuel_awEquipTable;

    while (1) {
        s32 key = p[0];
        s32 n;

        if (key == 0) {
            return 0;
        }
        n = p[1];
        p += 2;
        if (key == arg0) {
            do {
                if (arg1 == *p) {
                    return arg1;
                }
                n--;
                p++;
            } while (n != 0);
            return 0;
        }
        p += n;
    }
}

s32 Duel_CheckFusion(s32 arg0, s32 arg1)
{
    u8 *base = (u8 *)gDuel_aFusionTable;
    u8 *p;
    s32 off;
    s32 n;
    s32 b;

    if (arg1 < arg0) {
        s32 t = arg1;
        arg1 = arg0;
        arg0 = t;
    }
    off = *(u16 *)(base + arg0 * 2);
    if (off == 0) {
        return 0;
    }
    p = base + off;
    n = p[0];
    if (n == 0) {
        n = 0x1FF - p[1];
        p++;
    }
    p++;
    do {
        b = p[0];
        if ((((b << 8) & 0x300) | p[1]) == arg1) {
            return ((b << 6) & 0x300) | p[2];
        }
        if ((((b << 4) & 0x300) | p[3]) == arg1) {
            return ((b << 2) & 0x300) | p[4];
        }
        p += FUSION_TABLE_ENTRY_SIZE;
        n -= FUSION_TABLE_PAIRS_PER_ENTRY;
    } while (n > 0);
    return 0;
}
