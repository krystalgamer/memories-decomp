#ifndef MEMORIES_DECOMP_IO_EVENT_HELPERS_H
#define MEMORIES_DECOMP_IO_EVENT_HELPERS_H

#include "../types.h"

/* Both take one card class's four result-event handles, in the order
 * MemCard_InitIOEvents registers them: I/O end, timeout, error, new card.
 * gMemCard_aIOEventHandles is the SwCARD set and D_800F2AF0 the HwCARD set. */
void MemCard_ClearIOEvents(long *handles);
s32 MemCard_WaitIOEvent(long *handles, s32 once);
void MemCard_Init(long val);

#endif
