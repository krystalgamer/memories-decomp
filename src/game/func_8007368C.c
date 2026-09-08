#include "../types.h"
#include "ai_script_read_byte.h"

extern s32 gAiScript_aMemory[];
extern u8 D_800EAE8F[];
void func_8007368C(void)
{
    D_800EAE8F[0] = *(u8 *)&gAiScript_aMemory[AiScript_ReadByte()];
}
