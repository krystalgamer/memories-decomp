#include "../types.h"
#include "ai.h"
#include "card_constants.h"
#include "ai_script_read_byte.h"

void AiScript_LoadDeckSize(void) {
    s32 k = AiScript_ReadByte();
    s32 count = 0;
    s32 i;
    for (i = AI_SLOT_OWN_HAND_FIRST; i < AI_SLOT_OPPONENT_MONSTER_FIRST; i++)
        if (gDuel_aActiveCards[i].card_id != 0) count++;
    gAiScript_aMemory[k] = count - HAND_SIZE;
}
