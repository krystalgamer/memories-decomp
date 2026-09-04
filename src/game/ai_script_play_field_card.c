#include "../types.h"
#include "ai.h"

extern AiFieldCardState D_800EAE88;
extern u8 D_800EAE92;
extern s32 gAiScript_aMemory[AI_SCRIPT_MEMORY_COUNT];
extern s32 AiScript_ReadByte(void);

void AiScript_PlayFieldCard(void)
{
    s32 first = gAiScript_aMemory[AiScript_ReadByte()];
    s32 second = gAiScript_aMemory[AiScript_ReadByte()];
    s32 third = gAiScript_aMemory[AiScript_ReadByte()];
    s32 category = gAiScript_aMemory[AiScript_ReadByte()];

    D_800EAE88.field_09 = (s8)first;
    D_800EAE88.field_0B = (s8)third;

    if (category == 0) {
        if (first >= 6 && second == 1) {
            D_800EAE88.field_0A = (s8)first;
            return;
        }
        D_800EAE92 = 0;
        return;
    }

    if (category < 0xFF) {
        D_800EAE88.field_0A = (s8)category;
    } else {
        D_800EAE88.field_0A = 0x3A;
    }
}
