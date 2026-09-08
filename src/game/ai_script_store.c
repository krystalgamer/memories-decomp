#include "../types.h"
#include "ai_script_read_byte.h"

extern int AiScript_ReadShort(void);
extern int gAiScript_aMemory[];

void AiScript_Store(void)
{
    int value = AiScript_ReadShort();
    int index = AiScript_ReadByte();
    volatile int *values = gAiScript_aMemory;

    values[index] = value;
}
