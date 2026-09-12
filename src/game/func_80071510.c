#include "../../src/types.h"
#include "../../src/game/ai.h"
#include "../../src/game/ai_script_read_byte.h"
#include "../../src/game/ai_script_commands.h"

void func_80071510(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;

    values[index] = D_800EAE90;
}
