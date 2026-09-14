#ifndef MEMORIES_DECOMP_MAIN_MENU_ENTRYPOINTS_H
#define MEMORIES_DECOMP_MAIN_MENU_ENTRYPOINTS_H

#include "../../types.h"

/* Resident callers must load the main-menu image before using these entries.
 * Frontend and Trade lifecycle declarations live in their owning headers. */
void MainMenu_StartValueSetup(u16 *first, u16 *second, u8 *toggle);
s32 MainMenu_UpdateValueSetup(void);
void MainMenu_FinishValueSetup(void);

#endif
