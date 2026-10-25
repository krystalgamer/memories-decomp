#ifndef MEMORIES_DECOMP_MAIN_BOOT_LOAD_STAGES_H
#define MEMORIES_DECOMP_MAIN_BOOT_LOAD_STAGES_H

#include "../types.h"
#include "file_transfer.h"

/* Main_RunBootSequence supplies these callbacks to the existing boot package
 * and image requests. The Japanese image entry keeps its native alias in
 * both the caller and implementation adapters. */
void Main_LoadBootPackageStage(
    FileTransferDescriptor *descriptor, s32 stage
);
#ifdef VERSION_JAPAN
extern void Main_LoadBootImageStage(FileTransferDescriptor *descriptor, s32 mode)
    asm("func_80042B64");
#else
void Main_LoadBootImageStage(FileTransferDescriptor *descriptor, s32 mode);
#endif

#endif
