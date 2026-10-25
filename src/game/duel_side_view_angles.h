#ifndef MEMORIES_DECOMP_DUEL_SIDE_VIEW_ANGLES_H
#define MEMORIES_DECOMP_DUEL_SIDE_VIEW_ANGLES_H

#include "../types.h"

/* Opposing view headings, indexed by duel side. */
#ifdef VERSION_JAPAN
extern u16 gDuel_awSideViewHeadings[2] asm("gJapanese_DuelSideViewHeadings");
#else
extern u16 gDuel_awSideViewHeadings[2];
#endif

#endif
