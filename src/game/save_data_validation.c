#include "../types.h"
#include "save_data.h"

typedef struct {
    u8 pad_000[0x334];
    s32 duelist_code;
} SaveRecord;

extern s32 gSaveDataSequence;

s32 SaveData_HasSameDuelistCode(SaveRecord *left, SaveRecord *right)
{
    s32 i;

    if (left->duelist_code == right->duelist_code) {
        i = 6;
        while (--i >= 0) {
        }
        return 1;
    }
    return 0;
}

s32 SaveData_MatchesDuelistAndCurrentSequence(SaveRecord *left, u8 *right)
{
    s32 result;

    if (SaveData_HasSameDuelistCode(left, (SaveRecord *)right)) {
        result =
            gSaveDataSequence == *(s32 *)(right + SAVE_DATA_SEQUENCE_OFFSET);
    } else {
        result = 0;
    }
    return result;
}
