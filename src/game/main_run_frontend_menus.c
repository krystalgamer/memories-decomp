#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "../overlays/main_menu/frontend.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "../overlays/password/shop.h"
#include "../psyq/rand.h"
#include "../psyq/setjmp.h"
#include "fade.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "game_over.h"
#include "main_modes.h"
#include "menu_record_reset.h"
#include "sound.h"
#include "main_services.h"
#include "options.h"
#include "../unmatched.h"
#include "main_mode_state.h"

void Main_RunMenu(void){unsigned char f=gMain_bActiveModeState;int r;if((f&0x40)==0){gMain_bActiveModeState=f|0x40;File_RequestMainMenuPackage();File_WaitForTransfers();DisplayEffect_ResetMenuRecords();MainMenu_InitFrontendMenu(D_8009B268,gMain_bRequestedMenuID);Fade_WaitIn();}rand();r=MainMenu_UpdateFrontendMenu();if(r>=0){SD_BGMFadeOut();Fade_WaitOut();MainMenu_DestroyFrontendMenu();Main_ApplyMenuSelection(r);gMain_bReturnMode=8;}}
