#ifndef MEMORIES_DECOMP_DUEL_FALLBACK_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_FALLBACK_SEMANTIC_VIEWS_H

#include "../../types.h"

extern void DuelEffect_DrawErrorCross(s32 mode);
/* Error texts DuelEffect_DrawErrorCross prints in the NTSC banks; the PAL
 * banks have none. */
extern char gDuelEffect_ErrorMessage[];
extern char gDuelEffect_InvalidVariantMessage[];

#endif
