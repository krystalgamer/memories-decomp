#ifndef MEMORIES_DECOMP_MAIN_MODE_STATE_H
#define MEMORIES_DECOMP_MAIN_MODE_STATE_H

#include "../types.h"

/* Cross-subsystem frontend mode state.
 *
 * D_8009B0D1 is cleared during startup and when starting a new campaign.
 * Startup keeps its volatile small-data view; the menu transition requires
 * an absolute .data store.
 *
 * gMain_bReturnMode is the next/base mode copied back when a mode runner
 * exits. gMain_bActiveModeState holds the active mode in its low bits plus
 * lifecycle flags: 0x20 for the animated-battle variant, 0x40 for mode
 * initialization, and 0x80 for the main-loop reset gate. Both are declared
 * once in main_mode_controls.h; MAIN_MODE_STATE_{NEXT,ACTIVE}_{IN_DATA,
 * AS_ARRAY,AS_SCALAR} select the addressing form each caller needs. */
#ifdef D_8009B0D1_IN_DATA
extern u8 D_8009B0D1 __attribute__((section(".data")));
#elif defined(D_8009B0D1_IS_VOLATILE)
extern volatile u8 D_8009B0D1;
#else
extern u8 D_8009B0D1;
#endif

#include "main_mode_controls.h"

#endif
