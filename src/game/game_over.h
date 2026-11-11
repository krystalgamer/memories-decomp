#ifndef MEMORIES_DECOMP_GAME_OVER_H
#define MEMORIES_DECOMP_GAME_OVER_H

#include "../types.h"

void GameOver_Init(void);
s32 GameOver_Update(void);

/* The Game Over title display object, defined by game_over.c and held as a
 * word. */
extern s32 gGameOver_pTitleObject;

#endif
