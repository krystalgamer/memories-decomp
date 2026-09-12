#ifndef MEMORIES_DECOMP_DEBUG_EFFECT_SCREEN_H
#define MEMORIES_DECOMP_DEBUG_EFFECT_SCREEN_H

#include "../types.h"

/* The developer effect-preview screen. func_80022618 is the gDuel_apfnSceneStateHandler duel
 * phase entry for it: it runs the preview builder and then prints the
 * "EFFECT = %2d %2d" line with one of two divider strings, chosen by
 * D_8009AF2A.
 *
 * The other two functions this unit defines are reached only from within it. */
void func_80022618(void);

#endif
