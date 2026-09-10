#ifndef MEMORIES_DECOMP_DUEL_EFFECT_INTERACTION_STATES_H
#define MEMORIES_DECOMP_DUEL_EFFECT_INTERACTION_STATES_H

#include "../types.h"
#include "duel_effect.h"

/* Five more D_80090E64 entries: the states that wait on the player. They open
 * the choice list through Dialog_OpenChoice, run the SQUARE-button hold-to-skip
 * countdown in D_8009B32C, and release the choice object through func_8004036C
 * when the state ends. Like the rest of the table they take the state record
 * and report through its 0x51 byte. */
void func_800375A4(DuelEffectChannel *object);
void func_8003767C(DuelEffectChannel *object);
void func_8003771C(DuelEffectChannel *object);
void func_800377AC(DuelEffectChannel *object);
void func_800377C8(DuelEffectChannel *object);

/* The gate on the script state machine, armed by two of those states and
 * spent by TextBox_BuildStep.
 *
 * func_8003767C and func_8003771C set it and D_8009B27C together, to 5 and to
 * 7, and TextBox_BuildStep pumps the machine while it is set:
 *
 *     if (D_8009B357 != 0) {
 *         D_80090C50[*(u8 *)&D_8009B27C]();
 *         if (D_8009B27C == 0) { D_8009B357 = 0; }
 *     }
 *
 * so it means "D_8009B27C still has work" and is cleared when that reaches 0.
 * func_800377AC reads it the other way, ending its state once the byte is 0.
 *
 * It is declared here rather than in script_state.h, which owns D_8009B27C
 * and would otherwise be the obvious home. Both of the files that use this
 * byte spell D_8009B27C divergently -- `u16 D_8009B27C[]` in this unit and
 * `u16 D_8009B27C __attribute__((section(".data")))` in text_box_build_step.c
 * -- and script_state.h declares it plain, so including that header in either
 * file would be a conflicting redeclaration rather than a tidy-up. */
extern u8 D_8009B357;

#endif
