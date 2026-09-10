#ifndef YUGIOH_GAME_DUEL_ACTION_LOCK_H
#define YUGIOH_GAME_DUEL_ACTION_LOCK_H

#include "../types.h"

/* The card-effect flag word. duel_action_lock.c owns the lock half of it:
 * DuelEffect_MarkInitialized sets bit 0x80 the first time it is asked and
 * func_80024E4C clears the whole word. func_80026BA4 starts an effect by
 * writing DUEL_CARD_EFFECT_FLAG_ACTIVE, or ACTIVE | SECOND_HANDLER
 * (card_constants.h), and the effect handlers in duel_card_effects.c and
 * its neighbours set and test the bits below 0x80 as they step. Retail
 * reaches it gp-relative at every site, 23 lhu and 34 sh, so u16 is the
 * width and sign the accesses give it. */
extern u16 D_8009B220;

/* The two values func_80026BA4 stores just before that flag word. D_8009B1D2
 * is its first parameter, stored unmodified once it falls in one of the two
 * DUEL_EFFECT_*_CARD_ID blocks or equals DUEL_DARK_MAGIC_RITUAL_CARD_ID; the
 * handlers compare it against
 * DUEL_DRAGON_CAPTURE_JAR_CARD_ID and DUEL_SPELLBINDING_CIRCLE_CARD_ID,
 * subtract DUEL_DIRECT_DAMAGE_FIRST_CARD_ID from it, and func_8002622C
 * passes it to Duel_CheckRitual as ritualId. D_8009B1A8 is the index
 * func_80026BA4 computes from that value, which duel_magic_effect_dispatch.c
 * uses as indices[D_8009B1A8] into gDuelEffect_abGroupByEffectId. Retail
 * reads D_8009B1D2 with lh at seven sites and D_8009B1A8 with lh at one,
 * all gp-relative; the one lhu of D_8009B1D2 is in func_800262D4, still
 * assembly. The u16 spellings were only in the writer, where a store shows
 * no sign. func_80024E58 reads the low byte of D_8009B1D2 through an
 * explicit (u8 *) cast in its candidate source, which is retail's lbu. */
extern s16 D_8009B1A8;
extern s16 D_8009B1D2;

/* Two halfwords; the field sweeps in duel_card_effects.c and
 * func_800260D0.c all reach [1], which retail addresses
 * as %gp_rel(D_8009B20C + 0x2): 23 sites, 7 lh, 5 lhu and 11 sh, three of
 * them in func_800262D4, still assembly. In the four matched TUs the lhu
 * is where the source writes *(u16 *)&D_8009B20C[1] for the +1 or -1.
 * func_80025D30 runs it down from 0x10; the other three reset it to 0 or
 * -1, advance it and index D_800907D8 by it. [0] is written and read only
 * by func_8001BD88 and func_8001D670, still assembly, 30 sites at +0;
 * whether it belongs to the same object is not established. Four bytes,
 * the extent up to D_8009B210, stays gp-relative at every threshold in
 * play; with [4] shared instead, an assembler -G4 (func_8002596C's until
 * #3859 found that threshold inert) takes the eight bytes out of small data
 * and the tree does not link. */
extern s16 D_8009B20C[2];

/* The halfword immediately past that pair, at D_8009B20C + 4 -- the extent
 * the note above measures against. It stays a separate declaration for the
 * reason recorded there: spelling the pair as [4] to cover these four bytes
 * takes eight bytes out of small data under an assembler -G4, which
 * func_8002596C was built at until #3859, and the tree stopped linking, so
 * this address got its own scalar.
 *
 * It is the trap presentation's mode: func_8001F364 in duel_trap_resolution.c
 * sequences on `D_8009B210 & 0xF` through four modes and stores 1, 2 and 3
 * itself, and duel_card_effects.c clears it to 0 at the two points the effect
 * finishes. Four further writers are still assembly, all `sh $zero`.
 *
 * Unsigned: every read in the image is lhu, and all nine accesses are
 * gp-relative, so no unit needs a .data arm. */
extern u16 D_8009B210;

s32 DuelEffect_MarkInitialized(void);
void func_80024E4C(void);

#endif
