#ifndef MEMORIES_DECOMP_MAIN_DEBUG_H
#define MEMORIES_DECOMP_MAIN_DEBUG_H

#include "../types.h"

/* Resets the frontend runtime once, before Main_Loop begins dispatching
 * modes. It is not itself a mode runner: main_modes.h owns those, including
 * this translation unit's other function, Main_RunDebugMenu. */
void func_8002CDE8(void);

#endif
