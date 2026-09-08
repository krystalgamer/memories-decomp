#ifndef MEMORIES_DECOMP_DATA_TRANSFER_REQUEST_H
#define MEMORIES_DECOMP_DATA_TRANSFER_REQUEST_H

#include "../types.h"

/* Polls the memory-card dialog and reports its result, or 0 while a dialog
 * is still up. */
s32 func_8003F70C(void);

/* Raises the memory-card dialog for the given operation. */
void func_8003F740(s32 value);

/* Queues a memory-card transfer of arg1 bytes from data, to or from the
 * file named by name, with arg3 selecting the operation. */
void func_8003F758(void *data, s32 arg1, u8 *name, s32 arg3);

#endif
