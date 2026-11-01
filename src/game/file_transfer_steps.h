#ifndef MEMORIES_DECOMP_FILE_TRANSFER_STEPS_H
#define MEMORIES_DECOMP_FILE_TRANSFER_STEPS_H

#include "../types.h"
#include "../ygo_types.h"

extern u8 gMainMenu_PackageStageRect[];
extern u8 gModel_abAuxiliaryPaletteSector[];
extern u8 gModel_bAuxiliaryLoadStep[];

/* The file-transfer phase callbacks Model_LoadMonsterMerge installs
 * through File_TryRequestAsyncTransfer: Model_LoadMonsterMergeStage for duel slots,
 * Model_LoadAuxiliaryPackageStage for another monster record and Model_LoadSpecialBattlePackageStage for the
 * 0x309 special case.
 *
 * That file declared them as `void func(void);`. The declarations were only
 * ever used to take the functions' addresses -- FileTransferCallback is
 * `void (*)()`, which accepts any function pointer -- so neither the wrong
 * parameter count nor the wrong types were ever checked against anything,
 * and the build matched with the old spellings live in the tree. */
void Model_LoadMonsterMergeStage(FileTransferDescriptor *object, s32 mode);
void Model_LoadAuxiliaryPackageStage(FileTransferDescriptor *object, s32 mode);
void Model_LoadSpecialBattlePackageStage(FileTransferDescriptor *object, s32 mode);

#endif
