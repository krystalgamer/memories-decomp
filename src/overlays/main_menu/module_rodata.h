#ifndef MAIN_MENU_MODULE_RODATA_H
#define MAIN_MENU_MODULE_RODATA_H

#include "../../types.h"

typedef struct {
    s32 (*entries[6])();
} MainMenuComparators;

extern const MainMenuComparators gMainMenu_aCardSortComparators;

#endif
