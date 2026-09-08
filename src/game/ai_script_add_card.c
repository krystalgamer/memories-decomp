#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
extern u8 gAiScript_State[];
void AiScript_AddCard(void) {
    s32 *p = gAiScript_aMemory;
    u8 *q;
    s32 v;
    s32 i;
    v = p[AiScript_ReadByte()];
    q = gAiScript_State;
    for (i = 0; i < AI_SCRIPT_CARD_SET_COUNT; i++) {
        u16 c = *(u16 *)(q + i * AI_SCRIPT_CARD_SET_ENTRY_SIZE +
                        AI_SCRIPT_CARD_SET_BYTE_OFFSET);
        if (c == v) return;
        if (c == AI_SCRIPT_CARD_SET_EMPTY) {
            *(u16 *)(q + i * AI_SCRIPT_CARD_SET_ENTRY_SIZE +
                     AI_SCRIPT_CARD_SET_BYTE_OFFSET) = v;
            return;
        }
    }
}
