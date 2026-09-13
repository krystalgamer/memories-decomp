#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_LoadBestTarget(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value;

    index *= sizeof(*values);
    value = gAi_bBestTarget;
    *(u32 *)((u8 *)values + index) = value;
}
