#include "../types.h"

extern s32 gAiScript_aMemory[];
extern u8 gAiScript_State[];

extern s32 AiScript_ReadByte(void);

void AiScript_ClearCards(void)
{
    s32 i = 31;
    u16 *entries = (u16 *)gAiScript_State;

    entries += 31;
    do {
        entries[31] = 0;
        i--;
        entries--;
    } while (i >= 0);
}

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

void AiScript_ClearTypes(void)
{
    s32 i = 24;
    u8 *entries = gAiScript_State;

    entries += i;
    do {
        entries[126] = 0;
        i--;
        entries--;
    } while (i >= 0);
}
