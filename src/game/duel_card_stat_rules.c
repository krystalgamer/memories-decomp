#include "../types.h"
#include "duel_card_stat_rules.h"
#include "duel_scene.h"
#include "card_constants.h"
#include "duel_card.h"

#define DUEL_GUARDIAN_STAR_BONUS 500
#define DUEL_GUARDIAN_STAR_FIRST_CYCLE_START 1
#define DUEL_GUARDIAN_STAR_FIRST_CYCLE_COUNT 6
#define DUEL_GUARDIAN_STAR_SECOND_CYCLE_START \
    (DUEL_GUARDIAN_STAR_FIRST_CYCLE_START + \
     DUEL_GUARDIAN_STAR_FIRST_CYCLE_COUNT)
#define DUEL_GUARDIAN_STAR_SECOND_CYCLE_COUNT 4

/* IDs 1-6 and 7-10 form separate cycles; adjacent matchups return +/-500. */
s32 Duel_CalcGuardianStarMatchup(s32 star, s32 opposing_star) {
    s32 cycle_length;

    star -= DUEL_GUARDIAN_STAR_SECOND_CYCLE_START;
    if (star >= 0) {
        opposing_star -= DUEL_GUARDIAN_STAR_SECOND_CYCLE_START;
        if (opposing_star < 0) {
            return 0;
        }
        cycle_length = DUEL_GUARDIAN_STAR_SECOND_CYCLE_COUNT;
    } else {
        cycle_length = DUEL_GUARDIAN_STAR_FIRST_CYCLE_COUNT;
        opposing_star -= DUEL_GUARDIAN_STAR_FIRST_CYCLE_START;
        star += cycle_length;
        if (opposing_star >= cycle_length) {
            return 0;
        }
    }

    star += 1;
    if (star >= cycle_length) {
        star = 0;
    }
    if (star == opposing_star) {
        return DUEL_GUARDIAN_STAR_BONUS;
    }
    star -= 2;

    if (star < 0) {
        star += cycle_length;
    }
    if (star == opposing_star) {
        return -DUEL_GUARDIAN_STAR_BONUS;
    }
    return 0;
}

/* MATCH (2026-09-05). Was an ASSEMBLY TRANSCRIPTION (Unchiga's port of
 * 2026-08-30, an inline asm block) counted as debt in docs/ASM_DEBT.md;
 * this is the C. Flags: default compiler, as -G0. History in docs/PARKED.txt.
 *
 * Reads a card's 32-bit record from gDuel_adwCardStats: the low 9 bits (or bits
 * 9..17 when is_defense is set) times ten, plus Duel_GetTerrainBoost's bonus for the
 * 5-bit field at bits 26..30, clamped to 0..9999 (negative results return
 * 0, which is why the clamp is a single `return s`).
 *
 * Two shapes carry the match and both are the register allocator's, not
 * the code's. `do { s = v * 10; } while (0);` pins the *10 ahead of the
 * third record read so the load-delay slot after `lw $a0` stays empty as
 * retail has it. And the record base for that read goes through a local
 * `t` -- which alone gives retail's instruction ORDER with the two
 * registers exchanged (6) -- plus a dead conditional assignment to `v`
 * inside the pin, `if (!s) { v = !s; }`, which gcc deletes entirely but
 * which numbers a pseudo that tips the allocation the other way: 3 -> 0.
 * Either half alone is 3 or 6; the permuter found the pair at score 0
 * after seventeen hand spellings of the halves had been measured. Read it
 * as the residue of a macro in the original.
 */

s32 Duel_GetBaseCardStat(s32 card_id, s32 is_defense) {
    s32 v;
    s32 s;
    s32 k;
    s32 *t;

    if (is_defense != 0) {
        v = gDuel_adwCardStats[card_id - 1];
        v = v >> CARD_STAT_DEFENSE_SHIFT;
    } else {
        v = gDuel_adwCardStats[card_id - 1];
    }

    v &= CARD_STAT_VALUE_MASK;
    do {
        s = v * CARD_STAT_SCALE;
        if (!s) {
            v = !s;
        }
    } while (0);
    t = gDuel_adwCardStats;
    k = card_id - 1;
    s += Duel_GetTerrainBoost((t[k] >> CARD_STAT_TYPE_SHIFT) &
                              CARD_STAT_TYPE_MASK);

    if (s < 0) {
        return 0;
    }

    if (s >= CARD_STAT_MAX + 1) {
        s = CARD_STAT_MAX;
    }

    return s;
}
