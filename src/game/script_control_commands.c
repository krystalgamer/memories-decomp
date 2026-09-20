#define MAIN_MODE_STATE_NEXT_AS_ARRAY
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "script_command_busy.h"
#include "script_state.h"
#include "script_control_commands.h"
#include "../unmatched.h"
#include "main_mode_state.h"

void Script_OpGameOver(void)
{
    D_8009B26C[0] = 12;
    D_8009B269[0] = 12;
}

void Script_OpCredits(void)
{
    D_8009B26C[0] = 15;
    D_8009B269[0] = 15;
}

void Script_OpWait(void)
{
    if (ScriptCommand_MarkStarted() == 0) {
        u8 *p = D_8009B290;
        D_8009B290 = p + 2;
        D_8009B278 = p[0] | (p[1] << 8);
    }
    D_8009B278--;
    if (D_8009B278 <= 0)
        D_8009B27C = 0;
}
