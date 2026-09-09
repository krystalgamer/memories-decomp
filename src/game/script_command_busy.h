#ifndef MEMORIES_DECOMP_SCRIPT_COMMAND_BUSY_H
#define MEMORIES_DECOMP_SCRIPT_COMMAND_BUSY_H

#include "../types.h"

/* One-shot latch on bit 15 of D_8009B27C. Returns 0 the first time it is
 * called after the word is cleared, and 1 on every call after that, so a
 * script command handler runs its body once and then reports busy. Callers
 * uniformly spell the test as func_8002E3B4() == 0. */
s32 func_8002E3B4(void);

void func_8002E3DC(void);

#endif
