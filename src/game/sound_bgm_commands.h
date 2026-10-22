#ifndef GAME_SOUND_BGM_COMMANDS_H
#define GAME_SOUND_BGM_COMMANDS_H

#include "../types.h"

#ifdef D_8009B404_IN_DATA
extern u32 gSD_dwSavedBgmCommand __attribute__((section(".data")));
#else
extern u32 gSD_dwSavedBgmCommand;
#endif

void SD_CdBgmPlay(u32 value);

#endif
