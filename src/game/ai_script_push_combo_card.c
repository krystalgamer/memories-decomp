#include "../types.h"

extern u8 gAiScript_State[];
extern u8 D_800EAE88[];

void AiScript_PushComboCard(void)
{
    s32 count = 0;
    s32 i;
    u8 *output;

    i = 5;
    output = D_800EAE88 + i;
    do {
        *output = 0;
        i--;
        output--;
    } while (i >= 0);

    for (i = 0; i <= gAiScript_State[0xA3]; i++) {
        s32 value = gAiScript_State[i + 0x38];

        if (value >= 11) {
            D_800EAE88[count] = value;
            count++;
        }
    }
}
