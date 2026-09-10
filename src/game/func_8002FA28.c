#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
#include "../types.h"
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "main_mode_state.h"
#include "main_services.h"
#include "func_8002FA28.h"

void func_8002FA28(void)
{
    D_8009B268 = 1;
    D_8009B26D = 5;
    D_8009B26C[0] = 8;
}
