#include "../types.h"
#include "card_constants.h"
#include "duel_card_checks.h"

#define FUSION_TABLE_BYTES(table) ((u8 *)(table))
#define FUSION_TABLE_OFFSETS(table) ((u16 *)(table))

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
