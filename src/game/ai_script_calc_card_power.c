#include "../types.h"

extern s32 gAiScript_aMemory[];
extern s32 gDuel_adwCardStats[];
extern s32 AiScript_ReadByte(void);
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

    if (((gDuel_adwCardStats[card - 1] >> 26) & 0x1F) < 0x14) {
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
            case 0x157:
                power = 0x32;
                break;
            case 0x158:
                power = 0x64;
                break;
            case 0x159:
                power = 0xC8;
                break;
            case 0x15A:
                power = 0x1F4;
                break;
            case 0x15B:
                power = 0x3E8;
                break;
            }
        }
    }

    gAiScript_aMemory[dst] = power;
}
