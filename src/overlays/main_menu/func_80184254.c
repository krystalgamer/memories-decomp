#include "../../types.h"

typedef struct {
    s16 id;
    u16 rank;
} MainMenuSortEntry;

extern s16 D_801D4D8E[];
extern s32 func_8008E590(void);

s32 func_80184254(MainMenuSortEntry *a, MainMenuSortEntry *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 keyA;
    s32 keyB;
    s32 rankA;
    s32 rankB;

    idA = a->id;
    idB = b->id;
    if (idA == idB) {
        if (func_8008E590() & 1) {
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if (idA != 0) {
            keyA = D_801D4D8E[idA - 1];
            rankA = a->rank;
        } else {
            keyA = 0x7FFFFFFF;
            rankA = 0x80000001;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
            rankB = b->rank;
        } else {
            keyB = 0x7FFFFFFF;
            rankB = 0x80000001;
        }
        if (rankA < rankB) {
            result = 1;
        } else if (rankB < rankA) {
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
