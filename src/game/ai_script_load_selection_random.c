#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_LoadSelectionRandom(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;

    values[index] = D_800EAE90;
}
