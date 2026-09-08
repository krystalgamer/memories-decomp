#ifndef MEMORIES_DECOMP_MAIN_MENU_FRONTEND_H
#define MEMORIES_DECOMP_MAIN_MENU_FRONTEND_H

#include "../../types.h"

typedef void (*MainMenuEntryEffectUpdate)(u8 *object);

void MainMenu_StartFrontendEntryTransition(s32 mode);
void MainMenu_DrawFrontendBackground(void);
void MainMenu_SpawnFrontendEntryAfterimage(u8 *source);
void MainMenu_UpdateFrontendEntryAfterimage(u8 *object);

#endif
