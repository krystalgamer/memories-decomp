#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
extern u8 D_800EAE8F[];
void func_8007368C(void)
{
    D_800EAE8F[0] = *(u8 *)&gAiScript_aMemory[AiScript_ReadByte()];
}
