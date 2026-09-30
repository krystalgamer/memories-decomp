#ifndef MEMORIES_DECOMP_IO_EVENT_HELPERS_H
#define MEMORIES_DECOMP_IO_EVENT_HELPERS_H

#include "../types.h"

/* Both take one card class's four result-event handles, in the order
 * MemCard_InitIOEvents registers them: I/O end, timeout, error, new card.
 * gMemCard_aIOEventHandles is the SwCARD set and gMemCard_aHwIOEventHandles
 * the HwCARD set. */
void MemCard_ClearIOEvents(PSXLONG *handles);
s32 MemCard_WaitIOEvent(PSXLONG *handles, s32 once);
void MemCard_Init(PSXLONG val);
void MemCard_InitIOEvents(void);

#endif
