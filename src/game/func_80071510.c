#include "../types.h"
#define AI_RANDOM_AS_ARRAY
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void func_80071510(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value;

    index <<= 2;
    value = D_800EAE90[0];
    *(s32 *)((u8 *)values + index) = value;
}
