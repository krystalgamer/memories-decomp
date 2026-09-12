#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "duel_terrain_state.h"

void AiScript_LoadTerrain(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value = gDuel_bTerrain;

    values[index] = value;
}
