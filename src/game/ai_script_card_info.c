#include "../types.h"
#include "ai.h"
#include "card_constants.h"

extern s32 gDuel_adwCardStats[];

extern s32 AiScript_ReadByte(void);
extern s32 Duel_GetBaseCardStat(s32, s32);

void AiScript_TestHighStat(void)
{
    register s32 *values = gAiScript_aMemory;
    s32 value = values[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();
    s32 result;

    if (((gDuel_adwCardStats[value - 1] >> 26) & 31) < CARD_TYPE_MAGIC) {
        s32 attack = Duel_GetBaseCardStat(value, 0);
        s32 defense = Duel_GetBaseCardStat(value, 1);
        result = !(defense < attack);
    } else {
        result = 0;
    }
    gAiScript_aMemory[dest] = result;
}

void AiScript_LoadCardType(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = memory[AiScript_ReadByte()] - 1;

    memory[AiScript_ReadByte()] =
        (gDuel_adwCardStats[index] >> 26) & 0x1F;
}

void AiScript_LoadCardID(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = memory[AiScript_ReadByte()];

    memory[AiScript_ReadByte()] = gDuel_aActiveCards[index].card_id;
}
