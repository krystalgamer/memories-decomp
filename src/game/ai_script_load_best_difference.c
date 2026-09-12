#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_LoadBestDifference(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value;

    index *= sizeof(*values);
    value = gAi_wBestDifference[0];
    *(u32 *)((u8 *)values + index) = value;
}
