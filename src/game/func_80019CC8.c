#include "../types.h"
#include "campaign_flags.h"

extern u8 D_8009B1D5;
extern s8 D_8009B360[9];
extern s8 gDuel_bOpponentID[9];

void func_80019CC8(void *object)
{
    if (D_8009B1D5 == 0 && D_8009B360[0] < 0 && gDuel_bOpponentID[0] >= 0) {
        Library_UpdateCardUsedFlag((s32)((char *)object + CAMPAIGN_FLAG_LIBRARY_CARD_BASE));
    }
}
