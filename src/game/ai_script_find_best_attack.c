#include "../types.h"
#include "ai.h"
#include "ai_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_commands.h"
extern AiScriptState gAiScript_State;
extern u16 gAi_wBestDifference[];

s32 AiScript_ReadByte(void);

void AiScript_FindBestAttack(void) {
    u8 *t;
    u8 *r;
    u8 *e;
    u8 *s;
    u8 *a;
    s32 want;
    s32 out;
    s32 i;
    s32 j;
    s32 v;
    u16 f;

    want = gAiScript_aMemory[AiScript_ReadByte()];
    out = AiScript_ReadByte();
    i = 1;
    t = (u8 *)gDuel_aActiveCards;
    r = t + AI_ACTIVE_CARD_RECORD_SIZE;
    a = (u8 *)&gAiScript_State;
    *(s16 *)(a + 0x98) = 0;
    s = a;

    while (i < DUEL_FIELD_ROW_SIZE + 1) {
        if (*(s16 *)r == 0) {
            continue;
        }
        if ((*(u16 *)(r + 6) & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
            continue;
        }

        for (j = 0x38,
             e = t + (AI_ACTIVE_CARD_RECORD_SIZE + AI_ACTIVE_CARD_SIDE_BYTE_STRIDE);
             j < 0x3D; j++, e += AI_ACTIVE_CARD_RECORD_SIZE) {
            if (*(s16 *)e == 0) {
                continue;
            }
            f = *(u16 *)(e + 6);
            if ((f & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
                continue;
            }
            if ((f & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                continue;
            }
            if (want != 0) {
                if ((f & DUEL_CARD_FLAG_FACE_DOWN) != 0) {
                    continue;
                }
            }
            v = *(s16 *)(r + 2) - *(s16 *)(e + 2);
            v += Duel_CalcGuardianStarMatchup(*(s8 *)(r + 9), *(s8 *)(e + 9));
            if (*(u16 *)(s + 0x98) < v) {
                *(s16 *)(s + 0x98) = v;
                *(s8 *)(s + 0x9A) = i;
                *(s8 *)(s + 0x9B) = j;
            }
        }
    }

    if (gAi_wBestDifference[0] != 0) {
        gAiScript_aMemory[out] = 0;
    } else {
        gAiScript_aMemory[out] = 1;
    }
}
