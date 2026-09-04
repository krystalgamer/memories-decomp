#include "../types.h"
#include "ai.h"

extern AiFieldCardState D_800EAE88;
extern AiScriptState gAiScript_State;

extern s32 AiScript_ReadByte(void);
extern s32 func_80074170(s32);
extern void func_8008E360(void *, s32);
extern void AiScript_EndHand(void);
extern void AiScript_EndField(void);
extern void AiScript_PlayFieldCard(void);

void AiScript_Init(void *arg0)
{
    u8 *state = (u8 *)&gAiScript_State;

    func_8008E360(state, sizeof(gAiScript_State));
    func_8008E360(&D_800EAE88, sizeof(D_800EAE88));
    func_8008E360(gAiScript_aMemory, sizeof(gAiScript_aMemory));
    if (arg0 == 0)
        state[0] = 1;
    *(void **)(state + 4) = arg0;
    *(void **)(state + 8) = arg0;
}

s32 AiScript_Run(void)
{
    for (;;) {
        s32 idx;
        AiScriptHandler handler;

        gAiScript_State.previous_cursor = gAiScript_State.script_cursor;
        idx = AiScript_ReadByte();
        gAiScript_apfnCommand[idx]();
        handler = gAiScript_apfnCommand[idx];

        if (handler == AiScript_EndHand)
            return 1;
        if (handler == AiScript_EndField)
            return 3;
        if (handler == AiScript_PlayFieldCard)
            return 2;
        if (func_80074170(1) >= 0xF0)
            break;
    }

    return 0;
}
