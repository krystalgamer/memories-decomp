#ifndef MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_SETUP_H
#define MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_SETUP_H

#include "../types.h"
#include "file_transfer.h"

FileTransferDescriptor *func_80029164(s32 slot, s32 value);

/* Async phase callback for func_80029164's seven-sector request. It retains
 * the definition's byte view of the transfer descriptor. */
void func_800289BC(u8 *descriptor, s32 mode);

#endif
