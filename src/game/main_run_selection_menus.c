#define GRAPHICS_ORDERING_TABLE_LENGTHS_UNSIZED
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#define D_8009B2F8_AS_ARRAY
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
#include "graphics_frame.h"
#include "main_modes.h"
#include "fade.h"
#include "sound.h"
#include "../unmatched.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "main_mode_state.h"
#include "build_deck_init_screen.h"
#include "build_deck_draw_card_list_page_data.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bReturnMode;

void Main_RunCampaignMap(void){unsigned char f=gMain_bActiveModeState;if((f&0x40)==0){gMain_bActiveModeState=f|0x40;gGraphics_abOrderingTableLengths[3]=10;File_RequestEgyptOverworldPackage();CampaignMap_SetLocation(gCampaignMap_bEntryLocation[0]);Fade_StartIn();}else{CampaignMap_UpdateLocation();if((gMain_bActiveModeState&0x40)==0){Main_ClearFrameServiceCallbacks();gGraphics_abOrderingTableLengths[3]=6;Main_AdvanceFrame();}}}

/* The common definition makes MASPSX preserve the load-delay nop before the
 * final gMain_bActiveModeState store. c_symbols.ld supplies the retail storage address. */
u8 gMain_bActiveModeState;

void Main_RunBuildDeckMenu(void)
{
    unsigned char flags = gMain_bActiveModeState;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        BuildDeck_InitScreen(
            (BuildDeckTransitionState *)gRuntime_pSharedWorkBase,
            (unsigned char *)gDuel_awPlayerDeck,
            0,
            gBuildDeck_bConfirmMode[0]
        );
        Fade_WaitIn();
    } else if (BuildDeck_Update() == 0) {
        unsigned char value;

        SD_BGMFadeOut();
        Fade_WaitOut();
        value = gMain_bReturnMode;
        gMain_bActiveModeState = value;
    }
}

void Main_RunFreeDuelMenu(void) {
    unsigned char flags = gMain_bActiveModeState;
    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        Main_InitFreeDuelMenu();
        Fade_WaitIn();
    }
    FreeDuel_Entry();
    if ((gMain_bActiveModeState & 0x40) == 0) SD_BGMFadeOut();
}
