#ifndef MEMORIES_DECOMP_DUEL_DRAW_RESOLUTION_H
#define MEMORIES_DECOMP_DUEL_DRAW_RESOLUTION_H

#include "../types.h"

/* The records used to read the staged card table. duel_draw_resolution.c
 * consumes both fields below for the Exodia check; func_80027DF8 shares the
 * database view to export card IDs into the AI active-card table. */

/* One entry of the staged card table. Only the two fields the Exodia check
 * reads are named. This is NOT the game's full card record -- it is the view
 * needed to compare an id and step by the table's stride. */
typedef struct {
    s16 id;
    u8 type;
    u8 pad3[3];
} ExodiaCardRecord;

/* The staged card table reached from D_8015C424. The leading pad is how the
 * table's base is reached at its true offset rather than a claim that
 * 0x4B9FC bytes of header exist; cards[1] is a flexible tail, not a
 * one-element array, and the Exodia scan indexes it with hand entries. */
typedef struct {
    u8 pad0[0x4B9FC];
    ExodiaCardRecord cards[1];
} ExodiaCardDatabase;

/* Returns nonzero when all five Exodia pieces are in hand. It consumes a local
 * copy of the hand and blanks each match, so a single hand entry cannot
 * satisfy two pieces. */
s32 Duel_HasAllExodiaPieces(void);

void func_80018DB4(void);

#endif
