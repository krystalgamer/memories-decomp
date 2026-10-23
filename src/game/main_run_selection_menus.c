#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "secondary_scene_commands.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/overworld/campaign_map.h"
#include "build_deck_update.h"
#include "file_transfer.h"
#include "save_data.h"
#include "main_frame.h"
#include "main_init_free_duel_menu.h"
#include "main_services.h"
#include "main_modes.h"
#include "fade.h"
#include "sound.h"
#include "../unmatched.h"

#include "high_memory_addresses.h"
#include "main_mode_state.h"

void Main_RunCampaignMap(void){unsigned char f=gMain_bActiveModeState;if((f&0x40)==0){gMain_bActiveModeState=f|0x40;gGraphics_bOrderingTable3Length[0]=10;File_RequestEgyptOverworldPackage();CampaignMap_SetLocation(gCampaignMap_bEntryLocation[0]);Fade_StartIn();}else{CampaignMap_UpdateLocation();if((gMain_bActiveModeState&0x40)==0){Main_ClearFrameServiceCallbacks();gGraphics_bOrderingTable3Length[0]=6;Main_AdvanceFrame();}}}
