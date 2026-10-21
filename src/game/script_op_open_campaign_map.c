#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "secondary_scene_commands.h"
#include "script_state.h"
#include "../unmatched.h"
#include "script_op_open_campaign_map.h"
#include "main_mode_state.h"

void Script_OpOpenCampaignMap(void)
{
    unsigned char value = *gScript_StreamCursor++;

    D_8009B26C[0] = 5;
    gCampaignMap_bEntryLocation[0] = value;
}
