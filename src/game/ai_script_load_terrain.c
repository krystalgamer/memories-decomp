#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

extern u8 gDuel_bTerrain;

void AiScript_LoadTerrain(void)
{
    s32 index;
    s32 *values;
    u32 value;

    index = AiScript_ReadByte();
    values = gAiScript_aMemory;
    value = gDuel_bTerrain;
    *(volatile s32 *)(values + index) = value;
}
