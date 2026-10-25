#ifndef MEMORIES_DECOMP_DATA_TRANSFER_REQUEST_H
#define MEMORIES_DECOMP_DATA_TRANSFER_REQUEST_H

#include "../types.h"

/* Public request API implemented by mem_card_dialog.c. */

/* Polls the memory-card dialog and reports its result, or 0 while a dialog
 * is still up. */
s32 MemCardDialog_Poll(void);

/* Raises the memory-card dialog on step `step` of the gMemCardDialog_apfnSteps table. */
void MemCardDialog_Start(s32 step);

/* Queues a memory-card transfer of size bytes from buf, to or from the file
 * named by name, with step selecting the operation. The callers use steps 0
 * (load), 1 (load without the confirmation prompt), 2 (save), 3 (Japanese
 * options-screen save) and 4. */
void MemCardDialog_Request(u8 *buf, s32 size, u8 *name, s32 step);

#ifdef VERSION_JAPAN
/* Options invokes step 3 for the fixed 0x8000-byte buffer at 0x80140000;
 * this does not establish that the buffer contains only option settings. */
extern void SaveData_RequestOptionsWrite(void) asm("func_8003EFA8");
#endif

#endif
