#include "../types.h"
#include "ai_script_commands.h"
#include "../psyq/libetc.h"
#include "../psyq/memory.h"
#include "ai.h"
#include "ai_script_end.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"

extern AiSelection D_800EAE88;

s32 AiScript_ReadByte(void)
{
    // Preserve the original stream-base register allocation.
    register AiScriptState *stream = &gAiScript_State;

    return *stream->script_cursor++;
}

/* Reads a little-endian 16-bit value from the stream and advances the
   cursor by 2. */
s32 AiScript_ReadShort(void) {
    u8 *p = gAiScript_State.script_cursor;
    gAiScript_State.script_cursor += 2;
    return p[0] | (p[1] << 8);
}

void AiScript_Init(void *arg0)
{
    u8 *state = (u8 *)&gAiScript_State;

    bzero(state, sizeof(gAiScript_State));
    bzero((u8 *)&D_800EAE88, sizeof(D_800EAE88));
    bzero((u8 *)gAiScript_aMemory, sizeof(gAiScript_aMemory));
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
        if (VSync(1) >= 0xF0)
            break;
    }

    return 0;
}
