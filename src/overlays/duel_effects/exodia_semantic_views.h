#ifndef MEMORIES_DECOMP_DUEL_EXODIA_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_EXODIA_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_RunExodiaBurst(void *work, s32 phase);
extern VECTOR gDuelEffect_ExodiaInitialScale;
/* The zero-terminated card-object list the field collectors fill. */
extern struct DisplayObject *gDuelEffect_CardObjectScratchWords[21];

#endif
