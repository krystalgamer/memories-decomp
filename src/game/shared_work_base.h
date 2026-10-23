#ifndef GAME_SHARED_WORK_BASE_H
#define GAME_SHARED_WORK_BASE_H

#include "../types.h"

#ifdef HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
extern u8 *G32 gRuntime_pSharedWorkBase __attribute__((section(".data")));
#else
extern u8 *G32 gRuntime_pSharedWorkBase;
#endif

#endif
