#ifndef MEMORIES_DECOMP_DUEL_RANK_H
#define MEMORIES_DECOMP_DUEL_RANK_H

#include "../types.h"

#define DUEL_RANK_SCORE_INITIAL 50
#define DUEL_RANK_SCORE_THRESHOLD_COUNT 5

#define DUEL_RANK_ADJUST_EXODIA_WIN 40
#define DUEL_RANK_ADJUST_DECK_OUT_WIN (-40)

#define DUEL_RESULT_TEXT_SELECTOR_DEFAULT 0x40
#define DUEL_RESULT_TEXT_SELECTOR_DECK_OUT 0x41
#define DUEL_RESULT_TEXT_SELECTOR_EXODIA 0x42

#define DUEL_RANK_RULE_TURNS 0
#define DUEL_RANK_RULE_EFFECTIVE_ATTACKS 1
#define DUEL_RANK_RULE_DEFENSIVE_WINS 2
#define DUEL_RANK_RULE_FACE_DOWN_PLAYS 3
#define DUEL_RANK_RULE_PURE_MAGIC 4
#define DUEL_RANK_RULE_TRAPS_TRIGGERED 5
#define DUEL_RANK_RULE_CARDS_USED 6
#define DUEL_RANK_RULE_REMAINING_LP 7
#define DUEL_RANK_RULE_INITIATE_FUSION 8
#define DUEL_RANK_RULE_EQUIP_MAGIC 9

/* One threshold/change pair in a duel-rank rule row. The first threshold
 * strictly above the measured value supplies the score change. */
typedef struct {
    s16 threshold;
    s16 score_change;
} DuelRankScoreChangeEntry;

extern DuelRankScoreChangeEntry
    gDuel_awRankScoreChange[][DUEL_RANK_SCORE_THRESHOLD_COUNT];

s32 Duel_CalcRankScoreChange(s32 rule, s32 value);

#endif
