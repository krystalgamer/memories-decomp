#include "../types.h"
#include "ai.h"
#include "ai_constants.h"
#include "duel_calc_guardian_star_matchup.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_commands.h"
#include "ai_script_read_byte.h"
void AiScript_FindBestAttack(void) {
    AiActiveCard *t;
    AiActiveCard *r;
    AiActiveCard *e;
    s32 want;
    s32 out;
    s32 i;
    s32 j;
    s32 v;
    u16 f;

    want = gAiScript_aMemory[AiScript_ReadByte()];
    out = AiScript_ReadByte();
    i = 1;
    t = gDuel_aActiveCards;
    r = &t[AI_SLOT_OWN_MONSTER_FIRST];
    gAiScript_State.attack_best_stat = 0;

    while (i < DUEL_FIELD_ROW_SIZE + 1) {
        if (r->card_id == 0) {
            continue;
        }
        if ((r->flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
            continue;
        }

        for (j = AI_SLOT_OPPONENT_MONSTER_FIRST,
             e = &t[AI_SLOT_OPPONENT_MONSTER_FIRST];
             j < AI_SLOT_OPPONENT_MONSTER_FIRST + AI_ACTIVE_CARD_ROW_SLOT_COUNT;
             j++, e++) {
            if (e->card_id == 0) {
                continue;
            }
            f = e->flags;
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
            v = r->attack - e->attack;
            v += Duel_CalcGuardianStarMatchup(r->guardian_star, e->guardian_star);
            if (gAiScript_State.attack_best_stat < v) {
                gAiScript_State.attack_best_stat = v;
                gAiScript_State.attack_best_slot = i;
                gAiScript_State.attack_best_target = j;
            }
        }
    }

    if (gAi_wBestDifference[0] != 0) {
        gAiScript_aMemory[out] = 0;
    } else {
        gAiScript_aMemory[out] = 1;
    }
}
