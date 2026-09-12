#include "../types.h"
#include "ai.h"
#define AI_OPPONENT_ID_IN_DATA
#include "ai_opponent_data.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_LoadOpponentID(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    s32 value = gDuel_bOpponentID;

    values[index] = value;
}
