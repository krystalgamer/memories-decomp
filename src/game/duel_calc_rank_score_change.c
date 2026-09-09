#include "../types.h"
#include "duel_rank.h"

s32 Duel_CalcRankScoreChange(s32 arg0, s32 arg1)
{
    DuelRankScoreChangeEntry *p = &gDuel_awRankScoreChange[arg0][0];

    while (1) {
        if (arg1 < p->threshold) {
            return p->score_change;
        }
        p++;
    }
}
