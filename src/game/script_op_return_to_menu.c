#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
#include "../types.h"
#include "main_services.h"
#include "script_op_return_to_menu.h"

extern u8 D_8009B26C[];

void Script_OpReturnToMenu(void)
{
    D_8009B268 = 1;
    D_8009B26D = 5;
    D_8009B26C[0] = 8;
}
