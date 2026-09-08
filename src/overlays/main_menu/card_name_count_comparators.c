#include "../../types.h"
#include "../../psyq/rand.h"

typedef struct {
    s16 id;
    u16 count;
} MainMenuSortEntry;

extern s16 D_801D4D8E[];

s32 MainMenu_CompareCardsByName(s16 *a, s16 *b)
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

s32 MainMenu_CompareCardsByCount(MainMenuSortEntry *a, MainMenuSortEntry *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 keyA;
    s32 keyB;
    s32 countKeyA;
    s32 countKeyB;

    idA = a->id;
    idB = b->id;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if (idA != 0) {
            keyA = D_801D4D8E[idA - 1];
            countKeyA = a->count;
        } else {
            keyA = 0x7FFFFFFF;
            countKeyA = 0x80000001;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
            countKeyB = b->count;
        } else {
            keyB = 0x7FFFFFFF;
            countKeyB = 0x80000001;
        }
        if (countKeyA < countKeyB) {
            result = 1;
        } else if (countKeyB < countKeyA) {
            result = -1;
        } else if (keyB < keyA) {
            result = 1;
        } else if (keyA < keyB) {
            result = -1;
        } else {
            result = 0;
        }
    }
    return result;
}
