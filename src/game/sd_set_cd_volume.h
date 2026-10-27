#ifndef MEMORIES_DECOMP_SD_SET_CD_VOLUME_H
#define MEMORIES_DECOMP_SD_SET_CD_VOLUME_H

#include "../types.h"

/* Scales the two channel volume scalars in g_SDValue by a shared factor
 * derived from level and pushes the resulting 4-byte CdMix packet. */
void SD_SetCdVolume(s16 level);

#endif
