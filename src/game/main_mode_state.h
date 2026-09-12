#ifndef MEMORIES_DECOMP_MAIN_MODE_STATE_H
#define MEMORIES_DECOMP_MAIN_MODE_STATE_H

#include "../types.h"

/* D_8009B269 is the next/base mode copied back on exit. D_8009B26C holds
 * the active mode and lifecycle flags. Select the independently measured
 * scalar, absolute DATA, or absolute array view before including this header. */
#if defined(MAIN_MODE_STATE_NEXT_IN_DATA)
extern u8 D_8009B269 __attribute__((section(".data")));
#elif defined(MAIN_MODE_STATE_NEXT_AS_ARRAY)
extern u8 D_8009B269[];
#else
extern u8 D_8009B269;
#endif

#if defined(MAIN_MODE_STATE_ACTIVE_IN_DATA)
extern u8 D_8009B26C __attribute__((section(".data")));
#elif defined(MAIN_MODE_STATE_ACTIVE_AS_ARRAY)
extern u8 D_8009B26C[];
#else
extern u8 D_8009B26C;
#endif

#endif
