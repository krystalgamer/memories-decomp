#include "../../src/types.h"
#include "../../src/game/ai.h"
#include "../../src/game/ai_script_read_byte.h"
#include "../../src/game/ai_script_commands.h"

#define GDUEL_TERRAIN_SCALAR
#include "../../src/game/duel_terrain_boost.h"

void AiScript_LoadTerrain(void)
{
    s32 index = AiScript_ReadByte();
    volatile s32 *values = gAiScript_aMemory;

    values[index] = gDuel_bTerrain;
}
