#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "duel_terrain_boost.h"

void AiScript_LoadTerrain(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value = gDuel_bTerrain;

    values[index] = value;
}
