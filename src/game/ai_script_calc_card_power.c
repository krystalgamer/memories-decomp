#include "../types.h"
#include "ai_script_read_byte.h"
#include "card_constants.h"

extern s32 gAiScript_aMemory[];
extern s32 gDuel_adwCardStats[];
extern s32 Duel_GetBaseCardStat(s32 card, s32 stat);

void AiScript_CalcCardPower(void)
{
    s32 *mem = gAiScript_aMemory;
    s32 card;
    s32 mode;
    s32 dst;
    s32 power;

    card = mem[AiScript_ReadByte()];
    mode = mem[AiScript_ReadByte()];
    dst = AiScript_ReadByte();

    if (((gDuel_adwCardStats[card - 1] >> CARD_STAT_TYPE_SHIFT) &
         CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
        switch (mode) {
        case 0:
            power = Duel_GetBaseCardStat(card, 0);
            break;
        case 1:
            power = Duel_GetBaseCardStat(card, 1);
            break;
        case 2:
            if (Duel_GetBaseCardStat(card, 0) > Duel_GetBaseCardStat(card, 1)) {
                power = Duel_GetBaseCardStat(card, 0);
            } else {
                power = Duel_GetBaseCardStat(card, 1);
            }
            break;
        }
    } else {
        power = 0;
        if (mode == 0) {
            switch (card) {
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID:
                power = DUEL_SPARKS_DAMAGE;
                break;
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID + 1:
                power = DUEL_HINOTAMA_DAMAGE;
                break;
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID + 2:
                power = DUEL_FINAL_FLAME_DAMAGE;
                break;
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID + 3:
                power = DUEL_OOKAZI_DAMAGE;
                break;
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID + 4:
                power = DUEL_TREMENDOUS_FIRE_DAMAGE;
                break;
            }
        }
    }

    gAiScript_aMemory[dst] = power;
}
