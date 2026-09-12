#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "ai_opponent_data.h"

void AiScript_LoadOpponentID(void)
{
    s32 index;
    s32 *values;
    s32 value;

    index = AiScript_ReadByte();
    values = gAiScript_aMemory;
    value = gDuel_bOpponentID;
    *(volatile s32 *)(values + index) = value;
}
