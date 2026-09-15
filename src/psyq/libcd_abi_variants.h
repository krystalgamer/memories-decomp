#ifndef MEMORIES_DECOMP_PSYQ_LIBCD_ABI_VARIANTS_H
#define MEMORIES_DECOMP_PSYQ_LIBCD_ABI_VARIANTS_H

#include "../types.h"
#include "libcd.h"

/* Address-qualified interfaces for the second resident copies. */
CdlLOC *CdIntToPos_8007E600(s32 sector, CdlLOC *position);
s32 CdPosToInt_8007E710(const CdlLOC *position);

#endif
