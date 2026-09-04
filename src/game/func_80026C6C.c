#include "../types.h"
#include "duel_card.h"

extern u8 D_8009B1D5;
extern s32 gDuel_adwCardStats[];

s32 func_80026C6C(u8 **out, s32 arg1, s32 arg2)
{
    /* Retail keeps the result count in t0 and the five-entry index in a3. */
    register s32 count asm("$8") = 0;
    register s32 i asm("$7") = 0;
    s32 d = D_8009B1D5;
    s32 *t = gDuel_adwCardStats;
    DuelCardRecord *r =
        &D_801A7AD8[d * DUEL_CARD_SIDE_RECORD_COUNT + arg1];
    s32 k;

    do {
        /* Match the block numbering used by the adjacent func_80026D18. */
        do {
            if (r->flags & 0x8000) {
                k = (s16)r->card_id;
                k--;
                if (((t[k] >> 26) & 0x1F) < arg2) {
                    *out++ = (u8 *)r;
                    count++;
                }
            }
        } while (0);
        i++;
        r++;
    } while (i < 5);

    *out = 0;
    return count;
}
