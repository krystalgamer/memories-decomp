#include "../types.h"
#include "text_constants.h"

u32 Text_LookupString(s32 arg0, s32 arg1)
{
    s32 index = arg1;
    if (index > 0xCFFF)
        return ((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) |
            D_801C0000[index - 0xD000];
    if (index > (TEXT_GLOBAL_STRING_ID_BASE - 1))
        return ((u32)D_801D5800 & TEXT_BANK_ADDRESS_MASK) |
            D_801D5800[index - TEXT_GLOBAL_STRING_ID_BASE];
    if (index >= 0x500)
        index -= 0x100;
    return ((u32)D_801B0000 & TEXT_BANK_ADDRESS_MASK) | D_801C0000[index];
}
