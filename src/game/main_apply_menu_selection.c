#include "../types.h"
#include "duel_effect_mode_7.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "main_modes.h"
#include "main_services.h"
#include "../external_funcs.h"

extern u8 D_8009B26C;
extern s16 gCampaignSavedSceneIndex[];
extern u8 gCampaignSceneIndex __attribute__((section(".data")));
extern u8 D_8009B3D4 __attribute__((section(".data")));
extern u8 gFreeDuel_bReturnFlags __attribute__((section(".data")));

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
