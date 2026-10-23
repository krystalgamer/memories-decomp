#ifndef GAME_DISPLAY_OBJECT_DEPTH_BASES_H
#define GAME_DISPLAY_OBJECT_DEPTH_BASES_H

#include "../types.h"

#ifdef D_8009AF74_IN_DATA
extern volatile u16 gDisplayObject_awOrderingTableDepthBases[4] __attribute__((section(".data")));
#else
extern volatile u16 gDisplayObject_awOrderingTableDepthBases[4] __attribute__((section(".sdata")));
#endif

#endif
