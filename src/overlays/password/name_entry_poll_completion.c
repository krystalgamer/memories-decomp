#include "../../types.h"
#include "name_entry_keyboard.h"

extern u8 D_8016D400;

s32 NameEntry_PollCompletion(void)
{
    NameEntry_UpdateDialog();
    return D_8016D400 & 0x10;
}
