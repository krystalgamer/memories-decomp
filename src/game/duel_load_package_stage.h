#ifndef MEMORIES_DECOMP_DUEL_LOAD_PACKAGE_STAGE_H
#define MEMORIES_DECOMP_DUEL_LOAD_PACKAGE_STAGE_H

#include "../types.h"
#include "file_transfer.h"

/* The staged loader for the duel terrain package. Its two consumers never call
 * it: duel_scene.c and duel_scene.c each pass its
 * address as the
 * per-stage callback of a transfer request, so the transfer machinery supplies
 * both arguments later. That is why both could declare it `(void)` without the
 * build noticing. */
void Duel_LoadPackageStage(FileTransferDescriptor *d, s32 stage);

#include "file_staging_buffer.h"

extern u8 gAiScript_abFieldScript[];

#endif
