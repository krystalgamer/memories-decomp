#ifndef MEMORIES_DECOMP_MEM_CARD_WORK_INITIALIZE_OPERATION_H
#define MEMORIES_DECOMP_MEM_CARD_WORK_INITIALIZE_OPERATION_H

#include "../types.h"

#define FUNC_8003D300_STATE_SIZE 10

/* Six-store setup for the legacy work operation. No I/O is submitted and
 * neither the whole workspace nor the separate modal runtime is reset. */
void MemCardWork_InitializeOperation(u8 value);

#endif
