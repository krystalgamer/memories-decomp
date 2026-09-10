#ifndef MEMORIES_DECOMP_FILE_TRANSFER_STEPS_H
#define MEMORIES_DECOMP_FILE_TRANSFER_STEPS_H

#include "../types.h"
#include "../ygo_types.h"

/* The two file-transfer phase callbacks Model_LoadMonsterMerge installs
 * through File_TryRequestAsyncTransfer: func_80057544 for a monster record
 * and func_800577B0 for the 0x309 special case.
 *
 * That file declared both as `void func(void);`. The declarations were only
 * ever used to take the functions' addresses -- FileTransferCallback is
 * `void (*)()`, which accepts any function pointer -- so neither the wrong
 * parameter count nor the wrong types were ever checked against anything,
 * and the build matched with both spellings live in the tree. */
void func_80057544(FileTransferDescriptor *object, s32 mode);
void func_800577B0(FileTransferDescriptor *object, s32 mode);

#endif
