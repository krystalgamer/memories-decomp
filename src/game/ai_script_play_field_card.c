#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#define D_800EAE88_VISIBLE
#include "../unmatched.h"

extern u8 D_800EAE92;
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
