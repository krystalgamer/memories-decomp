#include "../types.h"

extern u8 gAiScript_State[];
extern u8 D_800EAE88[];

void AiScript_PushComboEmpty(void)
{
    s32 count;
    s32 index;

    for (count = 0, index = 0; index <= gAiScript_State[0xA3]; index++) {
        s32 value = gAiScript_State[index + 0x38];

        if (value >= 11) {
            D_800EAE88[count] = value;
            count++;
        }
    }
}
