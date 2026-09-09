#include "../../types.h"
#include "name_entry_keyboard.h"
#include "name_entry_state.h"

s32 NameEntry_PollCompletion(void)
{
    NameEntry_UpdateDialog();
    return D_8016D400 & 0x10;
}
