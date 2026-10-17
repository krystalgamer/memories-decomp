#ifndef MEMORIES_DECOMP_FUNC_80044DC0_H
#define MEMORIES_DECOMP_FUNC_80044DC0_H

#include "../types.h"

/* Scales the two channel volume scalars in g_SDValue by a shared factor
 * derived from a0 and pushes the resulting 4-byte CdMix packet. */
void SD_SetCdVolume(s16 a0);

#endif
