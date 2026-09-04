#include "../types.h"

extern s32 gAiScript_aMemory[];
extern u8 gAiScript_State[];
extern s32 AiScript_ReadByte(void);

void AiScript_AddType(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = AiScript_ReadByte();
    s32 type = memory[index] + 1;
    s32 i;

    for (i = 0; i < 25; i++) {
        u8 *entry = gAiScript_State + i;

        if (entry[0x7E] == type) {
            break;
        }
        if (entry[0x7E] == 0) {
            entry[0x7E] = type;
            break;
        }
    }
}
