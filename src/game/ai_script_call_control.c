#include "../types.h"
#include "ai.h"

extern int AiScript_ReadShort(void);
extern s32 AiScript_ReadByte(void);
extern void func_8008E870();
extern s32 rand(void);

extern AiScriptState gAiScript_State;
extern char D_800118AC[];
extern char D_800118E4[];
extern char D_8009B084[];
extern char D_800118CC[];

void AiScript_Call(void) {
    s32 val = AiScript_ReadShort();

    if (gAiScript_State.return_depth < 8) {
        gAiScript_State.return_stack[gAiScript_State.return_depth] =
            gAiScript_State.script_cursor;
        gAiScript_State.return_depth =
            gAiScript_State.return_depth + 1;
    } else {
        func_8008E870(D_800118AC);
        func_8008E870(D_8009B084, D_800118CC, 0x17B);
        for (;;)
            ;
    }

    {
        AiScriptState *state = &gAiScript_State;
        val += (s32)state->script_base;
        state->script_cursor = (u8 *)val;
    }
}

void AiScript_Return(void) {
    u8 count = gAiScript_State.return_depth;
    if (count != 0) {
        count = count - 1;
        gAiScript_State.return_depth = count;
        gAiScript_State.script_cursor = gAiScript_State.return_stack[count];
        return;
    }
    func_8008E870(D_800118E4);
    func_8008E870(D_8009B084, D_800118CC, 0x193);
    for (;;)
        ;
}

void AiScript_SetRandom(void) {
    s32 lo = AiScript_ReadShort();
    s32 hi = AiScript_ReadShort();
    s32 idx = AiScript_ReadByte();

    gAiScript_aMemory[idx] = rand() % (hi - lo + 1) + lo;
}
