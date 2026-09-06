#include "../../types.h"
#include "../../game/card_constants.h"

extern u16 *D_8016D3DC[];
extern s16 D_801D0200[];
extern s32 rand(void);
extern void func_8002CCE4(s32);

void func_8016A930(void)
{
    u8 counts[CARD_COUNT];
    u16 **table;
    u16 *entry;
    u16 *p;
    s16 *out;
    s32 remaining;
    s32 acc;
    s32 threshold;
    s32 i;

    for (i = CARD_COUNT - 1; i >= 0; i--) {
        counts[i] = 0;
    }
    out = D_801D0200;
    table = D_8016D3DC;
    entry = *table;
    while (entry != 0) {
        remaining = *entry;
        entry++;
        do {
            threshold = (rand() & 0x7FF) + 1;
            acc = 0;
            i = 0;
            p = entry;
            do {
                rand();
                acc += *p;
                if (acc >= threshold) {
                    if (counts[i] >= 3) {
                        remaining++;
                    } else {
                        counts[i] = counts[i] + 1;
                        *out = i + 1;
                        func_8002CCE4(i + 289);
                        out++;
                    }
                    break;
                }
                i++;
                p++;
            } while (i < STARTER_DECK_WEIGHT_SCAN_COUNT);
            remaining--;
        } while (remaining != 0);
        table++;
        entry = *table;
    }
}
