#include "../../types.h"

extern s16 D_801D4D8E[];
extern s32 rand(void);

s32 func_8018416C(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
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
        } else {
            keyA = 0x7FFFFFFF;
            idA = 0x7FFFFFFF;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
        } else {
            keyB = 0x7FFFFFFF;
            idB = 0x7FFFFFFF;
        }
        if (keyA <= keyB) {
            if (keyA >= keyB) {
                if (idA <= idB) {
                    if (idA < idB) {
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
