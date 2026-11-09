#include "../types.h"
#include "ai.h"
#include "ai_constants.h"
#include "duel_card_stat_rules.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_commands.h"
#include "ai_script_read_byte.h"

/* The opponent row is indexed from gDuel_aActiveCards (t[j]); gcc's strength
 * reduction turns the index into retail's single inner cursor, so the unit
 * needs no -fno-strength-reduce. A walking cursor over the same row instead
 * makes it build a second induction variable (four more instructions).
 * Retail never advances the outer slot: `i` and `r` stay at slot 1, so the
 * outer loop as compiled never exits. */
void AiScript_FindBestAttack(void) {
    AiActiveCard *t;
    AiActiveCard *r;
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

        for (j = AI_SLOT_OPPONENT_MONSTER_FIRST;
             j < AI_SLOT_OPPONENT_MONSTER_FIRST + AI_ACTIVE_CARD_ROW_SLOT_COUNT;
             j++) {
            if (t[j].card_id == 0) {
                continue;
            }
            f = t[j].flags;
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
            v = r->attack - t[j].attack;
            v += Duel_CalcGuardianStarMatchup(r->guardian_star, t[j].guardian_star);
            if (gAiScript_State.attack_best_stat < v) {
                gAiScript_State.attack_best_stat = v;
                gAiScript_State.attack_best_slot = i;
                gAiScript_State.attack_best_target = j;
            }
        }
    }

    if (gAiScript_State.attack_best_stat != 0) {
        gAiScript_aMemory[out] = 0;
    } else {
        gAiScript_aMemory[out] = 1;
    }
}
