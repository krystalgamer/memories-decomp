#include "../types.h"

struct OppData {
    s8 b[9];
};

extern s32 gAiScript_aMemory[];
extern struct OppData gDuel_aOpponentData[];
extern s32 AiScript_ReadByte(void);

void AiScript_LoadOpponentData(void)
{
    s32 index;
    s32 field;
    s32 dst;
    s32 *mem = gAiScript_aMemory;

    index = mem[AiScript_ReadByte()];
    field = mem[AiScript_ReadByte()];
    dst = AiScript_ReadByte();
    if (field == 0) {
        mem[dst] = gDuel_aOpponentData[index].b[1] * 100;
    } else {
        mem[dst] = gDuel_aOpponentData[index].b[field + 1];
    }
}
