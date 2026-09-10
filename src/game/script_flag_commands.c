#include "../types.h"
#include "campaign_flags.h"
#include "script_state.h"
#include "script_flag_commands.h"

void Script_OpViewportTween(void)
{
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B2A8 = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B2AA = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B29C = cursor[0] | (cursor[1] << 8);
    }
    D_8009B27C = 7;
}
