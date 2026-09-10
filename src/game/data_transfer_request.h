#ifndef MEMORIES_DECOMP_DATA_TRANSFER_REQUEST_H
#define MEMORIES_DECOMP_DATA_TRANSFER_REQUEST_H

#include "../types.h"

/* Polls the memory-card dialog and reports its result, or 0 while a dialog
 * is still up. */
s32 MemCardDialog_Poll(void);

/* Raises the memory-card dialog on step `step` of the D_80090F9C table. */
void MemCardDialog_Start(s32 step);

/* Queues a memory-card transfer of size bytes from buf, to or from the file
 * named by name, with step selecting the operation. The callers use steps 0
 * (load), 1 (load without the confirmation prompt), 2 (save) and 4. */
void MemCardDialog_Request(void *buf, s32 size, u8 *name, s32 step);

#endif
