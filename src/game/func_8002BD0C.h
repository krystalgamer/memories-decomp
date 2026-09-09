#ifndef MEMORIES_DECOMP_FUNC_8002BD0C_H
#define MEMORIES_DECOMP_FUNC_8002BD0C_H

#include "../types.h"
#include "file_transfer.h"

/* File transfer completion callback for the stage-image read: `mode` is the step
 * the transfer reports and `object` the descriptor it reports through.
 *
 * Its two parameters are the point. func_8002BFCC declared it `void (void)`
 * because it only ever takes its address for File_RequestAsyncTransfer, which
 * hands both arguments back at callback time; with the real prototype the call
 * site casts to FileTransferCallback, which is what every other callback in the
 * transfer headers already does. */
void func_8002BD0C(FileTransferDescriptor *object, s32 mode);

#endif
