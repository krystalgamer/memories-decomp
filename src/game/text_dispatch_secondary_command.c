#include "../types.h"
#include "duel_effect_command_table.h"
#include "text_dispatch_secondary_command.h"

void Text_DispatchSecondaryCommand(u8 *arg0)
{
    u8 **pp = (u8 **)(arg0 + *(s8 *)(arg0 + 0x58) * 4);
    u8 *p = *pp;
    s32 op = *p;

    *pp = p + 1;
    D_80090EAC[op](arg0);
}
