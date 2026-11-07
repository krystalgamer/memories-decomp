#include "../types.h"
#include "secondary_scene_commands.h"
#include "script_state.h"
#include "../unmatched.h"
#include "script_op_open_campaign_map.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 *gScript_StreamCursor;

void Script_OpOpenCampaignMap(void)
{
    unsigned char value = *gScript_StreamCursor++;

    gMain_bActiveModeState = 5;
    gCampaignMap_bEntryLocation = value;
}
