#ifndef GAME_SHARED_WORK_BASE_H
#define GAME_SHARED_WORK_BASE_H

#include "../types.h"

/* Defined const by high_memory_addresses.c at 0x80010000. Readers see a
   plain pointer: a const view lets GCC sink DuelEffect_AllocateRequest's load
   below its first store. */
extern u8 *gRuntime_pSharedWorkBase;

#endif
