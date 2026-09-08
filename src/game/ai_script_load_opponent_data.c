#include "../types.h"
#include "ai.h"
#include "ai_opponent_data.h"
#include "ai_script_read_byte.h"
void AiScript_LoadOpponentData(void)
{
    s32 index;
    s32 field;
    s32 dst;
    s32 *mem = gAiScript_aMemory;

    index = mem[AiScript_ReadByte()];
    field = mem[AiScript_ReadByte()];
    dst = AiScript_ReadByte();
    if (field == 0) {
        mem[dst] = gDuel_aOpponentData[index].values[1] * 100;
    } else {
        mem[dst] = gDuel_aOpponentData[index].values[field + 1];
    }
}
