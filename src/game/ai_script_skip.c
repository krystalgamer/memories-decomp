#include "../types.h"
#include "ai_script_read_byte.h"

void AiScript_SkipHand(void)
{
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
}

void AiScript_SkipField(void)
{
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
}
