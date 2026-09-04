#include "../types.h"

typedef struct {
    u8 pad_000[0x334];
    s32 field_334;
} SaveRecord;

extern s32 D_8009B3B8;

int func_8003D288(SaveRecord *left, SaveRecord *right)
{
    int i;

    if (left->field_334 == right->field_334) {
        i = 6;
        while (--i >= 0) {
        }
        return 1;
    }
    return 0;
}

int func_8003D2B8(SaveRecord *left, u8 *right)
{
    int result;

    if (func_8003D288(left, (SaveRecord *)right)) {
        result = D_8009B3B8 == *(s32 *)(right + 0x404);
    } else {
        result = 0;
    }
    return result;
}
