#ifndef MEMORIES_DECOMP_PSYQ_LIBGS_ABI_VARIANTS_H
#define MEMORIES_DECOMP_PSYQ_LIBGS_ABI_VARIANTS_H

#include "../types.h"

/* Measured caller view for the byte-oriented func_80029EC4 candidate. */
void GsSortFastSprite(u8 *sprite, s32 ordering_table, s32 priority);
void GsSortGLine(u8 *line, s32 ordering_table, s32 priority);

#endif
