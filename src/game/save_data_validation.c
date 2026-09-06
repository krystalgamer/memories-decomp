#include "../types.h"

typedef struct {
    u8 pad_000[0x334];
    s32 duelist_code;
} SaveRecord;

extern s32 D_8009B3B8;

int SaveData_HasSameDuelistCode(SaveRecord *left, SaveRecord *right)
{
    int i;

    if (left->duelist_code == right->duelist_code) {
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

    if (SaveData_HasSameDuelistCode(left, (SaveRecord *)right)) {
        result = D_8009B3B8 == *(s32 *)(right + 0x404);
    } else {
        result = 0;
    }
    return result;
}
