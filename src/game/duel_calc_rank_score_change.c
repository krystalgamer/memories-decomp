#include "../types.h"

#define DUEL_RANK_SCORE_THRESHOLD_COUNT 5

typedef struct {
    s16 threshold;
    s16 value;
} Threshold;

extern Threshold gDuel_awRankScoreChange[][DUEL_RANK_SCORE_THRESHOLD_COUNT];

s32 Duel_CalcRankScoreChange(s32 arg0, s32 arg1)
{
    Threshold *p = &gDuel_awRankScoreChange[arg0][0];

    while (1) {
        if (arg1 < p->threshold) {
            return p->value;
        }
        p++;
    }
}
