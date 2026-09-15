#ifndef MEMORIES_DECOMP_FILE_CD_HELPERS_H
#define MEMORIES_DECOMP_FILE_CD_HELPERS_H

#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libcd_abi_variants.h"

s32 File_Exists(const char *path, CdlFILE *file);
s32 Movie_StepPlayback(void);
s32 Movie_MoveDisplayImage(s32 first, s32 second);

#endif
