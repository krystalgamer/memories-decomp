#include "../types.h"

#define DUEL_GUARDIAN_STAR_BONUS 500

/* IDs 1-6 and 7-10 form separate cycles; adjacent matchups return +/-500. */
int Duel_CalcGuardianStarMatchup(int a0, int a1) {
    int v1;

    a0 -= 7;
    if (a0 >= 0) {
        a1 -= 7;
        if (a1 < 0) {
            return 0;
        }
        v1 = 4;
    } else {
        v1 = 6;
        a1 -= 1;
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
