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
void Main_LoadBootImageStage(FileTransferDescriptor *descriptor, s32 mode);

#endif
