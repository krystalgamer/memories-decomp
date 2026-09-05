#include "../types.h"

struct ActiveCardEntry {
    s16 card_id;
    u8 pad_02[4];
    u16 flags;
    u8 pad_08[4];
};

extern s32 gAiScript_aMemory[];
extern struct ActiveCardEntry gDuel_aActiveCards[];
extern s32 AiScript_ReadByte(void);

void AiScript_CountCards(void)
{
    register s32 type __asm__("s0");
    register s32 count __asm__("s1");
    s32 *table = gAiScript_aMemory;
    s32 idx1;
    s32 idx2;
    s32 start;
    s32 end;
    s32 i;

    idx1 = AiScript_ReadByte();
    type = table[idx1];
    idx2 = AiScript_ReadByte();
    count = 0;

    Ai_GetCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        struct ActiveCardEntry *entry = &gDuel_aActiveCards[i];
        if (entry->card_id != 0) {
            if (type == 1 || type == 3 || type == 6 || type == 8) {
                if (!(entry->flags & 0x4000)) {
                    count++;
                }
            } else {
                count++;
            }
        }
    }

    gAiScript_aMemory[idx2] = count;
}
