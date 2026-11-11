#ifndef MEMORIES_DECOMP_FRONTEND_PACKAGE_STAGES_H
#define MEMORIES_DECOMP_FRONTEND_PACKAGE_STAGES_H

#include "../types.h"
#include "../ygo_types.h"

void FreeDuel_LoadPackageStage(FileTransferDescriptor *object, s32 mode);

void Main_InitFreeDuelMenu(void);

void NameEntry_LoadPackageStage(FileTransferDescriptor *object, s32 mode);

void Text_SjisToGlyphCodes(u8 *dst, const u8 *src, s32 count);

void Password_LoadPackageStage(FileTransferDescriptor *object, s32 mode);

void CampaignMap_LoadPackageStage(
    FileTransferDescriptor *object,
    s32 mode
);

#endif
