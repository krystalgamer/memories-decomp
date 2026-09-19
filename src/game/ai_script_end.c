#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "ai_script_end.h"
#define D_800EAE88_VISIBLE
#include "../unmatched.h"

/* The three AI script opcodes that end an interpreter run. After each
   dispatch AiScript_Run (ai_script_vm.c) compares the handler it just called
   against these and returns 2 for AiScript_PlayFieldCard, 1 for
   AiScript_EndHand and 3 for AiScript_EndField. Only the field play has a
   body: it stamps the script-selected slot, card and category into the
   play-command record before the run ends. */

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

void AiScript_EndHand(void)
{
}

void AiScript_EndField(void)
{
}
