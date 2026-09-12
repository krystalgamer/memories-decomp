#ifndef MEMORIES_DECOMP_FILE_CD_HELPERS_H
#define MEMORIES_DECOMP_FILE_CD_HELPERS_H

#include "../types.h"
#include "../psyq/libcd.h"

/* The resident SDK copies use the same three BCD position bytes as CdlLOC.
 * The encoder returns its output pointer; the decoder only reads the input. */
CdlLOC *CdIntToPos_8007E600(s32 sector, CdlLOC *position);
s32 CdPosToInt_8007E710(const CdlLOC *position);

s32 File_Exists(s32 first, s32 second);
s32 Movie_StepPlayback(void);
s32 Movie_MoveDisplayImage(s32 first, s32 second);

#endif
