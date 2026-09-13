#ifndef MEMORIES_DECOMP_DUEL_LOAD_PACKAGE_STAGE_H
#define MEMORIES_DECOMP_DUEL_LOAD_PACKAGE_STAGE_H

#include "../types.h"
#include "file_transfer.h"

/* The staged loader for the duel terrain package. Its two consumers never call
 * it: func_800179F4.c and func_8001798C.c each pass its
 * address as the
 * per-stage callback of a transfer request, so the transfer machinery supplies
 * both arguments later. That is why both could declare it `(void)` without the
 * build noticing. */
void Duel_LoadPackageStage(FileTransferDescriptor *d, s32 stage);

/* Address-only transfer views, not the records used by other arena clients. */
#ifdef DUEL_PACKAGE_STAGE_RAW_ARENAS
extern u8 D_801A8000[];
extern u8 D_801A9800[];
#endif

#endif
