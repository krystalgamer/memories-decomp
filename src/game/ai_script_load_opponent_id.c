#define GDUEL_OPPONENT_ID_SCALAR
#include "../types.h"
#include "ai.h"
#include "ai_opponent_data.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_LoadOpponentID(void)
{
    s32 index = AiScript_ReadByte();
    volatile s32 *values = gAiScript_aMemory;

    values[index] = gDuel_bOpponentID;
}
