#ifndef MEMORIES_DECOMP_DUEL_SCREEN_TABLES_H
#define MEMORIES_DECOMP_DUEL_SCREEN_TABLES_H

#include "../types.h"
#include "func_80020F4C.h"

/* The card-slot projection coordinates and the two duel-result sprite
   tables, declared here so the source that defines them and the three that
   read them cannot drift apart. */
extern u16 D_800908A0[];
extern u8 D_80090918[];
extern DuelResultSpriteSpec D_80090928[][DUEL_RESULT_SPRITE_COUNT];
extern DuelResultSpriteSpec D_80090960[][DUEL_RESULT_SPRITE_COUNT];

#endif
