#include "../types.h"
#include "duel_calc_guardian_star_matchup.h"

#define DUEL_GUARDIAN_STAR_BONUS 500
#define DUEL_GUARDIAN_STAR_FIRST_CYCLE_START 1
#define DUEL_GUARDIAN_STAR_FIRST_CYCLE_COUNT 6
#define DUEL_GUARDIAN_STAR_SECOND_CYCLE_START \
    (DUEL_GUARDIAN_STAR_FIRST_CYCLE_START + \
     DUEL_GUARDIAN_STAR_FIRST_CYCLE_COUNT)
#define DUEL_GUARDIAN_STAR_SECOND_CYCLE_COUNT 4

/* IDs 1-6 and 7-10 form separate cycles; adjacent matchups return +/-500. */
s32 Duel_CalcGuardianStarMatchup(s32 a0, s32 a1) {
    s32 v1;

    a0 -= DUEL_GUARDIAN_STAR_SECOND_CYCLE_START;
    if (a0 >= 0) {
        a1 -= DUEL_GUARDIAN_STAR_SECOND_CYCLE_START;
        if (a1 < 0) {
            return 0;
        }
        v1 = DUEL_GUARDIAN_STAR_SECOND_CYCLE_COUNT;
    } else {
        v1 = DUEL_GUARDIAN_STAR_FIRST_CYCLE_COUNT;
        a1 -= DUEL_GUARDIAN_STAR_FIRST_CYCLE_START;
        a0 += v1;
        if (a1 >= v1) {
            return 0;
        }
    }

    a0 += 1;
    if (a0 >= v1) {
        a0 = 0;
    }
    if (a0 == a1) {
        return DUEL_GUARDIAN_STAR_BONUS;
    }
    a0 -= 2;

    if (a0 < 0) {
        a0 += v1;
    }
    if (a0 == a1) {
        return -DUEL_GUARDIAN_STAR_BONUS;
    }
    return 0;
}
