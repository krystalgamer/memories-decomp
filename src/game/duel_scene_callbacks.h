#ifndef YUGIOH_GAME_DUEL_SCENE_CALLBACKS_H
#define YUGIOH_GAME_DUEL_SCENE_CALLBACKS_H

#include "../types.h"

/* The duel scene's phase callback table at 0x80090998.
 *
 * Deliberately left unsized. func_80024200 reaches it through
 * `callbacks[D_8009B23A & 0xF]`, and the mask permits an index the fifteen
 * defined entries do not cover, so a bound here would assert something the
 * call site does not. */
extern void (*D_80090998[])(void);

#endif
