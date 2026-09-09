#include "../types.h"
#include "ai.h"
#include "ai_script_read_short.h"

/* Reads a little-endian 16-bit value from the stream and advances the
   cursor by 2. */
s32 AiScript_ReadShort(void) {
    u8 *p = gAiScript_State.script_cursor;
    gAiScript_State.script_cursor += 2;
    return p[0] | (p[1] << 8);
}
