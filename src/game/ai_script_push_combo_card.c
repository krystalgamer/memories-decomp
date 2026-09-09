#include "../types.h"
#include "ai.h"
#include "ai_script_commands.h"

extern u8 D_800EAE88[];

void AiScript_PushComboCard(void)
{
    s32 count = 0;
    s32 i;
    u8 *output;

    i = AI_SCRIPT_COMBO_CARD_COUNT - 1;
    output = D_800EAE88 + i;
    do {
        *output = 0;
        i--;
        output--;
    } while (i >= 0);

    for (i = 0; i <= gAiScript_State.fusion_best_depth; i++) {
        s32 value = gAiScript_State.combo_cards[i];

        if (value >= 11) {
            D_800EAE88[count] = value;
            count++;
        }
    }
}
