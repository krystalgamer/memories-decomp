#ifndef MAIN_MENU_MODULE_RODATA_H
#define MAIN_MENU_MODULE_RODATA_H

#include "../../types.h"

typedef struct {
    s32 (*entries[6])();
} MainMenuComparators;

extern const MainMenuComparators D_80180004;

#endif
