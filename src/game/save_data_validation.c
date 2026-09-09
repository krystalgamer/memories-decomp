#include "../types.h"
#include "save_data.h"

s32 SaveData_HasSameDuelistCode(SaveDataState *left, SaveDataState *right)
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

s32 SaveData_MatchesDuelistAndCurrentSequence(
    SaveDataState *left,
    SaveDataState *right)
{
    s32 result;

    if (SaveData_HasSameDuelistCode(left, right)) {
        result = gSaveDataSequence == right->save_sequence;
    } else {
        result = 0;
    }
    return result;
}
