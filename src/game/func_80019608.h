#ifndef MEMORIES_DECOMP_FUNC_80019608_H
#define MEMORIES_DECOMP_FUNC_80019608_H

#include "../types.h"

/* D_80090998 entry 6, the card-use presentation sequence. Reached when
 * func_80024200 (src/candidates/func_80024200.c) dispatches on
 * `D_8009B23A & DUEL_SCENE_PHASE_MASK`, and it hands control
 * back by writing 5 into that word.
 *
 * On entry, while DUEL_SCENE_FLAG_INITIALIZED of D_8009B23A is clear, it latches the selected
 * slot's card id -- D_801A7AD8[object->field_6A] -- into D_8009B150, starts
 * func_80029164's resource request, bumps D_8009B1C8->field_05 for a
 * CARD_TYPE_MAGIC card, and seeds D_8009B174 with 1.
 *
 * D_8009B174 then drives five states, its low nibble selecting the step and
 * the high bits latching progress within it:
 *
 *   1  opens the enlarged card window with func_800291E0, ramps its
 *      field_21 alpha in steps of 6, releases the original object, then
 *      holds on field_60 counting down
 *   2  hands the latched id to DuelEffect_StartCardEffect with flag 0
 *   3  builds two func_80019564 copies of the window, one GsALON | GsAONE
 *      and one GsALON | GsATWO, sweeps their +0x44/+0x46 pair and their
 *      0xC colour word down together, and releases both when it reaches 0
 *   4  repeats DuelEffect_StartCardEffect with flag 1 and returns to state 5
 *
 * The previous comment here described a hand-refresh phase that walks the
 * acting side's hand slots. That is not this function: nothing here reads a
 * hand, and functions.csv and notes/research/the-game.md both already called
 * it the card-use presentation sequence. */
void func_80019608(void);

#endif
