#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"
#include "ai_script_commands.h"

void AiScript_Store(void)
{
    int value = AiScript_ReadShort();
    int index = AiScript_ReadByte();
    volatile int *values = gAiScript_aMemory;

    values[index] = value;
}
