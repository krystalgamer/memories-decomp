#define D_8009B268 gJapanese_bMenuRequest
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B26D gJapanese_bRequestedMenu
#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../../types.h"
#include "../main_services.h"
#include "../script_op_return_to_menu.h"
#include "../../unmatched.h"
#include "../main_mode_state.h"

void Script_OpReturnToMenu(void)
{
    gJapanese_bMenuRequest = 1;
    gJapanese_bRequestedMenu = 5;
    gJapanese_bActiveMainMode[0] = 8;
}
