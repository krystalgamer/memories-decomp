#ifndef MEMORIES_DECOMP_FUNC_80024DC8_H
#define MEMORIES_DECOMP_FUNC_80024DC8_H

#include "../types.h"

/* Arms a duel: stores arg0 into D_8009B360, arg1 into gDuel_bOpponentID and
 * arg2/arg3 into the halfwords D_8009B370/D_8009B372, resets the terrain and
 * sets D_8009B26C to 3.
 *
 * The parameters are s32 even though every store is a byte or a halfword.
 * Retail's Free Duel overlay passes a computed grid index as arg1 without
 * narrowing it, and an s8 parameter makes that caller emit an sll/sra pair
 * retail does not have. The definition stores through sb/sh either way, and
 * the two resident callers pass constants, so neither side moves. */
void func_80024DC8(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

/* Main_RunDuel indexes continuation bytes from this address; the setup
 * function stores the same address as a halfword. */
#ifdef D_8009B370_AS_ABSOLUTE_ARRAY
extern u8 D_8009B370[9];
#else
extern u16 D_8009B370;
#endif

#endif
