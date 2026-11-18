#ifndef MEMORIES_DECOMP_GAME_OVER_H
#define MEMORIES_DECOMP_GAME_OVER_H

#include "../types.h"

struct DisplayObject;

void GameOver_Init(void);
s32 GameOver_Update(void);

/* The Game Over title display object, defined by game_over.c. */
extern struct DisplayObject *gGameOver_pTitleObject;

#endif
