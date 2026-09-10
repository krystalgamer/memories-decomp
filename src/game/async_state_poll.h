#ifndef MEMORIES_DECOMP_ASYNC_STATE_POLL_H
#define MEMORIES_DECOMP_ASYNC_STATE_POLL_H

#include "../types.h"

/* Entry 1 of the frontend step table D_80090D7C (frontend_step_tables.c).
 * On entry it latches bit 0x80 of D_8009B2EB and clears D_8009B3ED and
 * D_8009B3EA. It then polls func_8003FD14 until that returns non-zero, and a
 * result of 1 loads the main-menu package and selects mode 0x10 before the
 * step byte is cleared. */
void func_80031000(void);

/* Entry 0 of both frontend step tables: clears the step byte D_8009B2EB and
 * does nothing else. */
void func_80031078(void);

#endif
