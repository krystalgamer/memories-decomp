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
 * once in main_mode_controls.h; MAIN_MODE_STATE_NEXT_AS_ARRAY and
 * MAIN_MODE_STATE_ACTIVE_AS_SCALAR select the addressing form each caller
 * needs. */
extern u8 D_8009B0D1;

#include "main_mode_controls.h"

#endif
