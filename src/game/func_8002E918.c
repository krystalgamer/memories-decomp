#include "../types.h"
#include "campaign_flags.h"
#include "script_state.h"
#include "script_flag_commands.h"

void func_8002E918(void)
{
    u8 *cursor = D_8009B290;
    u8 *next = cursor + 2;
    u16 command;

    D_8009B290 = next;
    command = cursor[0] | (cursor[1] << 8);
    if (command & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(command & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
    } else {
        u32 offset;

        D_8009B290 = cursor + 4;
        offset = cursor[2] | (next[1] << 8);
        if (Campaign_TestStoryFlag(command))
            D_8009B290 = D_801A8000 + offset;
    }
    D_8009B27C = 0;
}
