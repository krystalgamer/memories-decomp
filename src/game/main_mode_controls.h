#ifndef GAME_MAIN_MODE_CONTROLS_H
#define GAME_MAIN_MODE_CONTROLS_H

#include "../types.h"

#if defined(MAIN_MODE_STATE_NEXT_AS_ARRAY)
extern u8 gMain_bReturnMode[];
#else
extern u8 gMain_bReturnMode;
#endif

#if defined(MAIN_MODE_STATE_ACTIVE_AS_SCALAR)
extern u8 gMain_bActiveModeState;
#else
extern u8 gMain_bActiveModeState[];
#endif

#endif
