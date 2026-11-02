#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
#include "../types.h"
#include "main_services.h"
#include "script_op_return_to_menu.h"
#include "../unmatched.h"
#include "main_mode_state.h"

void Script_OpReturnToMenu(void)
{
    D_8009B268 = 1;
    gMain_bRequestedMenuID = 5;
    gMain_bActiveModeState[0] = 8;
}
