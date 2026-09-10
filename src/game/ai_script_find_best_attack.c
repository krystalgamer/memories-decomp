#include "../types.h"
#include "ai.h"
#include "ai_constants.h"
#include "duel_calc_guardian_star_matchup.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_commands.h"
#include "ai_script_read_byte.h"
/* The brackets are a lever, not a size. The object is one u16 -- two bytes,
   with gAi_bBestAttacker at +2 -- and nothing here indexes above [0]. This
   unit compiles at -G8, where a two-byte scalar would be placed in small data
   and addressed %gp_rel; declaring it an array is what keeps it on %hi/%lo.
   AiScript_LoadBestDifference (now src/candidates/func_8007164C.c) compiles
   at -G0 and so spells the same symbol a plain scalar. Both objects relocate
   HI16/LO16, which makes the two look like drift, but that agreement is two
   correct levers rather than one declaration that could serve both. See
   notes/build.md. */
extern u16 gAi_wBestDifference[];

void AiScript_FindBestAttack(void) {
    u8 *t;
    u8 *r;
    u8 *e;
    s32 want;
    s32 out;
    s32 i;
    s32 j;
    s32 v;
    u16 f;

    want = gAiScript_aMemory[AiScript_ReadByte()];
    out = AiScript_ReadByte();
    i = 1;
    t = (u8 *)gDuel_aActiveCards;
    r = t + AI_ACTIVE_CARD_RECORD_SIZE;
    gAiScript_State.attack_best_stat = 0;

    while (i < DUEL_FIELD_ROW_SIZE + 1) {
        if (*(s16 *)r == 0) {
            continue;
        }
        if ((*(u16 *)(r + 6) & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
            continue;
        }

        for (j = AI_SLOT_OPPONENT_MONSTER_FIRST,
             e = t + (AI_ACTIVE_CARD_RECORD_SIZE + AI_ACTIVE_CARD_SIDE_BYTE_STRIDE);
             j < AI_SLOT_OPPONENT_MONSTER_FIRST + AI_ACTIVE_CARD_ROW_SLOT_COUNT;
             j++, e += AI_ACTIVE_CARD_RECORD_SIZE) {
            if (*(s16 *)e == 0) {
                continue;
            }
            f = *(u16 *)(e + 6);
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
            v = *(s16 *)(r + 2) - *(s16 *)(e + 2);
            v += Duel_CalcGuardianStarMatchup(*(s8 *)(r + 9), *(s8 *)(e + 9));
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
