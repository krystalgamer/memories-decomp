#include "../../types.h"

extern s16 D_801D4D8E[];
extern s32 D_801D4244[];
extern s32 func_8008E590(void);

s32 func_80183884(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 statsA;
    s32 statsB;
    s32 atkA;
    s32 atkB;
    s32 defA;
    s32 defB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (func_8008E590() & 1) {
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if (idA != 0) {
            keyA = D_801D4D8E[idA - 1];
            statsA = D_801D4244[idA - 1];
            if (((statsA >> 26) & 0x1F) < 20) {
                atkA = (statsA & 0x1FF) * 10;
                defA = ((statsA >> 9) & 0x1FF) * 10;
            } else {
                atkA = 0x80000001;
                defA = 0x80000001;
            }
        } else {
            keyA = 0x7FFFFFFF;
            atkA = 0x80000001;
            defA = 0x80000001;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
            statsB = D_801D4244[idB - 1];
            if (((statsB >> 26) & 0x1F) < 20) {
                atkB = (statsB & 0x1FF) * 10;
                defB = ((statsB >> 9) & 0x1FF) * 10;
            } else {
                atkB = 0x80000001;
                defB = 0x80000001;
            }
        } else {
            keyB = 0x7FFFFFFF;
            atkB = 0x80000001;
            defB = 0x80000001;
        }
        if (defB <= defA) {
            if (defA <= defB) {
                if (atkB <= atkA) {
                    if (atkA <= atkB) {
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
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}
