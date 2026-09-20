#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "script_state.h"
#include "../unmatched.h"
#include "func_8002EB48.h"
#include "main_mode_state.h"

void Script_OpOpenCampaignMap(void)
{
    unsigned char value = *D_8009B290++;

    D_8009B26C[0] = 5;
    D_8009B363[0] = value;
}
