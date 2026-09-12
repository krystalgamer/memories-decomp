#include "../types.h"

#define GAI_BEST_TARGET_SCALAR
#include "ai.h"

#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_LoadBestTarget(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;

    values[index] = gAi_bBestTarget;
}
