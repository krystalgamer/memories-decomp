#include "../types.h"
#include "ai.h"

extern u8 gAiScript_State[];
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

    for (i = 0; i <= gAiScript_State[AI_SCRIPT_FUSION_BEST_DEPTH_BYTE_OFFSET]; i++) {
        s32 value = gAiScript_State[i + AI_SCRIPT_COMBO_BYTE_OFFSET];

        if (value >= 11) {
            D_800EAE88[count] = value;
            count++;
        }
    }
}
