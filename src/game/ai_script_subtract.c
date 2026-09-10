#include "../types.h"
#include "ai.h"
#include "ai_script_commands.h"
#include "ai_script_read_byte.h"
void AiScript_Subtract(void){int a=AiScript_ReadByte(),b=AiScript_ReadByte(),c=AiScript_ReadByte();register int*values=gAiScript_aMemory;values[c]=values[a]-values[b];}
