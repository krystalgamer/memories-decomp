#ifndef MEMORIES_DECOMP_DUEL_SCREEN_TABLES_H
#define MEMORIES_DECOMP_DUEL_SCREEN_TABLES_H

#include "../types.h"
#include "duel_result_outro.h"

/* The card-slot projection coordinates and the two duel-result sprite
   tables, declared here so the source that defines them and the units that
   read them cannot drift apart. D_800908A0 is read as u16 halfwords by
   debug_effect_screen.c, src/candidates/func_800177C4.c and
   src/candidates/func_8001B0CC.c (the last
   through inline assembly); duel_card_record_lifecycle.c keeps its own
   DuelFieldPosition view of the same table, because func_80024D34 loads the
   pair with lh. */
extern u16 D_800908A0[];
extern u8 D_80090918[];
extern DuelResultSpriteSpec D_80090928[][DUEL_RESULT_SPRITE_COUNT];
extern DuelResultSpriteSpec D_80090960[][DUEL_RESULT_SPRITE_COUNT];

/* Projects one slot-coordinate pair and returns the signed screen X. */
s16 func_8001B0CC(s32 index);

#endif
