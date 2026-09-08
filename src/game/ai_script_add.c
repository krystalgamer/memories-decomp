#include "../types.h"
#include "ai_script_read_byte.h"

extern int gAiScript_aMemory[];

void AiScript_Add(void)
{
    int left = AiScript_ReadByte();
    int right = AiScript_ReadByte();
    int output = AiScript_ReadByte();
    int *values = gAiScript_aMemory;

    values[output] = values[left] + values[right];
}
