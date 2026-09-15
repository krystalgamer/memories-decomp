#ifndef MEMORIES_DECOMP_FILE_TRANSFER_STEPS_H
#define MEMORIES_DECOMP_FILE_TRANSFER_STEPS_H

#include "../types.h"
#include "../ygo_types.h"

extern u8 D_8009B058[];
extern u8 D_801DD800[];
extern u8 D_800F5694[];

/* The file-transfer phase callbacks Model_LoadMonsterMerge installs
 * through File_TryRequestAsyncTransfer: func_80056D7C for duel slots,
 * func_80057544 for another monster record and func_800577B0 for the
 * 0x309 special case.
 *
 * That file declared them as `void func(void);`. The declarations were only
 * ever used to take the functions' addresses -- FileTransferCallback is
 * `void (*)()`, which accepts any function pointer -- so neither the wrong
 * parameter count nor the wrong types were ever checked against anything,
 * and the build matched with the old spellings live in the tree. */
void func_80056D7C(FileTransferDescriptor *object, s32 mode);
void func_80057544(FileTransferDescriptor *object, s32 mode);
void func_800577B0(FileTransferDescriptor *object, s32 mode);

#endif
