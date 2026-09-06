#include "../../types.h"
#include "../../psyq/rand.h"
#include "../../game/card_constants.h"

extern s16 D_801D4D8E[];
extern s32 D_801D4244[];

s32 func_80183A14(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 typeA;
    s32 typeB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if (idA != 0) {
            keyA = D_801D4D8E[idA - 1];
            typeA = (D_801D4244[idA - 1] >> CARD_STAT_TYPE_SHIFT) &
                    CARD_STAT_TYPE_MASK;
        } else {
            typeA = 0x7FFFFFFF;
            keyA = 0x7FFFFFFF;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
            typeB = (D_801D4244[idB - 1] >> CARD_STAT_TYPE_SHIFT) &
                    CARD_STAT_TYPE_MASK;
        } else {
            typeB = 0x7FFFFFFF;
            keyB = 0x7FFFFFFF;
        }
        if (typeA <= typeB) {
            if (typeA >= typeB) {
                if (keyA <= keyB) {
                    if (keyA < keyB) {
                        result = -1;
                    } else {
                        result = 0;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}
