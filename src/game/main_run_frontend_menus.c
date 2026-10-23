#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#define D_8009B0D1_IN_DATA
#include "../types.h"
#include "duel_effect_mode_7.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "main_modes.h"
#include "main_services.h"
#include "../overlays/password/name_entry_main.h"
#include "../unmatched.h"
#include "main_mode_state.h"
#define GFREEDUEL_BRETURNFLAGS_IN_DATA
#include "../overlays/free_duel/free_duel.h"
#define D_8009B3D4_IN_DATA
#include "mem_card.h"
#include "save_data.h"
#include "../overlays/main_menu/frontend.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "../overlays/password/shop.h"
#include "../psyq/rand.h"
#include "../psyq/setjmp.h"
#include "fade.h"
#include "game_over.h"
#include "menu_record_reset.h"
#include "sound.h"
#include "options.h"

void Main_ApplyMenuSelection(MainMenuSelection selection)
{
    D_8009B268 = 1;
    gMain_bRequestedMenuID = selection;

    switch (selection) {
    case MAIN_MENU_SELECTION_NEW_GAME:
        File_RequestNameEntryPackage();
        NameEntry_Main();
        gCampaignSavedSceneIndex[0] = 0x30;
        gCampaignSceneIndex = 0x30;
        gSaveData_AllowIdentityBypass = 1;
        D_8009B0D1 = 0;
        gMain_bActiveModeState = MAIN_MODE_CAMPAIGN;
        break;
    case MAIN_MENU_SELECTION_TWO_PLAYER_DUEL:
        gMain_bActiveModeState = MAIN_MODE_TWO_PLAYER_DUEL_SETUP;
        break;
    case MAIN_MENU_SELECTION_TRADE:
        gMain_bActiveModeState = MAIN_MODE_TRADE;
        break;
    case MAIN_MENU_SELECTION_LIBRARY:
        gMain_bActiveModeState = MAIN_MODE_LIBRARY;
        break;
    case MAIN_MENU_SELECTION_CAMPAIGN:
        gCampaignSceneIndex = gCampaignSavedSceneIndex[0];
        gMain_bActiveModeState = MAIN_MODE_CAMPAIGN;
        break;
    case MAIN_MENU_SELECTION_FREE_DUEL:
        gFreeDuel_bReturnFlags = 0;
        gMain_bActiveModeState = MAIN_MODE_FREE_DUEL;
        break;
    case MAIN_MENU_SELECTION_BUILD_DECK:
        BuildDeck_EnterNarrowConfirmMode();
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_OPTIONS:
        gMain_bActiveModeState = MAIN_MODE_OPTIONS;
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_PASSWORD:
        gMain_bActiveModeState = MAIN_MODE_PASSWORD;
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_SAVE:
    default:
        gMain_bActiveModeState = MAIN_MODE_DEBUG;
        break;
    }
}

void Main_RunMenu(void){unsigned char f=gMain_bActiveModeState;int r;if((f&0x40)==0){gMain_bActiveModeState=f|0x40;File_RequestMainMenuPackage();File_WaitForTransfers();DisplayEffect_ResetMenuRecords();MainMenu_InitFrontendMenu(D_8009B268,gMain_bRequestedMenuID);Fade_WaitIn();}rand();r=MainMenu_UpdateFrontendMenu();if(r>=0){SD_BGMFadeOut();Fade_WaitOut();MainMenu_DestroyFrontendMenu();Main_ApplyMenuSelection(r);gMain_bReturnMode=8;}}

u8 gMain_bActiveModeState;

void Main_RunNameEntry(void)
{
    u8 flags = gMain_bActiveModeState;
    u8 value;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        File_RequestNameEntryPackage();
        NameEntry_Init();
    }
    if (NameEntry_PollCompletion()) {
        value = gMain_bReturnMode;
        gMain_bActiveModeState = value;
    }
}

void Main_RunPasswordMenu(void)
{
    unsigned char flags = gMain_bActiveModeState;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        File_RequestPasswordPackage();
        Password_InitShopScreen();
    }
    Password_UpdateShopScreen();
}
