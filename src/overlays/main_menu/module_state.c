#include "../../types.h"
#include "module_state.h"

#define MAIN_MENU_LOADED_DATA __attribute__((section(".data")))

MainMenuFrontendState gMainMenu_FrontendState MAIN_MENU_LOADED_DATA = {
    0,
    0,
    0,
    0,
    {0},
    0,
    0,
    0,
    0,
    0,
    0,
    {0},
    {4, 0x24}
};

MainMenuValueSetupState gMainMenu_ValueSetupState MAIN_MENU_LOADED_DATA = {0};
MainMenuTradeState gMainMenu_TradeState MAIN_MENU_LOADED_DATA = {0};

#undef MAIN_MENU_LOADED_DATA
