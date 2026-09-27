#include "../types.h"
#include "text_constants.h"

#ifdef VERSION_EUROPE
u32 Text_LookupString(s32 arg0, s32 arg1)
{
    /* The European banks, as in TextBox_BuildStep. */
    if (arg1 <= 0x7FFF) {
        if (arg1 >= 0x500) {
            arg1 -= 0x100;
            return ((u32)D_801C0004 & TEXT_BANK_ADDRESS_MASK) |
                D_801B0004[arg1];
        }
        return ((u32)D_801B0004 & TEXT_BANK_ADDRESS_MASK) | D_801B0004[arg1];
    }
    return ((u32)D_801D5804 & TEXT_BANK_ADDRESS_MASK) |
        D_801D5804[arg1 - 0x8000];
}
#else
u32 Text_LookupString(s32 arg0, s32 arg1)
{
    s32 index = arg1;
#ifdef VERSION_JAPAN_TEXT_LOOKUP_STRING
    if (index & TEXT_GLOBAL_STRING_ID_BASE)
        return ((u32)D_801D6000 & TEXT_BANK_ADDRESS_MASK) |
            D_801D6000[index & (TEXT_GLOBAL_STRING_ID_BASE - 1)];
#else
    if (index > 0xCFFF)
        return ((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) |
            D_801C0000[index - 0xD000];
    if (index > (TEXT_GLOBAL_STRING_ID_BASE - 1))
        return ((u32)D_801D5800 & TEXT_BANK_ADDRESS_MASK) |
            D_801D5800[index - TEXT_GLOBAL_STRING_ID_BASE];
#endif
    if (index >= 0x500)
        index -= 0x100;
#ifdef VERSION_JAPAN_TEXT_LOOKUP_STRING
    return ((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) | D_801C0000[index];
#else
    return ((u32)D_801B0000 & TEXT_BANK_ADDRESS_MASK) | D_801C0000[index];
#endif
}
#endif
