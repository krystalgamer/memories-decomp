#include "../types.h"
#include "duel_card_stat_rules.h"
#include "duel_terrain_boost.h"
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
s32 Duel_CalcGuardianStarMatchup(s32 a0, s32 a1) {
    s32 v1;

    a0 -= DUEL_GUARDIAN_STAR_SECOND_CYCLE_START;
    if (a0 >= 0) {
        a1 -= DUEL_GUARDIAN_STAR_SECOND_CYCLE_START;
        if (a1 < 0) {
            return 0;
        }
        v1 = DUEL_GUARDIAN_STAR_SECOND_CYCLE_COUNT;
    } else {
        v1 = DUEL_GUARDIAN_STAR_FIRST_CYCLE_COUNT;
        a1 -= DUEL_GUARDIAN_STAR_FIRST_CYCLE_START;
        a0 += v1;
        if (a1 >= v1) {
            return 0;
        }
    }

    a0 += 1;
    if (a0 >= v1) {
        a0 = 0;
    }
    if (a0 == a1) {
        return DUEL_GUARDIAN_STAR_BONUS;
    }
    a0 -= 2;

    if (a0 < 0) {
        a0 += v1;
    }
    if (a0 == a1) {
        return -DUEL_GUARDIAN_STAR_BONUS;
    }
    return 0;
}

/* MATCH (2026-09-05). Was an ASSEMBLY TRANSCRIPTION (Unchiga's port of
 * 2026-08-30, an inline asm block) counted as debt in docs/ASM_DEBT.md;
 * this is the C. Flags: default compiler, as -G0. History in docs/PARKED.txt.
 *
 * Reads a card's 32-bit record from gDuel_adwCardStats: the low 9 bits (or bits
 * 9..17 when arg1 is set) times ten, plus Duel_GetTerrainBoost's bonus for the
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

s32 Duel_GetBaseCardStat(s32 arg0, s32 arg1) {
    s32 v;
    s32 s;
    s32 k;
    s32 *t;

    if (arg1 != 0) {
        v = gDuel_adwCardStats[arg0 - 1];
        v = v >> CARD_STAT_DEFENSE_SHIFT;
    } else {
        v = gDuel_adwCardStats[arg0 - 1];
    }

    v &= CARD_STAT_VALUE_MASK;
    do {
        s = v * CARD_STAT_SCALE;
        if (!s) {
            v = !s;
        }
    } while (0);
    t = gDuel_adwCardStats;
    k = arg0 - 1;
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
