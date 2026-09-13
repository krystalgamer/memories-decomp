#include "../../types.h"
#include "card_comparators.h"
#include "module_rodata.h"

const MainMenuComparators D_80180004 = {
    {
        (s32 (*)())MainMenu_CompareCardsByName,
        (s32 (*)())MainMenu_CompareCardsByMaxStat,
        (s32 (*)())MainMenu_CompareCardsByAttack,
        (s32 (*)())MainMenu_CompareCardsByDefense,
        (s32 (*)())MainMenu_CompareCardsByType,
        (s32 (*)())MainMenu_CompareCardsByCount,
    }
};
