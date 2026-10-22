#ifndef GAME_SAVE_DATA_WORKSPACE_H
#define GAME_SAVE_DATA_WORKSPACE_H

#include "../types.h"
#include "save_data.h"

#ifdef SAVE_DATA_WORKSPACE_AS_HALFWORDS
extern s16 gSaveData_Workspace[];
#else
extern u8 gSaveData_Workspace[];
#endif

s32 Library_CheckCardInChest(s32 card_id);

#endif
