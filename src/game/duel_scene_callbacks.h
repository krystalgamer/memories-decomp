#ifndef YUGIOH_GAME_DUEL_SCENE_CALLBACKS_H
#define YUGIOH_GAME_DUEL_SCENE_CALLBACKS_H

#include "../types.h"

/* The duel scene's phase callback table at 0x80090998.
 *
 * Deliberately left unsized. func_80024200 reaches it through
 * `callbacks[D_8009B23A & DUEL_SCENE_PHASE_MASK]`, and the mask permits an index the fifteen
 * defined entries do not cover, so a bound here would assert something the
 * call site does not. */
extern void (*D_80090998[])(void);

/* Card-move presentation, position choice, and deferred stat adjustment. */
void func_8001B170(void);

#endif
