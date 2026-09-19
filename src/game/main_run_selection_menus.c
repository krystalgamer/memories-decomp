#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/overworld/campaign_map.h"
#include "func_800339D0.h"
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

void Main_RunCampaignMap(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;D_8009B0A3[0]=10;File_RequestEgyptOverworldPackage();func_8016866C(D_8009B363[0]);Fade_StartIn();}else{CampaignMap_UpdateLocation();if((D_8009B26C&0x40)==0){Main_ClearFrameServiceCallbacks();D_8009B0A3[0]=6;Main_AdvanceFrame();}}}
