#ifndef MEMORIES_DECOMP_AI_BOARD_SNAPSHOT_H
#define MEMORIES_DECOMP_AI_BOARD_SNAPSHOT_H

#include "ai.h"

/* Interior alias of gDuel_aActiveCards[AI_SLOT_OWN_MONSTER_FIRST], not a
 * separate allocation. Retain the alias as the refresh routine's base. */
extern AiActiveCard gAiSnapshot_aOwnCards[];

void Ai_ExportSideSnapshot(AiActiveCard *out, s32 side);
void Ai_RefreshBoardSnapshot(void);

#endif
