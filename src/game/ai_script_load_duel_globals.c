#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "duel_terrain_boost.h"
#include "ai_opponent_data.h"

void AiScript_LoadOpponentID(void)
{
    s32 index;
    volatile s32 *values;
    s32 value;

    index = AiScript_ReadByte();
    values = gAiScript_aMemory;
    value = gDuel_bOpponentID;
    values[index] = value;
}

void AiScript_LoadTerrain(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value = gDuel_bTerrain;

    values[index] = value;
}
