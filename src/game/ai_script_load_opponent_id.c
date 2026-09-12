#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#define DUEL_OPPONENT_ID_IN_DATA
#include "duel_side_state.h"

void AiScript_LoadOpponentID(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    s32 value = gDuel_bOpponentID;

    values[index] = value;
}
