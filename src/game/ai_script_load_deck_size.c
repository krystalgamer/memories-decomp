#include "../types.h"
#include "ai.h"
#include "card_constants.h"

extern s32 AiScript_ReadByte(void);
void AiScript_LoadDeckSize(void) {
    s32 k = AiScript_ReadByte();
    s32 count = 0;
    s32 i;
    for (i = 0xB; i < 0x38; i++)
        if (gDuel_aActiveCards[i].card_id != 0) count++;
    gAiScript_aMemory[k] = count - HAND_SIZE;
}
