#include "../types.h"
#include "ai.h"
#include "ai_constants.h"
#include "card_constants.h"
#include "duel_card_stat_rules.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_commands.h"
#include "ai_script_read_byte.h"

/* Each slot is addressed as &gDuel_aActiveCards[i] from the loop index, so the
 * strength reducer folds the card_id, attack, flags and guardian_star reads
 * (+0, +2, +6, +9) into one cursor with plain displacements, as in retail. */

void AiScript_FindKiller(void) {
    AiActiveCard *c;
    AiActiveCard *ref;
    s32 a;
    s32 m;
    s32 w;
    s32 i;
    s32 k;
    s32 best;
    s32 d;
    s32 t;

    a = gAiScript_aMemory[AiScript_ReadByte()];
    best = CARD_STAT_MAX;
    m = gAiScript_aMemory[AiScript_ReadByte()];
    k = 0;
    w = AiScript_ReadByte();
    i = 1;
    ref = &gDuel_aActiveCards[a];

    for (; i < DUEL_FIELD_ROW_SIZE + 1; i++) {
        c = &gDuel_aActiveCards[i];
        if (c->card_id == 0) {
            continue;
        }
        if ((c->flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
            continue;
        }
        if (m == 0) {
            d = c->attack - ref->attack;
        } else {
            d = c->attack - ref->defense;
        }
        d = d + Duel_CalcGuardianStarMatchup(c->guardian_star, ref->guardian_star);
        if (d > 0) {
            t = c->attack;
            if (t < best) {
                best = t;
                k = i;
            }
        }
    }

    gAiScript_aMemory[w] = k;
}
