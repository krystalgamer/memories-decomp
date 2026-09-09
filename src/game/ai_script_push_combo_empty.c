#include "../types.h"
#include "ai_constants.h"
#include "ai_script_commands.h"
#include "ai.h"

extern AiScriptState gAiScript_State;
extern u8 D_800EAE88[];

void AiScript_PushComboEmpty(void)
{
    s32 count;
    s32 index;

    for (count = 0, index = 0;
         index <= gAiScript_State.fusion_best_depth; index++) {
        s32 value = gAiScript_State.combo_cards[index];

        if (value >= 11) {
            D_800EAE88[count] = value;
            count++;
        }
    }
}
