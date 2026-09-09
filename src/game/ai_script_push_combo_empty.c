#include "../types.h"
#include "ai_constants.h"
#include "ai_script_commands.h"

extern u8 gAiScript_State[];
extern u8 D_800EAE88[];

void AiScript_PushComboEmpty(void)
{
    s32 count;
    s32 index;

    for (count = 0, index = 0;
         index <= gAiScript_State[AI_SCRIPT_FUSION_BEST_DEPTH_BYTE_OFFSET]; index++) {
        s32 value = gAiScript_State[index + AI_SCRIPT_COMBO_BYTE_OFFSET];

        if (value >= 11) {
            D_800EAE88[count] = value;
            count++;
        }
    }
}
