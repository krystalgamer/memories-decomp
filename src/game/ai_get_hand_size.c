#include "../types.h"

struct OppData {
    s8 b[9];
};

extern struct OppData gDuel_aOpponentData[];
extern s8 gDuel_bOpponentID;

s8 Ai_GetHandSize(void)
{
    return gDuel_aOpponentData[gDuel_bOpponentID].b[0];
}
