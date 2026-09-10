#ifndef MEMORIES_DECOMP_FUNC_800383DC_H
#define MEMORIES_DECOMP_FUNC_800383DC_H

#include "duel_effect.h"

/* D_80090EAC entry: resolves the text bank pointer for the string id in
 * D_8009B32E -- above 0xCFFF from D_801C0000, above the global-string base from
 * D_801D5800, otherwise the campaign bank -- and hands back the slot it found.
 *
 * Its parameter is the DuelEffectChannel whose stream_58 byte selects one of
 * the pointer words at the front of the record. It returns the selected slot,
 * although the generic command table discards that result. */
u32 *func_800383DC(DuelEffectChannel *channel);

#endif
