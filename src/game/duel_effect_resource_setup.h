#ifndef MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_SETUP_H
#define MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_SETUP_H

#include "../types.h"
#include "file_transfer.h"

FileTransferDescriptor *func_80029164(s32 slot, s32 value);

/* Async phase callback for func_80029164's seven-sector request. Phase 0
 * points both buffer words at D_8009B118 for 0x3800 bytes; the next phase
 * uploads the four rects of the D_800EA0E8 entry whose index func_80029164
 * left in callback_data. */
void func_800289BC(FileTransferDescriptor *descriptor, s32 mode);

#endif
