#ifndef GAME_MAIN_MODE_CONTROLS_H
#define GAME_MAIN_MODE_CONTROLS_H

#include "../types.h"

#if defined(MAIN_MODE_STATE_NEXT_IN_DATA) || defined(D_8009B269_AS_SCALAR_DATA)
extern u8 gMain_bReturnMode __attribute__((section(".data")));
#elif defined(MAIN_MODE_STATE_NEXT_AS_ARRAY) || defined(D_8009B269_AS_ARRAY)
extern u8 gMain_bReturnMode[];
#else
extern u8 gMain_bReturnMode;
#endif

#if defined(MAIN_MODE_STATE_ACTIVE_IN_DATA) || defined(D_8009B26C_AS_SCALAR_DATA)
extern u8 gMain_bActiveModeState __attribute__((section(".data")));
#elif defined(MAIN_MODE_STATE_ACTIVE_AS_SCALAR) || defined(D_8009B26C_AS_SCALAR)
extern u8 gMain_bActiveModeState;
#else
extern u8 gMain_bActiveModeState[];
#endif

#endif
