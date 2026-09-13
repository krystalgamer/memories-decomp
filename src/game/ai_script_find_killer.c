#include "../types.h"
#include "ai.h"
#include "ai_constants.h"
#include "card_constants.h"
#include "duel_calc_guardian_star_matchup.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_commands.h"
#include "ai_script_read_byte.h"

/* -fno-strength-reduce, fourth user. The record walk reads card_id, attack,
 * flags and guardian_star (+0, +2, +6, +9) and gcc builds a SECOND giv
 * biased at +2 because most of the reads are there; that costs a
 * callee-saved register and four frame instructions (78 differences, +4
 * length). Retail has one cursor with plain displacements. No source
 * spelling removes the anchor -- an index form, the AiActiveCard cursor
 * used here, dropping the named compare value and inlining the base were
 * all tried -- because the bias is the reducer's, not the source's. */

void AiScript_FindKiller(void) {
    AiActiveCard *base;
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
    base = gDuel_aActiveCards;
    c = &base[AI_SLOT_OWN_MONSTER_FIRST];
    ref = base + a;

    for (; i < DUEL_FIELD_ROW_SIZE + 1; i++, c++) {
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
