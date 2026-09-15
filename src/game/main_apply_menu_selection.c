#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#define D_8009B0D1_IN_DATA
#include "../types.h"
#include "duel_effect_mode_7.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "main_modes.h"
#include "main_services.h"
#include "../external_funcs.h"
#include "../unmatched.h"
#include "main_mode_state.h"
#define GFREEDUEL_BRETURNFLAGS_IN_DATA
#include "../overlays/free_duel/free_duel.h"
#define D_8009B3D4_IN_DATA
#include "mem_card.h"

extern s16 gCampaignSavedSceneIndex[];

void Main_ApplyMenuSelection(MainMenuSelection selection)
{
    D_8009B268 = 1;
    D_8009B26D = selection;

    switch (selection) {
    case MAIN_MENU_SELECTION_NEW_GAME:
        File_RequestNameEntryPackage();
        func_8016AA6C();
        gCampaignSavedSceneIndex[0] = 0x30;
        gCampaignSceneIndex = 0x30;
        D_8009B3D4 = 1;
        D_8009B0D1 = 0;
        D_8009B26C = MAIN_MODE_CAMPAIGN;
        break;
    case MAIN_MENU_SELECTION_TWO_PLAYER_DUEL:
        D_8009B26C = MAIN_MODE_TWO_PLAYER_DUEL_SETUP;
        break;
    case MAIN_MENU_SELECTION_TRADE:
        D_8009B26C = MAIN_MODE_TRADE;
        break;
    case MAIN_MENU_SELECTION_LIBRARY:
        D_8009B26C = MAIN_MODE_LIBRARY;
        break;
    case MAIN_MENU_SELECTION_CAMPAIGN:
        gCampaignSceneIndex = gCampaignSavedSceneIndex[0];
        D_8009B26C = MAIN_MODE_CAMPAIGN;
        break;
    case MAIN_MENU_SELECTION_FREE_DUEL:
        gFreeDuel_bReturnFlags = 0;
        D_8009B26C = MAIN_MODE_FREE_DUEL;
        break;
    case MAIN_MENU_SELECTION_BUILD_DECK:
        func_80033C90();
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_OPTIONS:
        D_8009B26C = MAIN_MODE_OPTIONS;
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_PASSWORD:
        D_8009B26C = MAIN_MODE_PASSWORD;
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_SAVE:
    default:
        D_8009B26C = MAIN_MODE_DEBUG;
        break;
    }
}
