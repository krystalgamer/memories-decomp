/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 4 variables to hard registers and 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_reward_setup.c.
 */
#include "../types.h"
#include "../game/duel_reward_setup.h"
#include "../game/card_constants.h"
#include "../game/file_transfer.h"
#include "../game/func_80032184.h"

void func_80032370(void)
{
    register s16 *source __asm__("$6");
    register u8 *base __asm__("$4");
    register s32 i __asm__("$5");
    register s16 *current __asm__("$3");
    s16 *destination;

    __asm__ volatile(
        "lui $2,%%hi(gDuel_awRecentCardDrops)\n\t"
        "addiu $6,$2,%%lo(gDuel_awRecentCardDrops)"
        : "=r"(source)
        :
        : "$2");
    base = (u8 *)source - 0x56C;
    i = DUEL_RECENT_CARD_DROP_COUNT - 1;
    current = source + DUEL_RECENT_CARD_DROP_COUNT - 1;
    for (; i >= 0; i--, current--) {
        if (*current != 0 && base[*current - 1] == 0)
            *current = 0;
    }
    destination = source;
    for (i = 0; i < DUEL_RECENT_CARD_DROP_COUNT; i++, source++) {
        if (*source) {
            if (source != destination) {
                *destination = *source;
                *source = 0;
            }
            destination++;
        }
    }
}
