#include "../types.h"
#include "save_data.h"

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
