#ifndef GAME_MAIN_MENU_REQUEST_H
#define GAME_MAIN_MENU_REQUEST_H

#include "../types.h"

#ifdef D_8009B26D_IN_DATA
extern u8 gMain_bRequestedMenuID __attribute__((section(".data")));
#else
extern u8 gMain_bRequestedMenuID;
#endif

#endif
